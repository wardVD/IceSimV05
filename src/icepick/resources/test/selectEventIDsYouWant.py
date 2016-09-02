#!/usr/bin/env python

from I3Tray import I3Tray
from os.path import expandvars
from icecube import dataio 
from icecube import icepick 

## these few calls are nothing special.  Just
## pointing things to the default locations.
testdata = expandvars("$I3_TESTDATA")
infile = testdata + "/2006data/Run00087451.i3.gz"
outfile = "selectedEventIds.i3"

workspace = expandvars("$I3_SRC")
mbids = workspace + "/phys-services/resources/doms.txt"
amageofile = workspace + "/phys-services/resources/amanda.geo"
icegeofile = workspace + "/phys-services/resources/icecube.geo"

## Define my own parameter values
#allEvents = "false"
allEvents = "true"
numEvents = 90
#-------------------------------------------------------------
## creating an instance of icetray
tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename=infile)

tray.AddModule("Dump","dump")

tray.AddModule("I3IcePickModule<I3EventIdFilter>", "eventId")(
    ("discardEvents", True),
    ("NEventsToPick", 3),
    ("EventIds", [54,578,178,586]) # Event Order noes not affect selecting events.
)

# Chose the following module to get a certain Range of EventIDs. Theres an Outbox problem
# if both filters are active though - but when would you want that?

tray.AddModule("I3Writer","writer")(
    ("filename", outfile)
    )

tray.AddModule("TrashCan", "trashCan")

if allEvents != "true":
    tray.Execute(numEvents)
else:
    tray.Execute()

tray.Finish()

