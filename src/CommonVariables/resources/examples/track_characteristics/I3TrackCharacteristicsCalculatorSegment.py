# $Id: I3TrackCharacteristicsCalculatorSegment.py 129313 2015-02-20 16:22:31Z olivas $
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

"""This python script gives an example how to use the
common_variables.track_characteristics.segments.I3TrackCharacteristicsCalculatorSegment
icetray traysegment.

"""
import math
import os

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, hdfwriter
from icecube.icetray import I3Units

from icecube.common_variables import track_characteristics

#_______________________________________________________________________________
def main():
    test_data_base_dir = os.path.expandvars('$I3_TESTDATA')
    if not os.path.exists(test_data_base_dir):
        raise RuntimeError('No test data has been downloaded, yet! '\
                           'Type "cd $I3_BUILD; make rsync" to get it!')

    filename = os.path.join(
        test_data_base_dir, 'event-viewer',
        'Level3aGCD_IC79_EEData_Run00115990.i3'
    )
    hdf_filename = os.path.join(
        os.getenv('I3_SRC'), 'CommonVariables', 'resources', 'examples',
        'track_characteristics',
        'I3TrackCharacteristicsCalculatorSegment_data.hdf'
    )

    pulses_map_name       = 'MaskedOfflinePulses'
    reco_particle_name    = 'MPEFit_SLC'
    track_cylinder_radius = 150.*I3Units.m

    print('Calculating track characteristics for "%s" '\
          'pulses and "%s" reco particle within the "%fm" track cylinder '\
          'radius.'%\
          (pulses_map_name, reco_particle_name, track_cylinder_radius))

    print('Writing HDF output file to "%s".'%(hdf_filename))

    tray = I3Tray()
    tray.AddModule("I3Reader","reader",
        Filename = filename
    )

    tableio_keys_to_book = []

    tableio_keys_to_book +=\
    tray.AddSegment(track_characteristics.I3TrackCharacteristicsCalculatorSegment, 'tc',
        PulseSeriesMapName                     = pulses_map_name,
        ParticleName                           = reco_particle_name,
        OutputI3TrackCharacteristicsValuesName = reco_particle_name+'Characteristics',
        TrackCylinderRadius                    = track_cylinder_radius,
        BookIt                                 = True
    )

    tray.AddSegment(hdfwriter.I3HDFWriter, 'hdfwriter',
        Keys            = tableio_keys_to_book,
        SubEventStreams = ['nullsplit'],
        Output          = hdf_filename
    )

    tray.AddModule("TrashCan", "trash")
    tray.Execute()
    tray.Finish()

#_______________________________________________________________________________
if __name__ == "__main__":
    main()
