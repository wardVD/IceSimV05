from math import cos, log10, isnan
from I3Tray import I3Units
from icecube import icetray, dataclasses
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms.histogram_modules.histogram_module import HistogramModule

class NuGenWeightModule(HistogramModule):

    def __init__(self):
        HistogramModule.__init__(self)
        self.frame_key = "I3MCWeightDict"
        self.append(Histogram(, , , "OneWeight"))
        self.append(Histogram(, , , "TotalInteractionProbability"))

    def DAQ(self, frame):
                
        if self.frame_key not in frame :
            return 

        weight_dict = frame[self.frame_key]

        if "OneWeight" in weight_dict:
            self.histograms["OneWeight"].fill(weight_dict["OneWeight"])
        if "TotalInteractionProbability" in weight_dict:
            self.histograms["TotalInteractionProbability"].fill(weight_dict["TotalInteractionProbability"])



