# $Id: I3DirectHitsCalculatorSegment.py 129313 2015-02-20 16:22:31Z olivas $
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
common_variables.direct_hits.segments.I3DirectHitsCalculatorSegment icetray
traysegment.

"""
import math
import os

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, hdfwriter

from icecube.common_variables import direct_hits

#_______________________________________________________________________________
def main():
    test_data_base_dir = os.path.expandvars('$I3_TESTDATA')
    if not os.path.exists(test_data_base_dir):
        raise RuntimeError('No test data has been downloaded, yet! '\
                           'Type "cd $I3_BUILD; make rsync" to get it!')

    filename = os.path.join(test_data_base_dir, 'event-viewer', 'Level3aGCD_IC79_EEData_Run00115990.i3')
    hdf_filename = os.path.join(os.getenv('I3_SRC'), 'CommonVariables', 'resources', 'examples', 'direct_hits', 'I3DirectHitsCalculatorSegment_data.hdf')

    pulses_map_name    = 'MaskedOfflinePulses'
    reco_particle_name = 'MPEFit_SLC'

    dh_defs = direct_hits.default_definitions

    print('Calculating direct hits for "%s" pulses and "%s" reco particle, using '\
          'these direct hits definitions:'%(pulses_map_name, reco_particle_name))
    for dh_def in dh_defs:
        print(dh_def)

    print('Writing HDF output file to "%s".'%(hdf_filename))

    tray = I3Tray()
    tray.AddModule("I3Reader","reader",
        Filename = filename
    )

    tableio_keys_to_book = []

    tableio_keys_to_book +=\
    tray.AddSegment(direct_hits.I3DirectHitsCalculatorSegment, 'dh',
        DirectHitsDefinitionSeries       = dh_defs,
        PulseSeriesMapName               = pulses_map_name,
        ParticleName                     = reco_particle_name,
        OutputI3DirectHitsValuesBaseName = reco_particle_name+'DirectHits',
        BookIt                           = True
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
