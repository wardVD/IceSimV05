#
# Anna Pollmann <anna.pollmann@uni-wuppertal.de>


"""This python script gives an example how to calculate the time
characteristics cut variables using the utility function
``calculate_time_characteristics`` of the
``icecube.common_variables.time_characteristics`` python module.

"""

#_______________________________________________________________________________
def main():
    import os

    from icecube import icetray, dataclasses, dataio
    from icecube.icetray import I3Units
    from icecube.common_variables import time_characteristics

    test_data_base_dir = os.path.expandvars('$I3_TESTDATA')
    if not os.path.exists(test_data_base_dir):
        raise RuntimeError('No test data has been downloaded, yet! '\
                           'Type "cd $I3_BUILD; make rsync" to get it!')

    f = dataio.I3File(os.path.join(test_data_base_dir, 'event-viewer', 'Level3aGCD_IC79_EEData_Run00115990.i3'))

    frame = f.pop_physics()

    pulses_map_name       = 'MaskedOfflinePulses'
    reco_particle_name    = 'MPEFit_SLC'
    time_cylinder_radius = 150.*I3Units.m

    print('Calculating time characteristics for "%s" pulses'%\
          (pulses_map_name))

    time_characteristics_values = time_characteristics.calculate_time_characteristics_values(
        frame['I3Geometry'],
        frame[pulses_map_name].apply(frame),
    )

    print("Calculation results: %s"%(time_characteristics_values))

    # Put the calculated values into the frame.
    frame["TrackCharacteristicsValues"] = time_characteristics_values

#_______________________________________________________________________________
if __name__ == "__main__":
    main()
