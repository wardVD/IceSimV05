'''
This makes histograms of SPEFit2
'''
import math
from math import cos, log10
import math
from I3Tray import I3Units
from icecube.production_histograms.histograms.frame_histograms import PhysicsHistogram

def generate_particle_histograms(frame_key):

    expression = "%s(frame[%s].%s)"

    _properties = [(-1, 1, 100, frame_key + "CosZenith", expression % ("cos", frame_key, "dir.zenith")), 
                   (0., 2*math.pi , 100, frame_key + "Azimuth", expression % ("", frame_key, "dir.azimuth")), 
                   (0., 10., 100, frame_key + "Energy", expression % ("log10", frame_key, "energy")), 
                   (0., 100, 100, frame_key + "Time", expression % ("log10", frame_key, "time")), 
                   (0., 10., 100, frame_key + "Speed", expression % ("", frame_key, "speed")), 
                   (0., 1000., 100, frame_key + "Length", expression % ("", frame_key, "length")),
               ]
                   
    return [PhysicsHistogram(*args) for args in _properties]
