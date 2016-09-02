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

"""
This python script compares the
common_variables.track_characteristics.I3TrackCharacteristicsCalculator
icetray module with the calculation done by the I3CutsModule.

"""
import math
import os

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, phys_services, MuonVariables
from icecube.icetray import I3Units

from icecube.common_variables import track_characteristics

n_different = 0

#_______________________________________________________________________________
def compare_results(particle_name):
    def do(frame):
        global n_different

        i3cuts_values = frame[particle_name+"Cuts_TEST"]
        cv_tc_values  = frame[particle_name+'Characteristics_TEST']
        lempty        = frame['LEmpty_TEST'].value
        separation    = frame['Separation_TEST'].value
        if(not (i3cuts_values.sall == cv_tc_values.track_hits_distribution_smoothness) and
               (lempty             == cv_tc_values.empty_hits_track_length)            and
               (separation         == cv_tc_values.track_hits_separation_length)
          ):
            n_different += 1
            print("DIFFERENT:")
            print("    sall: %f, track_hits_distribution_smoothness: %f"%\
                (i3cuts_values.sall, cv_tc_values.track_hits_distribution_smoothness))
            print("    lempty: %f, empty_hits_track_length: %f"%\
                (lempty, cv_tc_values.empty_hits_track_length))
            print("    separation: %f, track_hits_separation_length: %f"%\
                (separation, cv_tc_values.track_hits_separation_length))
    return do

#_______________________________________________________________________________
def main():
    global n_different

    test_data_base_dir = os.path.expandvars('$I3_TESTDATA')
    if not os.path.exists(test_data_base_dir):
        raise RuntimeError('No test data has been downloaded, yet! '\
                           'Type "cd $I3_BUILD; make rsync" to get it!')

    filename = os.path.join(test_data_base_dir, 'event-viewer', 'Level3aGCD_IC79_EEData_Run00115990.i3')

    pulses_map_name       = 'MaskedOfflinePulses'
    reco_particle_name    = 'MPEFit_SLC'
    # The I3CutsModule has no track cylinder radius constraint implemented, so
    # we need to set a huge radius here.
    track_cylinder_radius = 9999999*I3Units.m

    print('Calculating track characteristics for "%s" pulses and '\
          '"%s" reco particle within the track cylinder radius "%fm".'%\
          (pulses_map_name, reco_particle_name, track_cylinder_radius))

    n_different = 0

    tray = I3Tray()
    tray.AddModule("I3Reader", "reader",
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

    tray.AddModule("I3CutsModule", "i3cuts_calc",
        PulsesName          = pulses_map_name+"OnlyFirstPulse",
        ParticleNames       = reco_particle_name,
        DirectHitsTimeRange = [0, 0],
        NameTag             = "_TEST"
    )
    tray.AddModule("I3LEmptyCalculator", "i3lemptycalc",
        PulseSeriesMapName = pulses_map_name+"OnlyFirstPulse",
        ParticleName       = reco_particle_name,
        OutputDoubleName   = "LEmpty_TEST",
        CylinderRadius     = track_cylinder_radius
    )
    tray.AddModule("I3SeparationCalculator", "i3separationcalc",
        PulseSeriesMapName = pulses_map_name+"OnlyFirstPulse",
        ParticleName       = reco_particle_name,
        OutputDoubleName   = "Separation_TEST",
        CylinderRadius     = track_cylinder_radius
    )

    tray.AddModule(track_characteristics.I3TrackCharacteristicsCalculator, "cv_calc",
        PulseSeriesMapName                     = pulses_map_name+"OnlyFirstPulse",
        ParticleName                           = reco_particle_name,
        OutputI3TrackCharacteristicsValuesName = reco_particle_name+"Characteristics_TEST",
        TrackCylinderRadius                    = track_cylinder_radius
    )

    tray.AddModule(compare_results(reco_particle_name), "compare_results")

    tray.AddModule("TrashCan","trash")
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
