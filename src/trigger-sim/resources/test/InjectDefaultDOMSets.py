#!/usr/bin/env python

from os.path import expandvars

from icecube import icetray, dataclasses, dataio, trigger_sim
from I3Tray import I3Tray

tray = I3Tray()

gcd_file = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
tray.AddModule("I3Reader", Filename=gcd_file)               

tray.AddModule(trigger_sim.InjectDefaultDOMSets)

tray.AddModule("I3Writer", Filename = "outfile.i3.gz")    

tray.Execute()
tray.Finish()
