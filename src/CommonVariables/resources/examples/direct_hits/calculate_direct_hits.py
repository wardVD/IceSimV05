# $Id: calculate_direct_hits.py 129313 2015-02-20 16:22:31Z olivas $
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

"""This python script gives an example how to calculate direct hits using the
utility function.

"""

#_______________________________________________________________________________
def main():
    import os

    from icecube import icetray, dataclasses, dataio
    from icecube.common_variables import direct_hits

    test_data_base_dir = os.path.expandvars('$I3_TESTDATA')
    if not os.path.exists(test_data_base_dir):
        raise RuntimeError('No test data has been downloaded, yet! '\
                           'Type "cd $I3_BUILD; make rsync" to get it!')

    f = dataio.I3File(os.path.join(test_data_base_dir, 'event-viewer', 'Level3aGCD_IC79_EEData_Run00115990.i3'))

    frame = f.pop_physics()

    pulses_map_name    = 'MaskedOfflinePulses'
    reco_particle_name = 'MPEFit_SLC'

    dh_defs = direct_hits.default_definitions

    print('Calculating direct hits for "%s" pulses and "%s" reco particle, using '\
          'these direct hits definitions:'%(pulses_map_name, reco_particle_name))
    for dh_def in dh_defs:
        print(dh_def)

    direct_hits_map = direct_hits.calculate_direct_hits(
        dh_defs,
        frame['I3Geometry'],
        frame[pulses_map_name].apply(frame),
        frame[reco_particle_name]
    )

    print("Calculation results:")
    for dh_def_name in [dh_def.name for dh_def in dh_defs]:
        print('direct hits "%s": %s'%(dh_def_name, str(direct_hits_map[dh_def_name])))

    # Put values for one specific direct hit definition into the frame.
    frame["DirectHits_A"]  = direct_hits_map['A']

    #---------------------------------------------------------------------------
    # One can also specify specialized time windows for the direct hits
    # calculation.
    from icecube.icetray import I3Units
    dh_defs = [
        direct_hits.I3DirectHitsDefinition("my definition 1", -10*I3Units.ns, 150*I3Units.ns),
        direct_hits.I3DirectHitsDefinition("my definition 2", -5*I3Units.ns, 20*I3Units.ns),
    ]
    direct_hits_map = direct_hits.calculate_direct_hits(
        dh_defs,
        frame['I3Geometry'],
        frame[pulses_map_name].apply(frame),
        frame[reco_particle_name]
    )
    print("Calculation results:")
    for dh_def_name in [dh_def.name for dh_def in dh_defs]:
        print('direct hits "%s": %s'%(dh_def_name, direct_hits_map[dh_def_name]))

    #---------------------------------------------------------------------------
    # One can also get the list of time residuals of all direct pulses.
    dh_def = dh_defs[0]
    dh_time_residuals = direct_hits.get_direct_pulses_time_residuals(
        dh_def,
        frame['I3Geometry'],
        frame[pulses_map_name].apply(frame),
        frame[reco_particle_name]
    )
    print("Direct pulses time residuals for %s: %s"%(dh_def, str(dh_time_residuals)))

#_______________________________________________________________________________
if __name__ == "__main__":
    main()
