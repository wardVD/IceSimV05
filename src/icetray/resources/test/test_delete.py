#!/usr/bin/env python
#
# Checking that the driving time exists
#
import sys
from I3Tray import *

from icecube import icetray 

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

tray.AddModule("AddNulls", "nulls")(
    ("where", ["here", "there", "everywhere", "tonsastuff"])
    )

# verify they are there
tray.AddModule("FrameCheck","checker")(
    ("ensure_physics_has", ["here", "there", "everywhere", "tonsastuff"])
    )

tray.AddModule("Delete", "delete1")(
    ("Keys", ["here", "there"])
    )

tray.AddModule("FrameCheck","checker2")(
    ("ensure_physics_has", ["everywhere", "tonsastuff"]),
    ("ensure_physics_hasnt", ["here", "there"])
    )
# print em.   lots of nulls, dump shouldn't crash.
tray.AddModule("Dump","dump")

# throw em out
tray.AddModule("TrashCan","adios")

tray.Execute(15)
tray.Finish()

