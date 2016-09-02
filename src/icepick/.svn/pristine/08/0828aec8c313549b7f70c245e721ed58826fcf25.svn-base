#!/usr/bin/env python

from I3Tray import I3Tray

from os.path import expandvars

import os
import sys

from icecube import dataio 
from icecube import icepick 

## these few calls are nothing special.  Just
## pointing things to the default locations.

testdata = expandvars("$I3_TESTDATA")
infile = testdata + "/2006data/Run00087451.i3.gz"
outfile = "event693.i3"

workspace = expandvars("$I3_SRC")
mbids = workspace + "/phys-services/resources/doms.txt"
amageofile = workspace + "/phys-services/resources/amanda.geo"
icegeofile = workspace + "/phys-services/resources/icecube.geo"

#-------------------------------------------------------------
## creating an instance of icetray
tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename=infile)

tray.AddModule("I3IcePickModule<I3EventIdFilter>","filter")(
    ("DiscardEvents",True),
    ("NEventsToPick", 1), ##Just stop after you get the event
    ("EventIDs",[693]),
    )

tray.AddModule("Dump","dump")

#
# And this is the magic writer.  We will make it work harder later.
#
tray.AddModule("I3Writer","writer")(
    ("filename", outfile)
    )

tray.AddModule("TrashCan", "trashCan")

tray.Execute()

tray.Finish()

