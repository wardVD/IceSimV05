# $Id: calculate_hit_statistics_values.py 129313 2015-02-20 16:22:31Z olivas $
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

"""This python script gives an example how to calculate hit statistics values
using the utility function.

"""

#_______________________________________________________________________________
def main():
    import os

    from icecube import icetray, dataclasses, dataio
    from icecube.icetray import I3Units
    from icecube.common_variables import hit_statistics

    test_data_base_dir = os.path.expandvars('$I3_TESTDATA')
    if not os.path.exists(test_data_base_dir):
        raise RuntimeError('No test data has been downloaded, yet! '\
                           'Type "cd $I3_BUILD; make rsync" to get it!')

    f = dataio.I3File(os.path.join(test_data_base_dir, 'event-viewer', 'Level3aGCD_IC79_EEData_Run00115990.i3'))

    frame = f.pop_physics()

    pulses_map_name = 'MaskedOfflinePulses'

    print('Calculating hit statistics values for "%s" pulses.'%\
        (pulses_map_name))

    hit_statistics_values = hit_statistics.calculate_hit_statistics(
        frame['I3Geometry'],
        frame[pulses_map_name].apply(frame)
    )

    print("Calculation results:")
    print("COG         : %s"%(hit_statistics_values.cog))
    print("COGZSigma   : %.2f m"%(hit_statistics_values.cog_z_sigma))
    print("MinPulseTime: %.2f ns"%(hit_statistics_values.min_pulse_time/I3Units.ns))
    print("MaxPulseTime: %.2f ns"%(hit_statistics_values.max_pulse_time/I3Units.ns))
    print("QMaxDoms    : %.2f PE"%(hit_statistics_values.q_max_doms))
    print("QTotPulses  : %.2f PE"%(hit_statistics_values.q_tot_pulses))
    print("ZMin        : %.2f m"%(hit_statistics_values.z_min/I3Units.m))
    print("ZMax        : %.2f m"%(hit_statistics_values.z_max/I3Units.m))
    print("ZMean       : %.2f m"%(hit_statistics_values.z_mean))
    print("ZSigma      : %.2f m"%(hit_statistics_values.z_sigma))
    print("ZTravel     : %.2f m"%(hit_statistics_values.z_travel))

    # Put values into the frame.
    frame["MyHitStatisticsValues"] = hit_statistics_values

#_______________________________________________________________________________
if __name__ == "__main__":
    main()
