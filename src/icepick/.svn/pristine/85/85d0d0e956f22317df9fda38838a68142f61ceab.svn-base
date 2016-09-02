#!/usr/bin/env python
#
#
from I3Tray import *

from os.path import expandvars

import os
import sys

## This is just loading the libraries we'll need.  Nothing interesting here.
from icecube import dataclasses 
from icecube import dataio 
from icecube import icepick 
from icecube import icetray 

#============Need to change for each Run========================================||
testdata = expandvars("$I3_TESTDATA")
infile = testdata + "/2006data/Run00087451.i3.gz"
outfile = "selectedEvents.i3"

allEvents = "true"
numEvents = 100
#===================================================================================||

## creating an instance of icetray
tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename=infile)

# This module, in this example, will skip first 300 events and then choose following 5 events. 
tray.AddModule("I3IcePickModule<I3SkipNEventFilter>", "skip")(
    ("discardEvents", True),
    ("SkipNevents", 50),
    ("NeventStopick", 5)
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

