MPHitFilter
=============
The purpose of this module is to optimize the efficiency of coincident event simulation by filtering all event which would not produce hits in the detector. Otherwise, a majority of events would contain only one particle which is actually detected and therefore no different than a single event. One can set the threshold as a minimum number of hits. It is possible, for example to request that each event should on its own be able to trigger the detector. At present we set the threshold to 1 hit per particle.
In addition, MPHitFilter can also trim-down the I3MCTree and remove all branches whose particles don't produce enough PEs in the detector, making it much easier to read and reducing the storage requirements.

MPHitFilter has the following configurable paramters: MPHitFilter::

 CleanHitMap
   Description :  Remove hitseries for missing DOMs
   Default     :  True

 HitOMThreshold
   Description :  Minimum number of hit OMs to pass filter 
   Default     :  1

 HitSeriesMapName
   Description :  Name of HitSeriesMap object to scan
   Default     :  

 HitSeriesMapNames
   Description :  List of names of HitSeriesMap objects to scan
   Default     :  <icecube.icetray.vector_string object at 0x2a9a718c50>

 NumberOfPrimaries
   Description :  Number of primary particles to merge
   Default     :  1

 WeightMap
   Description :  map of weights to merge
   Default     :  'CorsikaWeightMap'

 WeightSumName
   Description :  Name to store weightsum in summary service
   Default     :  'WeightSum'


This module should be run after the photon propagation (ClSim or PPC). The following example uses PoissonMerger to combine events which are then propagated and their light emission has been simulated. MPHit filter then trims down the MCTree and removesEvents that did not produce light in the detector::

    from icecube.icetray import I3Units
    from icecube.diplopia import CoicidentI3ReaderService

    # Read events from file ""BACKGROUNDFILE.i3"
    background = diplopia.CoincidentI3ReaderService()
    background.open("BACKGROUNDFILE.i3")

    tray.AddModule("PoissonMerger","merge",
       BGWeights = "CorsikaWeightMap",
       CoincidentEventService = background,
     )

    tray.AddModule("Rename",keys=["I3MCTree","I3MCTree_preMuonProp"])
    tray.AddSegment(segments.PropagateMuons, "PropagateMuons",
         RandomService = randomServiceForPropagators)

    from icecube.simprod import segments
    tray.AddSegment(segments.PropagatePhotons, "normalpes",
            RandomService = "I3RandomService",
            HybridMode = False,
            IgnoreMuons = False,
            IgnoreCascades = False,
            UseGPUs = True,
            UseAllCPUCores = False,
            InputMCTree = "I3MCTree",
            UseGeant4 = False)


    tray.AddModule("MPHitFilter","hitfilter", 
                   HitOMThreshold=1, 
                   WeightMap="CorsikaWeightMap")



