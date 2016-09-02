from math import cos, log10
from I3Tray import I3Units
from icecube import icetray
from icecube import icetray, dataclasses
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms. histogram_modules.histogram_module import HistogramModule

type_to_int_dict = {dataclasses.I3Particle.EPlus : 0,
                    dataclasses.I3Particle.EMinus : 1,
                    dataclasses.I3Particle.MuPlus : 2,
                    dataclasses.I3Particle.MuMinus : 3,
                    dataclasses.I3Particle.Neutron : 4,
                    dataclasses.I3Particle.PPlus : 5,
                    dataclasses.I3Particle.PMinus : 6,
                    dataclasses.I3Particle.NuE : 7,
                    dataclasses.I3Particle.NuEBar : 8,
                    dataclasses.I3Particle.TauPlus : 9,
                    dataclasses.I3Particle.TauMinus : 10,
                    dataclasses.I3Particle.NuTau : 11,
                    dataclasses.I3Particle.NuTauBar : 12,
                    dataclasses.I3Particle.He4Nucleus : 13,
                    dataclasses.I3Particle.Li7Nucleus : 14,
                    dataclasses.I3Particle.Be9Nucleus : 15,
                    dataclasses.I3Particle.B11Nucleus : 16,
                    dataclasses.I3Particle.C12Nucleus : 17,
                    dataclasses.I3Particle.N14Nucleus : 18,
                    dataclasses.I3Particle.O16Nucleus : 19,
                    dataclasses.I3Particle.F19Nucleus : 20,
                    dataclasses.I3Particle.Ne20Nucleus : 21,
                    dataclasses.I3Particle.Na23Nucleus : 22,
                    dataclasses.I3Particle.Mg24Nucleus : 23,
                    dataclasses.I3Particle.Al27Nucleus : 24,
                    dataclasses.I3Particle.Si28Nucleus : 25,
                    dataclasses.I3Particle.P31Nucleus : 26,
                    dataclasses.I3Particle.S32Nucleus : 27,
                    dataclasses.I3Particle.Cl35Nucleus : 28,
                    dataclasses.I3Particle.Ar40Nucleus : 29,
                    dataclasses.I3Particle.K39Nucleus : 30,
                    dataclasses.I3Particle.Ca40Nucleus : 31,
                    dataclasses.I3Particle.Sc45Nucleus : 32,
                    dataclasses.I3Particle.Ti48Nucleus : 33,
                    dataclasses.I3Particle.V51Nucleus : 34,
                    dataclasses.I3Particle.Cr52Nucleus : 35,
                    dataclasses.I3Particle.Mn55Nucleus : 36,
                    dataclasses.I3Particle.Fe56Nucleus : 37,
                    dataclasses.I3Particle.Monopole : 38,
                    dataclasses.I3Particle.NuMu : 39,
                    dataclasses.I3Particle.NuMuBar : 40,
                    dataclasses.I3Particle.unknown : 41
                }

class I3MCTreePrimaryModule(HistogramModule):

    def __init__(self):
        HistogramModule.__init__(self)
        self.frame_key = "I3MCTree"
        self.append(Histogram(0, 180, 180, "PrimaryZenith"))
        self.append(Histogram(-1, 1, 100, "PrimaryCosZenith"))
        self.append(Histogram(0, 10, 50, "PrimaryEnergy"))
        self.append(Histogram(0, 42, 42, "PrimaryType"))
        self.append(Histogram(0, 10, 10, "PrimaryMultiplicity"))

    def DAQ(self, frame):
                
        if self.frame_key not in frame :
            return 

        self.histograms["PrimaryMultiplicity"].fill(len(frame[self.frame_key].primaries))

        for p in frame[self.frame_key].primaries:
            self.histograms["PrimaryZenith"].fill(p.dir.zenith/I3Units.degree)
            self.histograms["PrimaryCosZenith"].fill(cos(p.dir.zenith))
            if p.energy > 0:
                self.histograms["PrimaryEnergy"].fill(log10(p.energy/I3Units.GeV))
            int_type = type_to_int_dict[p.type] \
                       if p.type in type_to_int_dict else 41
            self.histograms["PrimaryType"].fill(int_type)


