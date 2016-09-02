#!/usr/bin/env python

from I3Tray import I3Tray

from os.path import expandvars

import os
import sys

## This is just loading the libraries we'll need.  Nothing interesting here.
from icecube import dataio 
from icecube import icepick 

## these few calls are nothing special.  Just
## pointing things to the default locations.
workspace = expandvars("$I3_SRC")
testdata = expandvars("$I3_TESTDATA")
infile = testdata + "/2006data/Run00087451.i3.gz"
outfile = "bigEvents.i3"

mbids = workspace + "/phys-services/resources/doms.txt"
amageofile = workspace + "/phys-services/resources/amanda.geo"
icegeofile = workspace + "/phys-services/resources/icecube.geo"

## Define my own parameter values
hitThreshold = 70
allEvents = "false"
numEvents = 10000
#-------------------------------------------------------------
## creating an instance of icetray
tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename=infile)

tray.AddModule("I3IcePickModule<I3PickRawNHitEventFilter>","filter")(
    ("DiscardEvents",True),
    ("HitThresholdLow", hitThreshold)
    )

tray.AddModule("Dump","dump")

#
# And this is the magic writer.  We will make it work harder later.
#
tray.AddModule("I3Writer","writer")(
    ("filename", outfile)
    )

tray.AddModule("TrashCan", "trashCan")

if allEvents != "true":
    tray.Execute(numEvents)
else:
    tray.Execute()

tray.Finish()

