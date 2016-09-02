from math import cos, log10, isnan
from I3Tray import I3Units
from icecube import icetray, dataclasses
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms.histogram_modules.histogram_module import HistogramModule

class CORSIKAWeightModule(HistogramModule):

    def __init__(self):
        HistogramModule.__init__(self)
        self.frame_key = "CorsikaWeightMap"
        self.append(Histogram(, , , "FluxSum"))
        self.append(Histogram(, , , "Weight"))

    def DAQ(self, frame):
                
        if self.frame_key not in frame :
            return 

        weight_dict = frame[self.frame_key]

        if "FluxSum" in weight_dict:
            self.histograms["FluxSum"].fill(weight_dict["FluxSum"])
        if "Weight" in weight_dict:
            self.histograms["Weight"].fill(weight_dict["Weight"])



