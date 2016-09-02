#!/usr/bin/env python
from I3Tray import *

from icecube import icetray, dataclasses, dataio, trigger_sim

from os.path import expandvars
import sys

from icecube.trigger_sim.deep_core_test_modules import TestSource, TestModule

tray = I3Tray()

gcd_file = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
tray.AddModule("I3InfiniteSource", "gcd", prefix=gcd_file, stream=icetray.I3Frame.DAQ)

tray.AddModule(TestSource, GenerateInDOMSet = True)

tray.AddModule("SimpleMajorityTrigger", TriggerConfigID = 1011)

tray.AddModule(TestModule, TriggerConfigID = 1011)
               
tray.Execute(100)
tray.Finish()

