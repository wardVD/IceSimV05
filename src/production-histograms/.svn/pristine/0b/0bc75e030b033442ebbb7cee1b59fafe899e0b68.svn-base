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
