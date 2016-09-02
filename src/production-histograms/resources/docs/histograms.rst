The Histogram Base Class
========================

If you want to do something more complicated, like traversing containers (e.g. I3MCTree),
the convenience histograms become less convenient and you'll need to go straight to the 
base class.

Note:

* **All histograms must be default constructable** - Meaning it should be possible to create an instance by calling the constructor with no arguments.
* **Implement Method** - Typically either DAQ or Physics, but the other stops are supported as well.

The noise histogram is a good example.

::

 from icecube import dataclasses, simclasses
 from icecube.production_histograms.histograms.histogram import Histogram
 
 class NoiseOccupancy(Histogram):
     '''
     Histograms the occupancy of noise PEs.
     '''
     def __init__(self):
         Histogram.__init__(self, 0, 65, 64, "NoiseOccupancy")
         self.frame_key = "I3MCPESeriesMap" 
 
     def DAQ(self, frame):
         if self.frame_key in frame:
             for omkey, pe_series in frame[self.frame_key]:
                 for pe in pe_series:
                     if pe.ID.minorID == 0 and pe.ID.majorID == 0:
                         for i in range(pe.npe):
                             self.fill(omkey.om)


