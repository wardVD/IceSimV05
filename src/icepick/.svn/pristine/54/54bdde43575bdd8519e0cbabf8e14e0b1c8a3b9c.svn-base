#!/usr/bin/env python
#
# An example script:  Will stop after 5 physics frames delivered.
#  Exercises the IcePick NEventsToPick parameter.


from I3Tray import *

from os.path import expandvars

import os
import sys

## This is just loading the libraries we'll need.  Nothing interesting here.
from icecube import icetray 
from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 
from icecube import icepick 


## these few calls are nothing special.  Just
## pointing things to the default locations.
testdata = expandvars("$I3_TESTDATA")
infile = testdata + "/2006data/Run00087451.i3.gz"
outfile = "5_Physics_events.i3"

#-------------------------------------------------------------
## creating an instance of icetray
tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename=infile)

tray.AddModule("I3IcePickModule<I3UnFilter>","nofilter")(
    ("DiscardEvents",False),
    ("NEventsToPick", 5), ##Just stop after you get the 5 event
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

