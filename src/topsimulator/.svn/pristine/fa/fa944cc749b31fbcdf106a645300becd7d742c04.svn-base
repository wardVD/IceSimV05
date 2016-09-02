"""
Convenient segments to do IceTop simulations. These are not 'official' in any way.
"""

from icecube import icetray

@icetray.traysegment
def SimulateNew(tray, name,
                InjectorServiceName,
                ResponseServiceName,
                RandomServiceName,
                PrimaryName = "MCPrimary",
                InIceMCTreeName = "I3MCTree",
                IceTopMCTreeName = "I3MCTreeIT",
                IceTopHitSeriesName = "MCTopHitSeriesMap",
                IceTopPESeriesName = "MCTopPESeriesMap",
                IceTopTestPulsesName = "",
                IceTopCherenkovHitSeriesName = "MCTopCherenkovPulses",
                CompressPEs = 2,
                Tanks = [],
                SuperNovaMode = False,
                tag = ''
             ):
    """
    Runs the main simulation modules using DOMLauncher. It has not been tested.
    """
    from icecube import simclasses, sim_services, phys_services
    from icecube import topsimulator, DOMLauncher
    from icecube.icetray import I3Units

    if tag:
        tag = '_' + tag
    
    Launches = 'IceTopRawData' + tag

    # The actual topsimulator module (depends on injector- and tank response services) 
    tray.AddModule("I3TopSimulator","I3TopSimulator",
                   InjectorServiceName = InjectorServiceName,
                   ResponseServiceName = ResponseServiceName,
                   PrimaryName = PrimaryName,
                   InIceMCTreeName = InIceMCTreeName,
                   IceTopMCTreeName = IceTopMCTreeName,
                   IceTopHitSeriesName = IceTopHitSeriesName,
                   IceTopPESeriesName = IceTopPESeriesName,
                   IceTopTestPulsesName = IceTopTestPulsesName,
                   IceTopCherenkovHitSeriesName = IceTopCherenkovHitSeriesName,
                   MuonEnergyCutOff = 273.0,
                   CompressPEs = CompressPEs,
                   Tanks = Tanks
                   )

    tray.AddModule("PMTResponseSimulator","PMTResponseSimulator",
                   Input = IceTopPESeriesName,
                   Output = IceTopPESeriesName + tag + '_Pulses',
                   RandomServiceName = RandomServiceName
                   #MergeHits = True
                   )
    tray.AddModule("DOMLauncher", "DOMLauncher",
                   Input = IceTopPESeriesName + tag + '_Pulses',
                   Output = "IceTopRawData" + tag,
                   SuperNovaMode = SuperNovaMode,
                   RandomServiceName = RandomServiceName
                   )


@icetray.traysegment
def SimulateOld(tray, name,
                InjectorServiceName,
                ResponseServiceName,
                PrimaryName = "MCPrimary",
                InIceMCTreeName = "I3MCTree",
                IceTopMCTreeName = "I3MCTreeIT",
                IceTopHitSeriesName = "MCTopHitSeriesMap",
                Tanks = [],
                tag = ''
             ):
    """
    Runs the main simulation modules using I3DOMSimulator.
    """
    from icecube import simclasses, sim_services, phys_services
    from icecube import DOMsimulator, pmt_simulator, topsimulator
    from icecube.icetray import I3Units

    if tag:
        tag = '_' + tag
    else:
        tag = ''

    Launches = 'IceTopRawData' + tag

    # The actual topsimulator module (depends on injector- and tank response services) 
    tray.AddModule("I3TopSimulator","top-simulator",
                   InjectorServiceName = InjectorServiceName,
                   ResponseServiceName = ResponseServiceName,
                   PrimaryName = PrimaryName,
                   InIceMCTreeName = InIceMCTreeName,
                   IceTopMCTreeName = IceTopMCTreeName,
                   IceTopHitSeriesName = IceTopHitSeriesName,
                   MuonEnergyCutOff=273.0,
                   Tanks = Tanks,
                   )

    tray.AddModule("I3PMTSimulator","pmt",
                   TreatIceTopAsInIce =  True,  # <-- IMPORTANT: This variable MUST be set to TRUE !!!
                   mchitseriesmapname = "MCTopHitSeriesMap",
                   pmtresponsemapname = "MCTopPMTResponseMap" + tag,
                   )

    tray.AddModule("I3DOMsimulator","domsimulator",
                   InputPMTResponse = "MCTopPMTResponseMap" + tag,
                   cleanLC = False,#Turn off LC condition
                   )


@icetray.traysegment
def CalibrateAndExtract(tray, name,
                        Launches, tag=''):
    """
    Generates calibrated charges and waveforms.

    This is here just for convenience and replicates things that are normally part of level 2.
    To use this segment WaveCalibrator and tpx must be part of the meta-project.
    """
    from icecube.icetray import I3Units
    from icecube import WaveCalibrator

    tray.AddModule("I3WaveCalibrator", "WaveCalibrator_IceTop" + tag,
                   Launches=Launches,
                   Waveforms="IceTopCalibratedWaveforms" + tag,
                   Errata="IceTopErrata" + tag,
                   WaveformRange='IceTopCalibratedWaveformRange' + tag)

    tray.AddModule('I3WaveformSplitter', 'WaveformSplitter_IceTop' + tag,
                   Force=True,
                   Input='IceTopCalibratedWaveforms' + tag,
                   PickUnsaturatedATWD=True,
                   HLC_ATWD='CalibratedIceTopATWD_HLC' + tag,
                   HLC_FADC='CalibratedIceTopFADC_HLC' + tag,
                   SLC='CalibratedIceTopATWD_SLC' + tag)

    try:
        from icecube import tpx
        tray.AddModule('I3TopHLCPulseExtractor', 'tpx_hlc' + tag,
                       Waveforms='CalibratedIceTopATWD_HLC' + tag,
                       PEPulses='IceTopPulses_HLC' + tag,
                       PulseInfo='IceTopHLCPulseInfo' + tag,
                       VEMPulses='IceTopHLCVEMPulses' + tag,
                       MinimumLeadingEdgeTime = -100.0*I3Units.ns)

        tray.AddModule('I3TopSLCPulseExtractor', 'tpx_slc' + tag,
                       Waveforms='CalibratedIceTopATWD_SLC' + tag,
                       PEPulses='IceTopPulses_SLC' + tag,
                       VEMPulses='IceTopSLCVEMPulses' + tag)
    except:
        pass

class DummySubEventSplit(icetray.I3Module):
    """
    Adds a Physics frame for each DAQ frame.

    This module exists so I can then use tableio to save things.
    Is there something for this in IceTray?
    """
    def __init__(self, ctx):
        icetray.I3Module.__init__(self, ctx)
    def Configure(self):
        self.AddOutBox("OutBox")
    def DAQ(self, daq):
        from icecube import dataclasses
        physics = icetray.I3Frame(icetray.I3Frame.Physics)
        header = dataclasses.I3EventHeader(daq['I3EventHeader'])
        header.sub_event_stream = 'IceTop'
        physics.Put('I3EventHeader', header)
        self.PushFrame(daq)
        self.PushFrame(physics)


@icetray.traysegment
def SimIceTop(tray, name,
              input,
              output=None,
              gcd=None,
              log_level=2,
              samples=1,
              tanks=[],
              x=0.0,
              y=0.0,
              r=0.0,
              seed=0,
              rng="gsl",
              injector="normal",
              response="g4",
              new=True,
              pecompress=2,
              weighted=False,
              calibrate=False,
              trigger=False):
    """
    Simulate IceTop response.

    Input can be a list of CORSIKA files or a tuple of ``(str, kwargs)``,
    which are passed to ``tray.AddService(str, "particle-injector", **kwargs)``
    to create a custom particle injector. RandomServiceName should not be
    specified, it is overridden internally.

    The option "weighted" only takes effect if the input are CORSIKA files.
    """
    import os
    import re
    from icecube import icetray, dataclasses, dataio
    from icecube.sim_services.sim_utils.gcd_utils import get_time
    from icecube.icetray import I3Units

    custom_injector = False
    if len(input) == 2 and isinstance(input[0], str) and isinstance(input[1], dict):
        custom_injector = True

    if gcd is None:
        raise ValueError("Need to specify a GCD file")
    if not os.path.exists(gcd):
        raise IOError("Specified GCD file does not exist")

    icetray.set_log_level_for_unit('I3TopSimulator', icetray.I3LogLevel(log_level))
    icetray.set_log_level_for_unit('I3CorsikaReader', icetray.I3LogLevel(log_level))
    icetray.set_log_level_for_unit('I3CorsikaInjector', icetray.I3LogLevel(log_level))
    icetray.set_log_level_for_unit('I3G4TankResponse', icetray.I3LogLevel(log_level))
    icetray.set_log_level_for_unit('I3ParamTankResponse', icetray.I3LogLevel(log_level))

    # to be used by injector
    for tag in ("inj", "resp", "other"):
        if rng == "gsl":
            tray.AddService("I3GSLRandomServiceFactory", "gslrandom_" + tag,
                            Seed=seed,
                            InstallServiceAs='random_' + tag)
        elif rng == "root":
            tray.AddService("I3TRandomServiceFactory", "rootrandom_" + tag,
                            Seed=seed,
                            InstallServiceAs='random_' + tag)
        elif rng == "sprng":
            tray.AddService("I3SPRNGRandomServiceFactory", "sprngrandom_" + tag,
                            Seed=seed,
                            NStreams=1,
                            StreamNum=0,
                            InstallServiceAs='random_' + tag)
        else:
            raise ValueError("Unknown randon number generator: " + rng)


    if custom_injector:
        tray.AddService(input[0], "particle-injector",
                        RandomServiceName = 'random_inj',
                        **input[1])
    else:
        # CORSIKA injector
        if weighted:
            if injector == "normal":
                tray.AddService("I3InjectorFactory<I3CorsikaInjector>", "particle-injector",
                                FileNameList = input,
                                RandomServiceName = 'random_inj',
                                NumSamples = samples,     # <-- Number of re-samples of the same shower
                                ImportanceSampling = True,
                                #PartDistr = os.path.join(os.path.dirname(options.output),
                                #                         'on_regions_' + os.path.basename(options.output).replace(extension, 'root')),
                                Tanks = tanks,
                                IgnoreParticleTypes = [75, 76, 85, 86, 95, 96]
                                )
            else:
                raise ValueError("option weighted requires normal injector")
        else:
            if injector == "normal":
                tray.AddService("I3InjectorFactory<I3CorsikaInjector>", "particle-injector",
                                FileNameList = input,
                                RandomServiceName = 'random_inj',
                                NumSamples = samples,     # <-- Number of re-samples of the same shower
                                RelocationX = x,  # <-- x-coordinate of core or resampling center (if Relocation R > 0)
                                RelocationY = y, # <-- y-coordinate or core or resampling center (if Relocation R > 0)
                                RelocationR = r,   # <-- Re-sampling radius (if zero --> fixed core location)
                                Tanks = tanks,
                                IgnoreParticleTypes = [75, 76, 85, 86, 95, 96]
                                )
            elif injector == "unthin":
                tray.AddService("I3InjectorFactory<I3CorsikaThinnedInjector>", "particle-injector",
                      FileNameList = input,
                      RandomServiceName = 'random_inj',
                      NumSamples = samples,     # <-- Number of re-samples of the same shower
                      RelocationX = x,  # <-- x-coordinate of core or resampling center (if Relocation R > 0)
                      RelocationY = y, # <-- y-coordinate or core or resampling center (if Relocation R > 0)
                      RelocationR = r,   # <-- Re-sampling radius (if zero --> fixed core location)
                      )
            else:
                raise ValueError("unknown injector option")

    if response == 'g4':
        from icecube import g4_tankresponse
        tray.AddService("I3IceTopResponseFactory<I3G4TankResponse>", "topresponse",
                        RandomServiceName =  "random_resp",
                        ChargeScale =  1.02
                        )
    elif response == 'param':
        tray.AddService("I3IceTopResponseFactory<I3ParamTankResponse>", "topresponse",
                        RandomServiceName =  "random_resp",
                        UseSnowParam = True
                        )
    else:
        raise ValueError("Unknown IceTop tank response: " + response)

    tray.AddModule("I3InfiniteSource", "source",
                   prefix = gcd,
                   stream = icetray.I3Frame.DAQ )

    time = get_time(dataio.I3File(gcd))
    tray.AddModule("I3MCEventHeaderGenerator","time-gen",
                   Year = time.utc_year,
                   DAQTime = time.utc_daq_time)

    if new:
        tray.AddSegment(SimulateNew, 'new_simulation',
                        InjectorServiceName = "particle-injector",
                        ResponseServiceName = "topresponse",
                        RandomServiceName = "random_other",
                        InIceMCTreeName = '',
                        Tanks = tanks,
                        CompressPEs=pecompress
                        )
    else:
        tray.AddSegment(SimulateOld, 'old_simulation',
                        InjectorServiceName = "particle-injector",
                        ResponseServiceName = "topresponse",
                        InIceMCTreeName = '',
                        Tanks = tanks
                        )

    if calibrate:
        from icecube import topsimulator
        tray.AddSegment(CalibrateAndExtract, 'CalibrateExtract',
                        Launches = 'IceTopRawData',
                        )
        tray.AddModule('I3TopAddComponentWaveforms', 'AddComponentWaveforms')

    if output is not None:  # writing of output is requested
        if output.endswith(".h5") or output.endswith(".root"):
            # write tables

            keep_keys = ['I3EventHeader',
                         'MCTopCherenkovPulses',
                         'MCTopHitSeriesMap',
                         'IceTopPulses_HLC',
                         'IceTopPulses_SLC',
                         'IceTopHLCVEMPulses',
                         'IceTopSLCVEMPulses']

            if custom_injector:
                keep_keys += ['I3MCTreeIT',
                              'MCTopPulses']
            else:
                keep_keys += ['MCPrimary',
                              'MCPrimaryInfo',
                              'SamplingWeight',
                              'Samples']

            tray.AddModule(DummySubEventSplit, 'split')

            from icecube.tableio import I3TableWriter
            if output.endswith('.h5'):
                from icecube.hdfwriter import I3HDFTableService
                hdf_service = I3HDFTableService(output)

                tray.AddModule(I3TableWriter, "writer",
                               Keys = keep_keys,
                               TableService = [ hdf_service ],
                               SubEventStreams = ['IceTop'],
                               )
            elif output.endswith('.root'):
                from icecube.rootwriter import I3ROOTTableService
                root_service = I3ROOTTableService(output)

                tray.AddModule(I3TableWriter, "writer",
                               Keys = keep_keys,
                               TableService = [ root_service ],
                               SubEventStreams = ['IceTop'],
                               )
        else:
            # write standard output format i3
            tray.AddModule("I3Writer", "i3-writer",
                           Filename = output,
                           streams = [icetray.I3Frame.DAQ]
                           )
