from math import cos, log10, isnan
from I3Tray import I3Units
from icecube import icetray, dataclasses
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms.histogram_modules.histogram_module import HistogramModule

class MCPrimary(HistogramModule):

    def __init__(self):
        HistogramModule.__init__(self)
        self.frame_key = "MCPrimary"
        self.append(Histogram(0, 180, 180, "MCPrimaryZenith"))
        self.append(Histogram(0, 360, 360, "MCPrimaryAzimuth"))
        self.append(Histogram(0, 10, 100, "MCPrimaryEnergy"))
        self.append(Histogram(0, 1000, 1000, "MCPrimaryLength"))

    def DAQ(self, frame):
                
        if self.frame_key not in frame :
            return 

        primary = frame[self.frame_key]
        
        if primary.energy > 0:
            self.histograms["MCPrimaryEnergy"].fill(log10(primary.energy/I3Units.GeV))
        self.histograms["MCPrimaryZenith"].fill(primary.dir.zenith/I3Units.degree)
        self.histograms["MCPrimaryAzimuth"].fill(primary.dir.azimuth/I3Units.degree)
        self.histograms["MCPrimaryLength"].fill(primary.length/I3Units.m)


