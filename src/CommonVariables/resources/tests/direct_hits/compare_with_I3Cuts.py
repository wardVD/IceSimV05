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
common_variables.direct_hits.I3DirectHitsCalculator icecube module with the
calculation done by the I3CutsModule.

"""
import math
import os

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, phys_services, DomTools

from icecube.common_variables import direct_hits

n_different = 0

#_______________________________________________________________________________
def compare_results(dh_defs, particle_name):
    def do(frame):
        global n_different

        for dh_def in dh_defs:
            i3cuts_values = frame[particle_name+"Cuts"+dh_def.name]
            cv_dh_values  = frame[particle_name+'DirectHits'+dh_def.name]
            if( not (
                  (i3cuts_values.ndir == cv_dh_values.n_dir_pulses) and
                  ((i3cuts_values.ldir == cv_dh_values.dir_track_length) or (math.isnan(i3cuts_values.ldir) and math.isnan(cv_dh_values.dir_track_length)) or ((i3cuts_values.ldir == 0.) and math.isnan(cv_dh_values.dir_track_length))) and
                  ((i3cuts_values.sdir == cv_dh_values.dir_track_hit_distribution_smoothness) or (math.isnan(i3cuts_values.sdir) and math.isnan(cv_dh_values.dir_track_hit_distribution_smoothness)))
                )
              ):
                n_different += 1
                print(("%s: DIFFERENT"%(dh_def.name)))
                print(("  ndir: %d, ldir: %f, sdir: %f"%(i3cuts_values.ndir, i3cuts_values.ldir, i3cuts_values.sdir)))
                print(("  %s"%(str(cv_dh_values))))
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

    dh_defs = direct_hits.default_definitions

    print(('Calculating direct hits for "%s" pulses and "%s" reco particle, using '\
          'these direct hits definitions:'%(pulses_map_name, reco_particle_name)))
    for dh_def in dh_defs:
        print(dh_def)

    tray = I3Tray()
    tray.AddModule("I3Reader","reader",
        Filename = filename
    )

    # NOTE: The I3FirstPulsifier module is needed here because the I3Cuts
    #       smoothness calculation will be done wrongly if there are more than
    #       only 1 pulse in each RecoPulseSeries!!!
    tray.AddModule("I3FirstPulsifier", "firstpulsifier",
        InputPulseSeriesMapName  = pulses_map_name,
        OutputPulseSeriesMapName = pulses_map_name+"OnlyFirstPulse",
        KeepOnlyFirstCharge      = True,
        UseMask                  = False
    )

    # Add an I3CutsModule for each direct hits definition
    for dh_def in dh_defs:
        tray.AddModule("I3CutsModule", "i3cuts_calc_direct_hits_%s"%(dh_def.name),
            PulsesName          = pulses_map_name+"OnlyFirstPulse",
            ParticleNames       = reco_particle_name,
            DirectHitsTimeRange = [dh_def.time_window.time_0, dh_def.time_window.time_1],
            NameTag             = dh_def.name
        )

    tray.AddModule(direct_hits.I3DirectHitsCalculator, "cv_calc_direct_hits",
        DirectHitsDefinitionSeries       = dh_defs,
        PulseSeriesMapName               = pulses_map_name+"OnlyFirstPulse",
        ParticleName                     = reco_particle_name,
        OutputI3DirectHitsValuesBaseName = reco_particle_name+'DirectHits'
    )

    tray.AddModule(compare_results(dh_defs, reco_particle_name), "compare_results")

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
