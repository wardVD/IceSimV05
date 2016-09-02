from icecube import dataclasses, simclasses
from icecube.production_histograms.histograms.histogram import Histogram

class SecondaryMultiplicity(Histogram):
    '''
    Histograms the multiplicity of before propagation
    '''
    def __init__(self):
        Histogram.__init__(self, 0, 10, 10, "SecondaryMultiplicity")
        self.frame_key = "I3MCTree"

    def DAQ(self, frame):
        if self.frame_key in frame:
            mctree = frame[self.frame_key]
            self.fill(len(mctree) - len(mctree.primaries))
