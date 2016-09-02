#!/usr/bin/env python

# This script tests for failure
# It only adds launches in DOMs that are not part of DOMset 4
# and ensures that no DC SMT3 trigger ever passes


from os.path import expandvars
import sys

from icecube import icetray, dataclasses, dataio, trigger_sim
from icecube.trigger_sim.deep_core_test_modules import TestSource, TestModule
from I3Tray import I3Tray

tray = I3Tray()

gcd_file = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
tray.AddModule("I3InfiniteSource",
               prefix=gcd_file,
               stream=icetray.I3Frame.DAQ)

# Generating only three DOMs is at the threshold
tray.AddModule(TestSource,
               NDOMs = 3,
               GenerateInDOMSet = True)

tray.AddModule("SimpleMajorityTrigger", TriggerConfigID = 1011)

tray.AddModule("I3GlobalTriggerSim", RunID = 0)

tray.AddModule(TestModule,
               TriggerName = 'I3TriggerHierarchy',
               TriggerConfigID = 1011)
               
               
tray.Execute(100)
tray.Finish()

