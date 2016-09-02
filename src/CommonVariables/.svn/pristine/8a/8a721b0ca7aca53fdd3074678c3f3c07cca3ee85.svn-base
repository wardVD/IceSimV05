# Anna Pollmann <anna.pollmann@uni-wuppertal.de>

"""
This python script does basic checks for the time characteristics variables since there
are no old official modules to compare them with.

"""
import math
import os

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, phys_services
from icecube.icetray import I3Units

from icecube.common_variables import time_characteristics

n_test = 0

#_______________________________________________________________________________
def test_results():
    def do(frame):
        global n_test

        values  = frame['Time_Characteristics_TEST']
        print values

        def checkInt(var, str):
            if var%1 != 0:
                print(("ERROR: The var %s is not an integer with value %f ") % (str, var))
                print values
                return 1
            return 0

        def checkLowerThreshold(var, thres, str):
            if var < thres:
                print(("ERROR: The var %s has value %f below threshold %f ") % (str, var, thres))
                print values
                return 1
            return 0

        def warnValue(var, val, str):
            if var==val:
                print (("WARN: The var %s has value %f. This is an error if this happens for all events.") % (str, var))
            return 0

        n_test+=checkInt(values.timelength_last_first, "timelength_last_first")
        n_test+=checkInt(values.zpattern, "zpattern")

        n_test+=checkLowerThreshold(values.timelength_last_first,0, "timelength_last_first")
        n_test+=checkLowerThreshold(values.timelength_fwhm,0, "timelength_fwhm")
        n_test+=checkLowerThreshold(values.timelength_maxgap,0, "timelength_maxgap")

        warnValue(values.zpattern, 0., "zpattern")

        if n_test>0:
            return 1
        return 0

    return do

#_______________________________________________________________________________
def main():
    global n_test

    test_data_base_dir = os.path.expandvars('$I3_TESTDATA')
    if not os.path.exists(test_data_base_dir):
        raise RuntimeError('No test data has been downloaded, yet! '\
                           'Type "cd $I3_BUILD; make rsync" to get it!')

    filename = os.path.join(test_data_base_dir, 'event-viewer', 'Level3aGCD_IC79_EEData_Run00115990.i3')

    pulses_map_name       = 'MaskedOfflinePulses'
    reco_particle_name    = 'MPEFit_SLC'
    # The I3CutsModule has no time cylinder radius constraint implemented, so
    # we need to set a huge radius here.
    time_cylinder_radius = 9999999*I3Units.m

    print('Calculating time characteristics for "%s" pulses.'%\
          (pulses_map_name))

    n_test = 0

    tray = I3Tray()
    tray.AddModule("I3Reader", "reader",
        Filename = filename
    )

    tray.AddModule(time_characteristics.I3TimeCharacteristicsCalculator, "cv_calc",
        PulseSeriesMapName                     = pulses_map_name,
        OutputI3TimeCharacteristicsValuesName = "Time_Characteristics_TEST",
    )

    tray.AddModule(test_results(), "test__results")

    tray.AddModule("TrashCan","trash")
    tray.Execute()
    tray.Finish()

    if n_test == 0:
        print("Test PASSED :o)")
        exit(0)
    else:
        print("Test FAILED !!!")
        exit(1)

#_______________________________________________________________________________
if __name__ == "__main__":
    main()
