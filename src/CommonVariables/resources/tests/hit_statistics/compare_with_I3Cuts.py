# $Id: compare_with_I3Cuts.py 129313 2015-02-20 16:22:31Z olivas $
#
# Copyright (C) 2012
# Martin Wolf <martin.wolf@icecube.wisc.edu>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
# Version: $Revision$
# Date:    $Date: 2015-02-20 11:22:31 -0500 (Fri, 20 Feb 2015) $
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This module is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

"""This python script compares the
common_variables.hit_statistics.I3HitStatisticsCalculator icecube module
with the calculation done by the I3CutsModule.

"""
import math
import os

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, phys_services

from icecube.common_variables import hit_statistics

n_different = 0

#_______________________________________________________________________________
def compare_results(particle_name):
    def do(frame):
        global n_different

        i3cuts_values = frame[particle_name+"CutsTEST"]
        cv_hs_values  = frame['HitStatisticsTEST']
        if( not (
              (i3cuts_values.cog.x - cv_hs_values.cog.x == 0.) and
              (i3cuts_values.cog.y - cv_hs_values.cog.y == 0.) and
              (i3cuts_values.cog.z - cv_hs_values.cog.z == 0.)
            )
          ):
            n_different += 1
            print("DIFFERENT")
            print("  i3cuts_cog_x: %f, i3cuts_cog_y: %f, i3cuts_cog_z: %f"%\
                (i3cuts_values.cog.x, i3cuts_values.cog.y, i3cuts_values.cog.z))
            print("  cvhits_cog_x: %f, cvhits_cog_y: %f, cvhits_cog_z: %f"%\
                (cv_hs_values.cog.x, cv_hs_values.cog.y, cv_hs_values.cog.z))
            print("  D_cog_x: %f, D_cog_y: %f, D_cog_z: %f"%\
                (i3cuts_values.cog.x - cv_hs_values.cog.x,
                 i3cuts_values.cog.y - cv_hs_values.cog.y,
                 i3cuts_values.cog.z - cv_hs_values.cog.z
                ))
    return do

#_______________________________________________________________________________
def main():
    global n_different

    test_data_base_dir = os.path.expandvars('$I3_TESTDATA')
    if not os.path.exists(test_data_base_dir):
        raise RuntimeError('No test data has been downloaded, yet! '\
                           'Type "cd $I3_BUILD; make rsync" to get it!')

    filename = os.path.join(test_data_base_dir, 'event-viewer', 'Level3aGCD_IC79_EEData_Run00115990.i3')

    pulses_map_name    = 'MaskedOfflinePulses'
    reco_particle_name = 'MPEFit_SLC'

    print('Calculating hit statistics values for "%s" pulses.'%\
        (pulses_map_name))

    tray = I3Tray()
    tray.AddModule("I3Reader","reader",
        Filename = filename
    )

    tray.AddModule("I3CutsModule", "i3cuts_calc_hit_statistics",
        PulsesName          = pulses_map_name,
        ParticleNames       = reco_particle_name,
        DirectHitsTimeRange = [0., 0.],
        NameTag             = "TEST"
    )

    tray.AddModule(hit_statistics.I3HitStatisticsCalculator, "cv_calc_hit_statistics",
        PulseSeriesMapName              = pulses_map_name,
        OutputI3HitStatisticsValuesName = "HitStatisticsTEST"
    )

    tray.AddModule(compare_results(reco_particle_name), "compare_results")

    tray.AddModule("TrashCan", "trash")
    tray.Execute()
    tray.Finish()

    if n_different == 0:
        print("Test PASSED :o)")
        exit(0)
    else:
        print("Test FAILED !!!")
        exit(1)

#_______________________________________________________________________________
if __name__ == "__main__":
    main()
