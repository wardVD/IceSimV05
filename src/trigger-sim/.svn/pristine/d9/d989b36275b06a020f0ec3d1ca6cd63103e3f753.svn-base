#!/usr/bin/env python
import unittest

from I3Tray import *

from icecube import icetray, dataclasses, dataio, trigger_sim

from os.path import expandvars
import sys

def frame_setup(frame):
    frame["I3Triggers"] = dataclasses.I3TriggerHierarchy()

    dom_launch = dataclasses.I3DOMLaunch()
    dom_launch.time = 42*I3Units.ns

    dom_launch_series = dataclasses.I3DOMLaunchSeries()
    dom_launch_series.append(dom_launch)

    launch_map = dataclasses.I3DOMLaunchSeriesMap()
    launch_map[icetray.OMKey(21,30)] = dom_launch_series
                        
    frame["InIceRawData"] = launch_map

TIME_INCREMENT = 100*I3Units.ns
    
class TestGlobalTriggerSim(unittest.TestCase):

    event_header = None

    def test_time_increment(self):
        print self.frame
        self.assertTrue( self.frame.Has("I3EventHeader"))
        event_header = self.frame["I3EventHeader"]
        if self.event_header :
            time_difference = event_header.start_time - self.event_header.start_time
            print("time difference = %f" % time_difference)
            print("  %s" % str(event_header.start_time))
            print("  %s" % str(self.event_header.start_time))
            self.assertTrue(time_difference/I3Units.ns - TIME_INCREMENT < 1*I3Units.picosecond, 
                            "TimeIncrement is not working correctly.")
        self.event_header = event_header


tray = I3Tray()

gcd_file = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
tray.AddModule("I3InfiniteSource",
               prefix=gcd_file,
               stream=icetray.I3Frame.DAQ)

tray.AddModule(frame_setup, Streams = [icetray.I3Frame.DAQ])

tray.AddModule("I3GlobalTriggerSim",
               I3DOMLaunchSeriesMapNames = ["InIceRawData"],
               TimeIncrement = TIME_INCREMENT,
               FilterMode = False,
               RunID = 0)

tray.AddModule(icetray.I3TestModuleFactory(TestGlobalTriggerSim),
               Streams = [icetray.I3Frame.DAQ])

tray.Execute(100)
tray.Finish()

