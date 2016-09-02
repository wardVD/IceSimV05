#!/usr/bin/env python
from I3Tray import *

from icecube import icetray, dataclasses, dataio, trigger_sim

from os.path import expandvars
import sys

from icecube.trigger_sim.inice_test_modules import TestSource, TestModule

tray = I3Tray()

gcd_file = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
tray.AddModule("I3InfiniteSource", prefix=gcd_file, stream=icetray.I3Frame.DAQ)

tray.AddModule(TestSource, NDOMs = 8)               

tray.AddModule("SimpleMajorityTrigger", TriggerConfigID = 1006)

tray.AddModule("I3GlobalTriggerSim", RunID = 0)

tray.AddModule(TestModule, TriggerName = 'I3TriggerHierarchy')
                              
tray.Execute(100)
tray.Finish()

print("PASSED")
