#!/usr/bin/env python

from os.path import expandvars

import os
import sys

from optparse import OptionParser

parser = OptionParser()
parser.add_option("-f","--format",dest="format",help="format to output [hdf5, root, or csv]",default='hdf5')
parser.add_option("-z","--compress",dest="compression",help="compression level",default=1,type=int)
parser.add_option("-n","--frames",dest="nframes",help="number of frames to process",default=None,type=int)

options,args = parser.parse_args()

if len(args) < 1:
    parser.error("You must supply at least one input filename")
infiles = args
outfile = os.path.basename(args[0]) + '.' + options.format

#print infiles
print("writing to %s" % outfile)

from I3Tray import *
from icecube import icetray, dataclasses, dataio, phys_services, tableio
from icecube import genie_icetray
from icecube import tableio

if options.format == 'hdf5':
    from icecube import hdfwriter
    tabler = hdfwriter.I3HDFTableService(outfile,options.compression)
elif options.format == 'root':
    from icecube import rootwriter
    tabler = rootwriter.I3ROOTTableService(outfile,"MasterTree",options.compression)
elif options.format == 'csv':
    from icecube import tableio
    tabler = tableio.I3CSVTableService(outfile[:-4] + '_csv')
else:
    raise ValueError("I don't have a writer service for format '%s'"%options.format)


tray = I3Tray()

tray.AddModule("I3Reader","reader",
    FilenameList = infiles)
    
tray.AddModule(tableio.I3TableWriter,'writer', 
               tableservice = tabler,
               keys = [dict(key="I3MCTree", name="I3MCTree"),
                       dict(key="I3MCWeightDict", name="I3MCWeightDict")
                       ])

tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()
