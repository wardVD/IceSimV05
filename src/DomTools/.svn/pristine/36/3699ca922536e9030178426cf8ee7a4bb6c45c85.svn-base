#!/usr/bin/env python

from optparse import OptionParser

parser = OptionParser(usage=usage)

parser.add_option("-i","--infiles", 
                  dest="INFILES", 
                  help="Infile regex",
                  default = )

parser.add_option("-o","--outfile", 
                  dest="OUTFILE", 
                  help="Outfile name.",
                  default = "output.i3.bz2")

(options, args) = parser.parse_args()

import sys, glob
from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, DomTools

infiles = glob.glob(options.INFILES)

tray = I3Tray() 

tray.Add("I3Reader", filenamelist = infiles)

tray.Add("I3FirstPulsifier",
         InputPulseSeriesMapName = "",
         OutputPulseSeriesMapName = "FirstPulsified",
         KeepOnlyFirstCharge = False,
         UseMask = False
     )

tray.Add("I3Writer", filename = option.OUTFILE)

tray.Execute(100)
tray.Finish()
