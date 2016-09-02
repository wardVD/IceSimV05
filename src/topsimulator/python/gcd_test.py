'''
Tests GCD-sanity for the use in simprod.

This is called by sim-services. The file and
the functions defined here may not be renamed.
'''

from icecube.i3math import isnan
from icecube import dataclasses as dc
from icecube.icetray import I3Units


def topsimulator_test(omkey, tank, domcal, vemcal, domstat):
    # we are only called for IceTop OMs

    def fail(reason):
        print("  {0}: {1}".format(omkey, reason))
        return False

    # check tank position
    if tank.position.mag2 == 0 or isnan(tank.position.mag2):
        fail("bad tank position " + str(tank.position))

    # check liner
    if tank.tanktype == dc.I3TankGeo.NotSet:
        fail("tanktype not set")

    # check tank geometry
    if tank.tankradius <= 0.0 or isnan(tank.tankradius):
        fail("bad tankradius = %g" % tank.tankradius)

    if tank.tankheight < 1.0 * I3Units.meter or isnan(tank.tankradius):
        fail("bad tankheight = %g" % tank.tankheight)

    # check ice height
    if (tank.fillheight < 0.5 * I3Units.meter or
        tank.fillheight >= tank.tankheight or
        isnan(tank.fillheight)):
        fail("bad fillheight = %g" % tank.fillheight)

    # check snow height
    if tank.snowheight < 0.0 or isnan(tank.snowheight):
        fail("bad snowheight = %g" % tank.snowheight)

    # check pe_per_vem
    if vemcal.pe_per_vem <= 0.0 or isnan(vemcal.pe_per_vem):
        fail("bad pe_per_vem = %g" % vemcal.pe_per_vem)

    # check corr_factor
    if vemcal.corr_factor <= 0.0 or isnan(vemcal.corr_factor):
        fail("bad corr_factor = %g" % vemcal.corr_factor)

    # check pmt_hv > 0
    if domstat.pmt_hv <= 0.0 or isnan(domstat.pmt_hv):
        fial("bad pmt_hv = %g" % domstat.pmt_hv)

    return True
