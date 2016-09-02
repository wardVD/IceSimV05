# $Id: calculate_hit_multiplicity_values.py 129313 2015-02-20 16:22:31Z olivas $
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

"""This python script gives an example how to calculate hit multiplicity values
using the utility function.

"""

#_______________________________________________________________________________
def main():
    import os

    from icecube import icetray, dataclasses, dataio
    from icecube.common_variables import hit_multiplicity

    test_data_base_dir = os.path.expandvars('$I3_TESTDATA')
    if not os.path.exists(test_data_base_dir):
        raise RuntimeError('No test data has been downloaded, yet! '\
                           'Type "cd $I3_BUILD; make rsync" to get it!')

    f = dataio.I3File(os.path.join(test_data_base_dir, 'event-viewer', 'Level3aGCD_IC79_EEData_Run00115990.i3'))

    frame = f.pop_physics()

    pulses_map_name = 'MaskedOfflinePulses'

    print(('Calculating hit multiplicity values for "%s" pulses.'%
        (pulses_map_name)))

    hit_multiplicity_values = hit_multiplicity.calculate_hit_multiplicity(
        frame['I3Geometry'],
        frame[pulses_map_name].apply(frame)
    )

    print("Calculation results:")
    print("NHitStrings     : %d"%(hit_multiplicity_values.n_hit_strings))
    print("NHitDoms        : %d"%(hit_multiplicity_values.n_hit_doms))
    print("NHitDomsOnePulse: %d"%(hit_multiplicity_values.n_hit_doms_one_pulse))
    print("NPulses         : %d"%(hit_multiplicity_values.n_pulses))

    # Put values into the frame.
    frame["MyHitMultiplicityValues"] = hit_multiplicity_values

#_______________________________________________________________________________
if __name__ == "__main__":
    main()