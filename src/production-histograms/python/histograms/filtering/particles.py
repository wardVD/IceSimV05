'''
This makes histograms of different track reconstructions. Sanity Checker
Zenith, azimuth, Energy, and one parameter per reco
'''
from icecube.production_histograms.histograms.frame_histograms import PhysicsHistogram
from icecube.production_histograms.histograms.particle_histogram_generator import generate_particle_histograms

_l1_particle_keys = ["LineFit", "PoleMuonLineFit", "PoleMuonLlhFit"]

l1_particle_histograms = list()
for frame_key in _l1_particle_keys:
    l1_particle_histograms.extend(generate_particle_histograms(frame_key))
        
