from math import isnan
from icecube import dataclasses

def vuvuzela_test(omkey, i3omgeo, cal):

    if i3omgeo.omtype != dataclasses.I3OMGeo.OMType.IceCube :
        return True
    
    if isnan(cal.dom_noise_thermal_rate) \
       or isnan(cal.dom_noise_decay_rate) \
       or isnan(cal.dom_noise_scintillation_hits) \
       or isnan(cal.dom_noise_scintillation_mean) \
       or isnan(cal.dom_noise_scintillation_sigma) :

        print('  %s  vuvuzela DOM noise parameters !!' % (str(omkey)) )
        print('     thermal rate = %.2f' % cal.dom_noise_thermal_rate )
        print('     decay rate = %.2f' % cal.dom_noise_decay_rate )
        print('     scintillation rate = %.2f' % cal.dom_noise_scintillation_hits )
        print('     scintillation mean = %.2f' % cal.dom_noise_scintillation_mean )
        print('     scintillation sigma = %.2f' % cal.dom_noise_scintillation_sigma )
        
        return False
    return True
