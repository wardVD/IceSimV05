'''
This makes histograms of SPEFit2
'''
import math
from math import cos, log10
from I3Tray import I3Units
from icecube.production_histograms.histograms.frame_histograms import PhysicsHistogram
from icecube.production_histograms.histograms.particle_histogram_generator import generate_particle_histograms

_l2_particle_keys = ["FiniteRecoFit", 
                    "LineFit",
                    "SPEFit2",
                    "SPEFit2MuEX_FSS",
                    "SPEFitSingle"]

l2_particle_histograms = list()
for frame_key in _l2_particle_keys:
    l2_particle_histograms.extend(generate_particle_histograms(frame_key))
