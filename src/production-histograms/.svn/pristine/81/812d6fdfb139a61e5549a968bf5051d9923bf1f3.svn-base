from math import cos, log10
from I3Tray import I3Units
from icecube import icetray
from icecube import icetray, dataclasses, simclasses
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms. histogram_modules.histogram_module import HistogramModule

type_to_int_dict = {"UNKNOWN": 0,
                    "SMT3" : 1,
                    "SMT8" : 2,
                    "SMT6" : 3,
                    "STRING" : 4,
                    "VOLUME" : 5,
                    "SLOP" : 6,
                    "GLOBAL_MERGED" : 7,
                    "GLOBAL_THROUGHPUT" : 8
                    }

class TriggerModule(HistogramModule):

    def __init__(self):
        HistogramModule.__init__(self)
        self.frame_key = "I3TriggerHierarchy"
        self.append(Histogram(0, 9, 9, "Triggers"))
        self.append(Histogram(10000, 12000, 100, "SMT3Time"))
        self.append(Histogram(0, 10000, 100, "SMT3Length"))
        self.append(Histogram(10000, 15000, 100, "SMT8Time"))
        self.append(Histogram(0, 10000, 100, "SMT8Length"))
        self.append(Histogram(10000, 100000, 100, "SMT6Time"))
        self.append(Histogram(0, 10000, 100, "SMT6Length"))
        self.append(Histogram(10000, 16000, 100, "StringTime"))
        self.append(Histogram(0, 2000, 100, "StringLength"))
        self.append(Histogram(10000, 16000, 100, "VolumeTime"))
        self.append(Histogram(0, 1000, 100, "VolumeLength"))
        self.append(Histogram(10000, 100000, 100, "SLOPTime"))
        self.append(Histogram(0, 100000, 100, "SLOPLength"))
        self.append(Histogram(10000, 100000, 100, "GlobalThroughputTime"))
        self.append(Histogram(10000, 100000, 100, "GlobalMergedTime"))

    def Physics(self, frame):
                
        if self.frame_key not in frame:
            return 

        detector_status = frame["I3DetectorStatus"]
        trigger_status = detector_status.trigger_status
            
        triggers = frame[self.frame_key]
        for trigger in triggers:
            if trigger.key.type == dataclasses.I3Trigger.SIMPLE_MULTIPLICITY:
                if trigger.key.source == dataclasses.I3Trigger.ICE_TOP:
                    self.histograms["SMT6Time"].fill(trigger.time)
                    self.histograms["SMT6Length"].fill(trigger.length)
                    self.histograms["Triggers"].fill(type_to_int_dict["SMT6"])
                else:
                    # need to figure out whether it's SMT3 or SMT8
                    if trigger.key.config_id == 1006 :
                        # it's SMT8.  this is valid all the way back to IC79.
                        self.histograms["SMT8Time"].fill(trigger.time)
                        self.histograms["SMT8Length"].fill(trigger.length)
                        self.histograms["Triggers"].fill(type_to_int_dict["SMT8"])
                    else:
                        self.histograms["SMT3Time"].fill(trigger.time)
                        self.histograms["SMT3Length"].fill(trigger.length)
                        self.histograms["Triggers"].fill(type_to_int_dict["SMT3"])
                        
            if trigger.key.type == dataclasses.I3Trigger.STRING:
                self.histograms["StringTime"].fill(trigger.time)
                self.histograms["StringLength"].fill(trigger.length)
                self.histograms["Triggers"].fill(type_to_int_dict["STRING"])
            if trigger.key.type == dataclasses.I3Trigger.VOLUME:
                self.histograms["VolumeTime"].fill(trigger.time)
                self.histograms["VolumeLength"].fill(trigger.length)
                self.histograms["Triggers"].fill(type_to_int_dict["VOLUME"])
            if trigger.key.type == dataclasses.I3Trigger.SLOW_PARTICLE:
                self.histograms["SLOPTime"].fill(trigger.time)
                self.histograms["SLOPLength"].fill(trigger.length)
                self.histograms["Triggers"].fill(type_to_int_dict["SLOP"])
            if trigger.key.source == dataclasses.I3Trigger.GLOBAL:
                if trigger.key.type == dataclasses.I3Trigger.MERGED:
                    self.histograms["GlobalMergedTime"].fill(trigger.time)
                if trigger.key.type == dataclasses.I3Trigger.THROUGHPUT:
                    self.histograms["GlobalThroughputTime"].fill(trigger.time)
