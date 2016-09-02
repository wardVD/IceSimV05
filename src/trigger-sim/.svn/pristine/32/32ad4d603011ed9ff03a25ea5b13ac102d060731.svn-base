#!/usr/bin/env python

# This script tests for failure
# It only adds launches in DOMs that are not part of DOMset 4
# and ensures that no DC SMT3 trigger ever passes

from I3Tray import *

from icecube import icetray, dataclasses, dataio, trigger_sim

from os.path import expandvars
import sys

from icecube.trigger_sim.deep_core_test_modules import TestSource, TestModule

tray = I3Tray()

gcd_file = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
tray.AddModule("I3InfiniteSource", prefix=gcd_file, stream=icetray.I3Frame.DAQ)

# generating DOMs outside the DC SMT3 time window
# this test should fail
tray.AddModule(TestSource,
               GenerateInDOMSet = True,
               TimeWindow = 5 * I3Units.microsecond, 
               GenerateInTimeWindow = False)

# we can't filter events in this case
tray.AddModule("SimpleMajorityTrigger", TriggerConfigID = 1011)

# by setting Trigger to False we're testing that
# the this configuration never triggers the detector
tray.AddModule(TestModule, Trigger = False)
  
tray.Execute(100)
tray.Finish()

