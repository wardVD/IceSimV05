from math import cos, log10
from I3Tray import I3Units
from icecube import icetray
from icecube import icetray, dataclasses
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms. histogram_modules.histogram_module import HistogramModule

class I3MCPEModule(HistogramModule):

    def __init__(self):
        HistogramModule.__init__(self)
        self.frame_key = "I3MCPESeriesMap"
        self.append(Histogram(1, 65, 64, "PEDOMOccup"))
        self.append(Histogram(0, 1000, 1000, "NMCPEChan"))
        self.append(Histogram(-10000, 100000, 1000, "MCPETime"))
        self.append(Histogram(0, 1000, 100, "MCPEs"))

    def DAQ(self, frame):
                
        if self.frame_key not in frame:
            return 

        pemap = frame[self.frame_key]
        nchannel = 0
        self.histograms["NMCPEChan"].fill(len(pemap))
        for omkey, mcpeseries in pemap:
            self.histograms["PEDOMOccup"].fill(omkey.om)

            for mcpe in mcpeseries:
                self.histograms["MCPETime"].fill(mcpe.time)
                self.histograms["MCPEs"].fill(mcpe.npe)
                if(mcpe.ID.majorID == 0 and mcpe.ID.minorID == 0):
                    nchannel +=1
                    break




