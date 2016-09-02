#!/usr/bin/env python

from optparse import OptionParser
from os.path import expandvars

usage = "usage: %prog [options] inputfile"
parser = OptionParser(usage)
parser.add_option("-o", "--outfile",default="test_genie.i3",
                  dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz} format)")
parser.add_option("-s", "--seed",type="int",default=12395,
                  dest="SEED", help="Initial seed for the random number generator")
parser.add_option("-r", "--runnumber", type="int", default=1,
                  dest="RUNNUMBER", help="The run number for this simulation")
parser.add_option("-n", "--numevents", type="int", default=10,
                  dest="NUMEVENTS", help="The number of events per run")

# parse cmd line args, bail out if anything is not understood
(options,args) = parser.parse_args()
if len(args) != 0:
        crap = "Got undefined options:"
        for a in args:
                crap += a
                crap += " "
        parser.error(crap)

from I3Tray import *
import os
import sys

from icecube import icetray, dataclasses, dataio, phys_services, sim_services
from icecube import genie_icetray

tray = I3Tray()

# a random number generator
randomService = phys_services.I3SPRNGRandomService(
    seed = options.SEED,
    nstreams = 10000,
    streamnum = options.RUNNUMBER)

tray.AddModule("I3InfiniteSource","streams",
                Stream=icetray.I3Frame.DAQ)

tray.AddModule("I3GENIEGenerator","genie_generator",
    RandomService = randomService, # alternatively, this can be None and the I3RandomService can be installed using tray.AddService()
    SplineFilename = expandvars("/cvmfs/icecube.opensciencegrid.org/data/genie-splines/splines_water_2.8.6.xml"),
    LHAPDFPath = expandvars("$I3_SRC/genie-icetray/resources/PDFsets"),
    NuEnergyMin = 10.*I3Units.GeV,
    NuEnergyMax = 20.*I3Units.GeV,
    PowerLawIndex = 2., # E^-2 spectrum
    GenVolRadius = 1200.*I3Units.m,
    GenVolLength = 2000.*I3Units.m,
    GenVolDepth = 1950.*I3Units.m,
    NeutrinoFlavor = "NuMu", # generates neutrinos and anti-neutrinos (1:1)
    MaterialDensity = 0.93*I3Units.g/I3Units.cm3, # ice density
    TargetMixIngredients = [1000080160,1000010010], # O16, H1
    TargetMixQuantities = [1,2], # H2O (O16->1x, H1->2x)
    ForceSingleProbScale = False,
    NEvents = options.NUMEVENTS)

#tray.AddModule("I3Writer","writer",
#    Filename = options.OUTFILE)

tray.AddModule("TrashCan", "the can")

tray.Execute(options.NUMEVENTS)
tray.Finish()
