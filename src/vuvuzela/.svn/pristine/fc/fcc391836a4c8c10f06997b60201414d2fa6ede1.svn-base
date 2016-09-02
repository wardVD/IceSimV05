#!/usr/bin/env python

from optparse import OptionParser
import os
import string

usage = "usage: %prog [options] inputfile"
parser = OptionParser(usage)

parser.add_option("-i", "--infile", default=None,
                  dest="INFILE", help="Input GCD file")
parser.add_option("-o", "--outfile", default=None,
                  dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz} format)")
parser.add_option("-t", "--textfile", default=None,
                  dest="TEXTFILE", help="Input text file containing the vuvuzela noise parameters.")

# parse cmd line args, bail out if anything is not understood
(options,args) = parser.parse_args()

outdir=""
outfile=None
if options.OUTFILE:
        outfile = options.OUTFILE
        # did the user specify a directory? then use that and auto-generate
        if os.path.isdir(outfile):
            outdir = outfile
            outfile = None
        else:
            outdir, outfile = os.path.split(outfile)

# add a trailing slash to the output directory name if not already there
if outdir and outdir!="":
    if outdir[-1] != "/":
        outdir += "/"

if not outfile:
        # automatically generate the output filename
        outfile = "TEST_NoiseInjected.i3"
print("output dir is %s" % outdir)
print("output file is %s" % outdir + outfile)

########################


from icecube import icetray, dataclasses, dataio, vuvuzela
from I3Tray import *

tray = I3Tray()

tray.AddModule("I3Reader","reader",
               Filename=options.INFILE)

tray.AddModule("Inject", "InjectNoiseParams",
	       InputNoiseFile = os.path.expandvars("$I3_SRC/vuvuzela/resources/data/parameters.dat"),
	       )

tray.AddModule("I3Writer","writer",
    Filename = outdir+outfile)

tray.AddModule("TrashCan", "the can")

tray.Execute(10)
tray.Finish()
