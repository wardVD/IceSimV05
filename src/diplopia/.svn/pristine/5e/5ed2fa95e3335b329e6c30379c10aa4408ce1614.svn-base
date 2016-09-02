I3PolyplopiaExp
===============

The name was chosen to reflect its ability to merge any number of primaries.
I3PolyplopiaExp combines adjecent events to fall within a time window with random times sampled from an exponential distribution 
with a mean time :math:`\tau`. given as a configuration parameter. If two or more
events fall within the time window :math:`\Delta t`, they are combined into a single event.

To produce accidental coincidences from background cosmic-rays::

    from icecube.icetray import I3Units
    from icecube import diplopia

    tray.AddModule("I3PolyplopiaExp","merge",
         MMCTrackListName = "MMCTrackList",
         MCTreeName = "I3MCTree",
         Rate = 5.5*I3Units.kilohertz,
         WeightSumName = "WeightSum",
         WeightMap = "CorsikaWeightMap",
         TimeWindow = 40.0*I3Units.microsecond,
     )

