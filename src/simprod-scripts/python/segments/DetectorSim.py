#!/usr/bin/env python

from icecube import icetray, dataclasses

def ConvertToLinearizedMCTree(frame):
    if 'I3MCTree' in frame:
        try:
            tree = dataclasses.I3LinearizedMCTree(frame['I3MCTree'])
        except:
            logger.error('cannot convert to I3LinearizedMCTree')
        else:
            del frame['I3MCTree']
            frame['I3MCTree'] = tree
    return True

@icetray.traysegment
def DetectorSim(tray, name,
    RandomService = None,
    RunID = None,
    GCDFile = None,
    KeepMCHits = False,
    KeepPropagatedMCTree = False,
    KeepMCPulses = False,
    SkipNoiseGenerator = False,
    LowMem = False,
    InputPESeriesMapName = "I3MCPESeriesMap",
    BeaconLaunches = True,
    TimeShiftSkipKeys=[],
    FilterTrigger=True):
    
    from icecube import icetray, dataclasses, dataio, phys_services
    from icecube import trigger_sim

    from I3Tray import I3Units

    from icecube import DOMLauncher
    from icecube import topsimulator

    if not RunID:
        icetray.logging.log_fatal("You *must* set a RunID in production.")

    MCPESeriesMapNames = [
        InputPESeriesMapName,
        "BackgroundI3MCPESeriesMap",
        "SignalI3MCPEs" 
        ]
    MCPulseSeriesMapNames = [
        "I3MCPulseSeriesMap",
        "I3MCPulseSeriesMapParticleIDMap" 
        ]
    MCTreeNames = [
        "I3MCTree",
        "BackgroundI3MCTree"
        ]
    MCPMTResponseMapNames = []

    if not SkipNoiseGenerator:
        InputPESeriesMapName_withoutNoise = InputPESeriesMapName + "WithoutNoise"
        tray.Add("Rename", "RenamePESeriesMap",
                 Keys=[InputPESeriesMapName, InputPESeriesMapName_withoutNoise])
        MCPESeriesMapNames.append(InputPESeriesMapName_withoutNoise)

        from icecube import vuvuzela
        
        tray.AddSegment(vuvuzela.AddNoise, name+"_vuvuzela",
            OutputName = InputPESeriesMapName,
            InputName = InputPESeriesMapName_withoutNoise,
            StartTime = -10.*I3Units.microsecond,
            EndTime = 10.*I3Units.microsecond,
            RandomServiceName = RandomService,
            )

    tray.AddSegment(DOMLauncher.DetectorResponse, "DetectorResponse",
        pmt_config = {'Input':InputPESeriesMapName,
                      'Output':"I3MCPulseSeriesMap",
                      'MergeHits':True,
                      'LowMem':LowMem,
                      'RandomServiceName' : RandomService},
        dom_config = {'Input':'I3MCPulseSeriesMap',
                      'Output':"I3DOMLaunchSeriesMap",
                      'UseTabulatedPT':True,
                      'RandomServiceName' : RandomService,
                      'BeaconLaunches':BeaconLaunches})
     

    timeshiftargs={'SkipKeys':TimeShiftSkipKeys}
    tray.AddSegment(trigger_sim.TriggerSim,
                    name+'_triggersim',
                    gcd_file=dataio.I3File(GCDFile), # for trigger auto-configuration
                    run_id = RunID,
                    prune = True,
                    time_shift = True,
                    time_shift_args = timeshiftargs,
                    filter_mode = FilterTrigger
                    )

    tray.AddModule('I3TopAddComponentWaveforms', 'AddComponentWaveforms',
                   PESeriesMap='I3MCPESeriesMap',
                   Waveforms="")
    


    tray.AddModule("Delete", name+"_cleanup",
        Keys = ["MCTimeIncEventID",
                "MCPMTResponseMap",
                ])

    if not KeepMCPulses:
        tray.AddModule("Delete", name+"_cleanup_2",
            Keys = MCPulseSeriesMapNames + MCPMTResponseMapNames)

    if not KeepMCHits:
        tray.AddModule("Delete", name+"_cleanup_I3MCHits_2",
            Keys = MCPESeriesMapNames)

    if not KeepPropagatedMCTree: # Always keep original tree
        tray.AddModule("Delete", name+"_cleanup_I3MCTree_3",
            Keys = MCTreeNames)
    

@icetray.traysegment
def DetectorSegment(tray,name,If=lambda f:True,
                     gcdfile='',
                     summaryfile='',
                     mctype='corsika_weighted',
                     MCPESeriesMapName='I3MCPESeriesMap',
                     detector_label='IC86:2012',
                     runtrigger=True,
                     filtertrigger=True,
                     stats={},
                     basicHisto=False,
                     icetop=False,
                     genie=False,
                     prescale=1,
                     uselineartree=True,
                     lowmem=False,
                     sanitychecker=False,
                     BeaconLaunches=True,
                     TimeShiftSkipKeys=[],
                     GeneratedEfficiency=0.0,
                     SampleEfficiency=0.0,
                     RunID=None,
                     KeepMCHits = False,
                     KeepPropagatedMCTree = False,
                     KeepMCPulses = False,
                     ):
    """
    Run IC86 detector simulation
    """
    from .. import segments
    
    # Combine MCPEs from both detectors
    if genie:
       tray.Add("Rename", Keys=[MCPESeriesMapName, 'GenieMCPEs'])
       tray.Add("I3CombineMCPE", 
               InputResponses = ["GenieMCPEs", "BackgroundMCPEs"],
               OutputResponse = MCPESeriesMapName)
       tray.Add("Delete", Keys=['BackgroundMCPEs','GenieMCPEs']) 

    if icetop:
       tray.Add("Rename", Keys=[MCPESeriesMapName, 'InIceMCPEs'])
       tray.Add("I3CombineMCPE", 
               InputResponses = ["IceTopMCPEs", "InIceMCPEs"],
               OutputResponse = MCPESeriesMapName)
       tray.Add("Delete", Keys=['InIceMCPEs', 'IceTopMCPEs'])


    # Sample a different efficiency
    if SampleEfficiency > 0.0:
        if SampleEfficiency > GeneratedEfficiency: 
            logger.fatal(
              'Cannot upscale from GeneratedEfficiency %s to SampleEfficiency %s' % (
                    SampleEfficiency, GeneratedEfficiency))

        tray.AddSegment(segments.MultiDomEffSample,"resample",
            GeneratedEfficiency=GeneratedEfficiency,
            SampleEfficiencies=[SampleEfficiency],
            InputSeriesName=MCPESeriesMapName,
            DeleteOriginalSeries=True,
            OverwriteOriginalSeries=True,
        )

    tray.AddSegment(DetectorSim, "DetectorSim",
        RandomService = 'I3RandomService',
        GCDFile = gcdfile,
        InputPESeriesMapName = MCPESeriesMapName,
        KeepMCHits = KeepMCHits,
        KeepMCPulses = KeepMCPulses,
        KeepPropagatedMCTree = KeepPropagatedMCTree,
        LowMem = lowmem,
        BeaconLaunches=BeaconLaunches,
        SkipNoiseGenerator = False,
        TimeShiftSkipKeys = TimeShiftSkipKeys,
        FilterTrigger=filtertrigger,
        RunID=RunID)

    from ..util import BasicCounter, DAQCounter
    tray.AddModule(BasicCounter,"count_triggers", 
                    Streams = [icetray.I3Frame.DAQ] ,
                    name="%s Triggered Events" % detector_label,
                    Stats=stats)

    skipkeys = [ "I3Triggers", "EnhancementFactor", "MCPMTResponseMap", "MCTimeIncEventID"]

    skipkeys += ["IceTopRawData_unused","MCPMTResponseMap","MCTopHitSeriesMap"]
    if "NKGInfo" in skipkeys:   # Keep NKGInfo for IceTop
             skipkeys.remove("NKGInfo")

    if sanitychecker: # Sanity Check
        print("ERROR: The sanity checkers have been temporarily moved out"
              " of simulation.  They'll be back shortly (mid-Nov 2015 at the latest).")
        #from icecube.sim_services.sanity_checker import SimulationSanityChecker
        #tray.AddModule( SimulationSanityChecker, "sanitycheck", RunType = mctype, OutputFilename="sanityfile.pkl")
            
    if uselineartree:
        tray.AddModule(ConvertToLinearizedMCTree,"lineartree",streams=[icetray.I3Frame.DAQ])




