#!/usr/bin/env python

import os
import sys

if(os.getenv("I3_BUILD") == None):
    print("I3_BUILD not set.")
    sys.exit()

from I3Tray import *

if(len(sys.argv)!=4):
    print("Use: eff.py [infile] [outfile] [eff]")
    sys.exit()

infile  =  sys.argv[1]
outfile =  sys.argv[2]
eff  = float(sys.argv[3])

load("libdataio")
load("libppc-eff")
load("simclasses") # not necessary

tray = I3Tray()

tray.AddModule("I3Reader", "reader")(
    ("FileNameList", [infile])
    )

tray.AddModule("AdjEff", "eff")(
    ("eff", eff)
    )

tray.AddModule("I3Writer", "writer")(
    ("filename", outfile)
    )

tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()
