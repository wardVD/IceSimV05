'''
Below classes for making histograms of pulses.
These classes use common_variables.
'''
from icecube.production_histograms.histograms.frame_histograms import PhysicsHistogram

expression = '''%s(frame['CVStatistics'].%s)'''

# argument properties for histogram construction
_properties = [(0,100,100,"QTotPulses", expression % ("log10","q_tot_pulses")), 
               (0,100,100,"COGPulses", expression % ("","cog.z")), 
               (0,100,100,"COGZSigmaPulses", expression % ("","cog_z_sigma")), 
               (0,100,100,"MinPulseTime", expression % ("log10","min_pulse_time/I3Units.ns")), 
               (0,100,100,"MaxPulseTime", expression % ("log10","max_pulse_time/I3Units.ns")), 
               (0,100,100,"QMaxDomsPulses", expression % ("log10","q_max_doms")), 
               (0,100,100,"ZMinPulses", expression % ("log10","z_min/I3Units.m")), 
               (0,100,100,"ZMaxPulses", expression % ("log10","z_max/I3Units.m")), 
               (0,100,100,"ZMeanPulses", expression % ("log10","z_mean/I3Units.m")), 
               (0,100,100,"ZSigmaPulses", expression % ("log10","z_sigma")), 
               (0,100,100,"ZTravelPulses", expression % ("log10","z_travel_pulses")), 
           ]

expression = '''frame['CVMultiplicity'].%s'''
_properties.append((0,100,100,"NHitStrings", expression % "n_hit_strings"))
_properties.append((0,100,100,"NHitDoms", expression % "n_hit_doms")) 
_properties.append((0,100,100,"NHitDomsOnePulse", expression % "n_hit_doms_one_pulse")) 
_properties.append((0,100,100,"NPulses", expression % "n_pulses")) 

cv_histograms = [PhysicsHistogram(*args) for args in _properties]
                 




