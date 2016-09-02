from math import cos, log10
from I3Tray import I3Units
from icecube import icetray
from icecube import icetray, dataclasses, simclasses
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms. histogram_modules.histogram_module import HistogramModule

class InIceResponseModule(HistogramModule):

    def __init__(self):
        HistogramModule.__init__(self)
        self.frame_key = "InIceRawData"
        self.append(Histogram(0, 1000, 1000, "InIceDOMNChannel"))
        self.append(Histogram(1, 10, 10, "InIceDOMLaunchPerChannel"))
        self.append(Histogram(1, 61, 60, "InIceDOMOccupancy"))
        self.append(Histogram(-10000, 100000, 1000, "InIceDOMLaunchTime"))

    def DAQ(self, frame):
                
        if self.frame_key not in frame:
            return 

        launch_map = frame[self.frame_key]
        self.histograms["InIceDOMNChannel"].fill(len(launch_map))
        for omkey, launch_series in launch_map:
            self.histograms["InIceDOMOccupancy"].fill(omkey.om)
            self.histograms["InIceDOMLaunchPerChannel"].fill(len(launch_series))

            for launch in launch_series:
                self.histograms["InIceDOMLaunchTime"].fill(launch.time)



