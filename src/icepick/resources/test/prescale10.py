#!/usr/bin/env python
#
#
from I3Tray import *

from os.path import expandvars

import os
import sys

# libs
from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 
from icecube import icepick 
from icecube import icetray 


# dirs
workspace = expandvars("$I3_BUILD")
testdata = expandvars("$I3_TESTDATA")

# files
infile = testdata + "/2006data/Run00087451.i3.gz"
outfile = workspace + "/icepick/prescale10.i3"

#######################################################################

## creating an instance of icetray
tray = I3Tray()

# The muxer is a sort of black box which makes frames.
# You just hope that it does what you think it should do.
tray.AddModule("I3Reader", "reader", Filename=infile, SkipKeys=["I3PfFilterMask"])

# This module, in this example, will keep only every 10th event
tray.AddModule("I3IcePickModule<I3PrescaleFilter>", "prescale")(
    ("DiscardEvents", True),
    ("PrescaleFactor", 10),
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
