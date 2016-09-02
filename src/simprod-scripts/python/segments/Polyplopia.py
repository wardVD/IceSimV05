from I3Tray import *
from icecube import icetray, dataclasses, dataio
from icecube.icetray import I3Frame
from icecube.icetray import traysegment
import os.path 


def SetMultiplicity(frame,mctreelist=[],weightmap="CorsikaWeightMap"):
    if weightmap  in frame:
        wm = frame[weightmap]
    else:
        wm = dataclasses.I3MapStringDouble()
    multiplicity = 0
    for t in mctreelist:
        multiplicity += len(frame[t].primaries)
        #print t, len(frame[t].primaries)
    wm["Multiplicity"] = multiplicity
    if weightmap  in frame:
       frame.Delete(weightmap)
    frame.Put(weightmap,wm)
    return True



@traysegment
def PolyplopiaSegment(tray, name,
                    mctype='CORSIKA',
                    RandomService=None,
                    mctree_name = "I3MCTree_preMuonProp",
                    separate_coincident_mctree_name = "", # leave empty to combine
                    bgfile = None,
                    timewindow = 40.*I3Units.microsecond,
                    rate = float('nan'),
                    If=lambda f: True
   ):

      """
         There are three scenarios for polyplopia:
            1. bgfile: We are reading background MC from a separate file and
            injecting events to signal (or weighted background) based on 
            a Poisson distribution within the given time window.

            2. we are generating MuonGun bundles and
            injecting events to signal (or weighted background) based on 
            a Poisson distribution within the given time window.
      """

      from icecube import polyplopia, MuonGun

      #tray.AddModule("ParticleMapper","mapprimary") 
      if bgfile: # merge bg into signal
          background = polyplopia.CoincidentI3ReaderService()
          background.open(bgfile)

      else:
          # Use MuonGun
          # Default: use Hoerandel as a template for generating muons.
          model = MuonGun.load_model("Hoerandel5_atmod12_SIBYLL")

          # Generate bundles (even if not 100 percent accurate).
          model.flux.max_multiplicity = 100

          gamma_index=2.6
          energy_offset=700.
          energy_min=1e4
          energy_max=1e7
          cylinder_length=1600.
          cylinder_radius=800.
          cylinder_x=0.
          cylinder_y=0.
          cylinder_z=0.

          # Default: cylinder aligned with z-axis at detector center as injection
          # surface.
          outsurface_center = dataclasses.I3Position(
               cylinder_x*I3Units.m,
               cylinder_y*I3Units.m,
               cylinder_z*I3Units.m)

          outsurface = MuonGun.Cylinder(
               length=cylinder_length*I3Units.m,
               radius=cylinder_radius*I3Units.m,
               center=outsurface_center)

          # Draw energies from a power law with offset.
          spectrum = MuonGun.OffsetPowerLaw(
               gamma=gamma_index,
               offset=energy_offset*I3Units.GeV,
               min=energy_min*I3Units.GeV,
               max=energy_max*I3Units.GeV)

          generator = MuonGun.StaticSurfaceInjector(
               outsurface, model.flux, spectrum, model.radius)

          background = polyplopia.MuonGunBackgroundService()
          background.set_generator(generator)
          background.set_rng(RandomService)
          background.set_rate(rate)
          background.set_mctree_name(mctree_name)

      tray.AddModule("PoissonMerger","merge",
          CoincidentEventService = background,
          PrimaryType = mctype,
          MCTreeName = mctree_name,
          Rate = rate,
          SeparateMCTree = separate_coincident_mctree_name,
          TimeWindow = timewindow)

      return tray



@traysegment
def PolyplopiaPhotons(tray, name,
                    mctype='CORSIKA',
                    RandomService=None,
                    mctree_name = "I3MCTree",
                    bgfile = None,
                    IceModel = "SpiceLea",
                    IceModelLocation = os.path.expandvars("$I3_BUILD/clsim/resources/ice"),
                    timewindow = 40.*I3Units.microsecond,
                    rate = float('nan'),
                    GPU = None,
                    UseGPUs = True,
                    DOMOversizeFactor = 5,
                    UseHoleIceParameterization = True,
                    MaxParallelEvents = 100,
                    TotalEnergyToProcess = 0,
                    Efficiency = 0.99,
                    PhotonSeriesName = "I3MCPESeriesMap",
                    PROPOSALParams=dict(),
                    UsePPC=False,
                    If=lambda f: True
   ):

        from .. import segments
        from I3Tray import I3Units

        tray.AddSegment(segments.PolyplopiaSegment,"coincify",
               RandomService=RandomService,
               mctype=mctype,
               mctree_name = mctree_name,
               separate_coincident_mctree_name = "BackgroundI3MCTree_preMuonProp",
               bgfile = bgfile,
               timewindow = timewindow,
               rate = rate,
           ) 

        tray.AddModule("Rename","rename_mmc", Keys=['MMCTrackList','SignalMMCTrackList'])

        tray.AddSegment(segments.PropagateMuons, 'propagator',
               RandomService= RandomService,
               SaveState=False,
               InputMCTreeName="BackgroundI3MCTree_preMuonProp",
               OutputMCTreeName="BackgroundI3MCTree",
               **PROPOSALParams
        ) 

        from .. import segments 
        from ..util import choose_max_efficiency
        # Propagate photons
        if type(Efficiency) == list or type(Efficiency) == tuple:
           if len(Efficiency) == 1:
              _efficiency=float(Efficiency[0])
           elif len(Efficiency) > 1:
              _efficiency=map(float,Efficiency)
           elif len(Efficiency) < 1:
              raise Exception("Configured empty efficiency list")
        else:
            _efficiency = choose_max_efficiency(Efficiency)
         

        if GPU is not None:
           os.putenv("CUDA_VISIBLE_DEVICES",str(GPU))
           os.putenv("COMPUTE",":0."+str(GPU))
           os.putenv("GPU_DEVICE_ORDINAL",str(GPU))

        tray.AddModule("Rename","rename_pes", Keys=[PhotonSeriesName,'SignalI3MCPEs'])

        if UsePPC:
           from ..modules.ppc import PPCTraySegment
           tray.AddSegment(PPCTraySegment,"ppc_photons",
			  gpu = -1,
			  usegpus = UseGPUs,
			  efficiency = _efficiency,
			  oversize = DOMOversizeFactor,
			  IceModelLocation = IceModelLocation.replace('clsim','ppc'),
			  IceModel = IceModel,
			  volumecyl = True,
			  gpulib = 'opencl',
			  InputMCTree="BackgroundI3MCTree",
			  keep_empty_events = True,
			  mcpeseries = "BackgroundI3MCPESeriesMap")


        else: # use clsim
          tray.AddSegment(segments.PropagatePhotons, "normalpes",
            InputMCTree="BackgroundI3MCTree",
            OutputPESeriesMapName="BackgroundI3MCPESeriesMap",
            RandomService = RandomService,
            HybridMode = False,
            IgnoreMuons = False,
            IgnoreCascades = False,
            UseCascadeExtension = False,
            UseGPUs = UseGPUs,
            UseAllCPUCores = False,
            IceModel         = IceModel,
            IceModelLocation = IceModelLocation,
            UnshadowedFraction = _efficiency,
            DOMOversizeFactor = DOMOversizeFactor,
            UseGeant4 = False,
            UseHoleIceParameterization = UseHoleIceParameterization,
            MaxParallelEvents = MaxParallelEvents,
            TotalEnergyToProcess = TotalEnergyToProcess)

        from icecube import polyplopia
        if mctype.lower() =='corsika':
           WeightMap="CorsikaWeightMap"
        else:
           WeightMap="I3MCWeightDict"

        tray.AddModule("MPHitFilter","hitfilter",
              HitOMThreshold=1,
              RemoveBackgroundOnly=False,
              I3MCPESeriesMapName="BackgroundI3MCPESeriesMap",
              MCTreeName="BackgroundI3MCTree",
              PruneTree=True,
              Filter=False)

        tray.Add(SetMultiplicity,
             mctreelist=[mctree_name,"BackgroundI3MCTree"],
             weightmap="PolyplopiaInfo",
             Streams=[icetray.I3Frame.DAQ])

        # Add to CorsikaWeightMap because people are used to finding it there
        tray.Add(SetMultiplicity,
             mctreelist=[mctree_name,"BackgroundI3MCTree"],
             weightmap=WeightMap,
             Streams=[icetray.I3Frame.DAQ])

        tray.AddModule("Rename","rename_mmc1", Keys=['MMCTrackList','BackgroundMMCTrackList'])
        tray.AddModule("Rename","rename_mmc2", Keys=['SignalMMCTrackList','MMCTrackList'])

        tray.AddModule("I3CombineMCPE","combinepes",
            InputResponses = ['SignalI3MCPEs','BackgroundI3MCPESeriesMap'],
            OutputResponse = PhotonSeriesName
        )


