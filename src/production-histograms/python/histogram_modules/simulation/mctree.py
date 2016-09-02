from math import cos, log10
from I3Tray import I3Units
from icecube import icetray, dataclasses
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms. histogram_modules.histogram_module import HistogramModule

type_to_int_dict = {dataclasses.I3Particle.Gamma : 0,
                    dataclasses.I3Particle.EPlus : 1,
                    dataclasses.I3Particle.EMinus : 2,
                    dataclasses.I3Particle.MuPlus : 3,
                    dataclasses.I3Particle.MuMinus : 4,
                    dataclasses.I3Particle.Pi0 : 5,
                    dataclasses.I3Particle.PiPlus : 6,
                    dataclasses.I3Particle.PiMinus : 7,
                    dataclasses.I3Particle.K0_Long : 8,
                    dataclasses.I3Particle.KPlus : 9,
                    dataclasses.I3Particle.KMinus : 10,
                    dataclasses.I3Particle.Neutron : 11,
                    dataclasses.I3Particle.PPlus : 12,
                    dataclasses.I3Particle.PMinus : 13,
                    dataclasses.I3Particle.K0_Short : 14,
                    dataclasses.I3Particle.NuE : 15,
                    dataclasses.I3Particle.NuEBar : 16,
                    dataclasses.I3Particle.NuMu : 17,
                    dataclasses.I3Particle.NuMuBar : 18,
                    dataclasses.I3Particle.TauPlus : 19,
                    dataclasses.I3Particle.TauMinus : 20,
                    dataclasses.I3Particle.NuTau : 21,
                    dataclasses.I3Particle.NuTauBar : 22,
                    dataclasses.I3Particle.CherenkovPhoton : 23,
                    dataclasses.I3Particle.Brems : 24,
                    dataclasses.I3Particle.DeltaE : 25,
                    dataclasses.I3Particle.PairProd : 26,
                    dataclasses.I3Particle.NuclInt : 27,
                    dataclasses.I3Particle.Hadrons : 28,
                    dataclasses.I3Particle.unknown : 29
                    }

class I3MCTreeModule(HistogramModule):

    def __init__(self):
        HistogramModule.__init__(self)
        self.frame_key = "I3MCTree"
        self.append(Histogram(0, 100, 100, "NMu"))
        self.append(Histogram(0, 180, 180, "SecondaryZenith"))
        self.append(Histogram(-1, 1, 100, "SecondaryCosZenith"))
        self.append(Histogram(0, 10, 50, "SecondaryEnergy"))
        self.append(Histogram(0, 42, 42, "SecondaryType"))
        self.append(Histogram(0, 10, 100, "CascadeEnergy"))
        self.append(Histogram(0, 1000, 1000, "MuonLength"))
        self.append(Histogram(0, 100, 100, "TauLength"))
        self.append(Histogram(2, 5, 100, "LogMostEnergeticMuon"))

    def DAQ(self, frame):

        if self.frame_key not in frame :
            return

        mctree = frame[self.frame_key]

        try:
            mem = mctree.most_energetic_muon
        except AttributeError:
            from icecube.dataclasses import get_most_energetic_muon
            mem = get_most_energetic_muon(mctree)

        if mem and mem.energy > 0:
            self.histograms["LogMostEnergeticMuon"].fill(log10(mem.energy/I3Units.GeV))

        for p in mctree:
            if p.is_cascade and p.energy > 0:
                self.histograms["CascadeEnergy"].fill(log10(p.energy))

            if (p.type == dataclasses.I3Particle.MuMinus or
                p.type == dataclasses.I3Particle.MuPlus):
                self.histograms["NMu"].fill(1)
                self.histograms["MuonLength"].fill(p.length/I3Units.m)

            if (p.type == dataclasses.I3Particle.TauMinus or
                p.type == dataclasses.I3Particle.TauPlus):
                self.histograms["TauLength"].fill(p.length/I3Units.m)

            if mctree.depth(p) > 0:
                self.histograms["SecondaryZenith"].fill(p.dir.zenith/I3Units.degree)
                self.histograms["SecondaryCosZenith"].fill(cos(p.dir.zenith))
                if p.energy > 0:
                    self.histograms["SecondaryEnergy"].fill(log10(p.energy/I3Units.MeV))
                int_type = type_to_int_dict[p.type] \
                           if p.type in type_to_int_dict else 29
                self.histograms["SecondaryType"].fill(int_type)
