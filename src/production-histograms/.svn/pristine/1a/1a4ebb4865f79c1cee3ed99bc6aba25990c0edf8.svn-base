from math import cos, log10
from I3Tray import I3Units
from icecube import icetray
from icecube import icetray, dataclasses, simclasses
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms. histogram_modules.histogram_module import HistogramModule

type_to_int_dict = {simclasses.I3MCPulse.UNKNOWN : 0,
                    simclasses.I3MCPulse.PE : 1,
                    simclasses.I3MCPulse.RANDOM : 2,
                    simclasses.I3MCPulse.AFTER_PULSE : 3,
                    simclasses.I3MCPulse.PRE_PULSE : 4,
                    simclasses.I3MCPulse.ELASTIC_LATE_PULSE : 5,
                    simclasses.I3MCPulse.INELASTIC_LATE_PULSE : 6,
                    simclasses.I3MCPulse.EARLY_AFTER_PULSE : 7
                   }

class PMTResponseModule(HistogramModule):

    def __init__(self):
        HistogramModule.__init__(self)
        self.frame_key = "I3MCPulseSeriesMap"
        self.mctree_frame_key = "I3MCTree"
        self.append(Histogram(0, 1000, 1000, "PulseNChannel"))
        self.append(Histogram(1, 65, 64, "PulseOccupancy"))
        self.append(Histogram(-10000, 100000, 1000, "PulseTime"))
        self.append(Histogram(0, 20, 100, "PulseCharge"))
        self.append(Histogram(0, 4, 100, "LogQtot"))
        self.append(Histogram(0, 4, 100, "LogQtotMultiPrimary"))
        self.append(Histogram(0, 8, 8, "PulseSource"))

    def DAQ(self, frame):
                
        if self.frame_key not in frame:
            return 

        pulse_map = frame[self.frame_key]
        self.histograms["PulseNChannel"].fill(len(pulse_map))
        total_charge = 0
        for omkey, pulse_series in pulse_map:
            self.histograms["PulseOccupancy"].fill(omkey.om)            
            for pulse in pulse_series:
                self.histograms["PulseTime"].fill(pulse.time)
                self.histograms["PulseCharge"].fill(pulse.charge)
                total_charge += pulse.charge

            int_type = type_to_int_dict[pulse.source]
            self.histograms["PulseSource"].fill(int_type)

        if total_charge > 0:
            self.histograms["LogQtot"].fill(log10(total_charge))
            
            if self.mctree_frame_key in frame:
                if len(frame[self.mctree_frame_key].primaries) > 1:
                    self.histograms["LogQtotMultiPrimary"].fill(log10(total_charge))


