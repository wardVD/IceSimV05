Histogram Modules
=================

If you want to histogram several quantities in a frame container, it's going to be more
efficient to create a HistogramModule.  This way you avoid multiple loops over the
same object.

Note:

* **Create and add** the histogram in the constructor.
* **Implement Method** - Typically either DAQ or Physics, but the other stops are supported as well.  In this method you're going to call the fill method of the appropriate histogram.  Histograms are stored in an internal dictionary called 'histograms.'

The I3MCTreeModule is a good example.

Minmial example:
::
   
 class I3MCTreeModule(HistogramModule):
     def __init__(self):
         HistogramModule.__init__(self)
         self.append(Histogram(0, 100, 100, "TreeSize"))
 
     def DAQ(self, frame):                
         mctree = frame['I3MCTree']        
         self.histograms["TreeSize"].fill(len(mctree))

More complicated example:
::

 from math import cos, log10
 from I3Tray import I3Units
 from icecube import icetray, dataclasses
 from icecube.production_histograms.histograms.histogram import Histogram
 from icecube.production_histograms. histogram_modules.histogram_module import HistogramModule
   
 class I3MCTreeModule(HistogramModule):
 
     def __init__(self):
         HistogramModule.__init__(self)
         self.frame_key = "I3MCTree"
         self.append(Histogram(0, 100, 100, "TreeSize"))
         self.append(Histogram(0, 10, 100, "CascadeEnergy"))
         self.append(Histogram(0, 1000, 1000, "MuonLength"))
         self.append(Histogram(0, 100, 100, "TauLength"))
 
    def DAQ(self, frame):
                
        if self.frame_key not in frame :
            return 
 
        mctree = frame[self.frame_key]
        
        self.histograms["TreeSize"].fill(len(mctree))
 
        for p in mctree:
            if p.is_cascade and p.energy > 0:
                self.histograms["CascadeEnergy"].fill(log10(p.energy))
 
            if (p.type == dataclasses.I3Particle.MuMinus or 
                p.type == dataclasses.I3Particle.MuPlus):
                self.histograms["MuonLength"].fill(p.length/I3Units.m)
 
            if (p.type == dataclasses.I3Particle.TauMinus or 
                p.type == dataclasses.I3Particle.TauPlus):
                self.histograms["TauLength"].fill(p.length/I3Units.m)
