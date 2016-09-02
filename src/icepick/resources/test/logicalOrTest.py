#!/usr/bin/env python
from I3Tray import *
from os.path import expandvars

from icecube.icepick.test_modules.LogicalOrTestModule import I3LogicalOrTestModule

from icecube import icetray 
from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 
from icecube import icepick 

tray = I3Tray()

tray.AddModule("I3Reader","reader",Filename=expandvars("$I3_TESTDATA")+"/string-21/Linux-i386.i3.gz")

tray.AddModule("I3IcePickModule<I3EventIdFilter>","id1")(
    ("EventIDs",[3]),
    ("DiscardEvents",False),
    ("DecisionName","id1"))

tray.AddModule("I3IcePickModule<I3EventIdFilter>","id2")(
    ("EventIDs",[5]),
    ("DiscardEvents",False),
    ("DecisionName","id2"))

tray.AddModule("I3IcePickModule<I3LogicalOrFilter>","or")(
    ("DiscardEvents",True),
    ("FirstDecisionName","id1"),
    ("SecondDecisionName","id2"))

tray.AddModule(I3LogicalOrTestModule,"ortest",
    FirstEventID = 3,
    SecondEventID = 5)

tray.AddModule("TrashCan","trash")

tray.Execute()
tray.Finish()
