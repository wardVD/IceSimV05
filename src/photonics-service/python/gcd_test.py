# I3PhotonicsHitMaker doesn't simulate IceTop
from math import isnan
from icecube import dataclasses as dc
def photonics_hit_maker_test(omkey, geo, cal):
    if geo.omtype == dc.I3OMGeo.OMType.IceTop : return True

    # needs a non-NAN Relative DOM Efficiency
    if isnan(cal.relative_dom_eff) :
        print('  %s  relative_dom_eff = %s !!' \
              % (str(omkey), cal.relative_dom_eff))
        return False
    return True
