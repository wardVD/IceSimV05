#!/usr/bin/env python
from I3Tray import *

from icecube import icetray, dataclasses, dataio, trigger_sim

from os.path import expandvars
import sys

from icecube.trigger_sim.inice_test_modules import TestSource, TestModule

tray = I3Tray()

gcd_file = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
tray.AddModule("I3InfiniteSource", prefix=gcd_file, stream=icetray.I3Frame.DAQ)

tray.AddModule(TestSource,
    NDOMs = 5,
    DOMType = dataclasses.I3OMGeo.IceTop)

tray.AddModule("SimpleMajorityTrigger",
    TriggerSource = dataclasses.I3Trigger.ICE_TOP,
    TriggerConfigID = 102
    )

#this event should not trigger
tray.AddModule(TestModule,
    Trigger = False, 
    TriggerSourceID = dataclasses.I3Trigger.ICE_TOP,
    TriggerConfigID = 102
    )
               
tray.Execute(100)
tray.Finish()

