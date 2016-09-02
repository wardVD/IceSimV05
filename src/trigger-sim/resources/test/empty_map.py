#!/usr/bin/env python

import os
from icecube import icetray, dataclasses, dataio, trigger_sim, phys_services
from I3Tray import I3Tray

from os.path import expandvars

if os.environ.get("I3_TESTDATA") :
    i3_testdata = expandvars("$I3_TESTDATA")
else :
    i3_testdata = expandvars("$I3_TESTDATA")

def dummy_trig( frame ):
    gt = dataclasses.I3TriggerHierarchy()
    gt.add_root( dataclasses.I3Trigger() )
    frame["I3TriggerHierarchy"] = gt

tray = I3Tray()

tray.AddModule("I3InfiniteSource", "source", 
               prefix = i3_testdata + "/sim/GeoCalibDetectorStatus_IC86.55697_corrected.i3.gz")
tray.AddModule(dummy_trig, "trigger", Streams = [icetray.I3Frame.DAQ])
tray.AddModule("I3Pruner","prune")

tray.AddModule("TrashCan","trash")

tray.Execute(10)
