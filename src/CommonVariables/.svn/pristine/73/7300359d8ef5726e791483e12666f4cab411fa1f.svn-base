# Anna Pollmann <anna.pollmann@uni-wuppertal.de>



"""This python script gives an example how to use the
common_variables.time_characteristics.segments.I3TimeCharacteristicsCalculatorSegment
icetray traysegment.

"""
import math
import os

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, hdfwriter
from icecube.icetray import I3Units

from icecube.common_variables import time_characteristics

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
        'time_characteristics',
        'I3TimeCharacteristicsCalculatorSegment_data.hdf'
    )

    pulses_map_name       = 'MaskedOfflinePulses'
    reco_particle_name    = 'MPEFit_SLC'
    time_cylinder_radius = 150.*I3Units.m

    print('Calculating time characteristics for "%s" '\
          'pulses' %\
          (pulses_map_name))

    print('Writing HDF output file to "%s".'%(hdf_filename))

    tray = I3Tray()
    tray.AddModule("I3Reader","reader",
        Filename = filename
    )

    tableio_keys_to_book = []

    tableio_keys_to_book +=\
    tray.AddSegment(time_characteristics.I3TimeCharacteristicsCalculatorSegment, 'tc',
        PulseSeriesMapName                     = pulses_map_name,
        OutputI3TimeCharacteristicsValuesName = reco_particle_name+'Characteristics',
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
