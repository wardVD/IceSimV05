#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Example script for wimpsim-reader
"""

import os, sys, glob
import random, numpy
from optparse import OptionParser

# standard imports
from I3Tray import *
from icecube import icetray, dataclasses, dataio

# imports for wimpsim-reader
from icecube import simclasses, wimpsim_reader, phys_services

outfile = os.path.expandars("$I3_BUILD/wimpsim-reader/resources/example.i3.bz2")
icetray.logging.log_notice("Output is written to %s"%(outfile))

#You'll need a tray
tray = I3Tray()

# You'll gonna need a random service for the wimpsim-reader, configure a name that can be passed on
tray.AddService("I3SPRNGRandomServiceFactory","Random",
  NStreams = 2,
  Seed = options.Seed,
  StreamNum = 1,
  InstallServiceAs = "Random") #<--- Name of Service

#Add the reader, here for sun events
tray.AddModule("I3WimpSimReader","wimpsim-reader",
  RandomServiceName = "Random", #<---- Name of random service
  FileNameList = [os.path.expandvars('$I3_BUILD/wimpsim-reader/resources/test_data_sun.dat')], #<--- a list of WimpSim-files to read
  NEvents = 0, #default 0
  StartMJD = NAN, #default NAN
  EndMJD = NAN, #default NAN #
  Oversampling = 0, #default 0
  PositionLimits = [-800,800,-800,800,-800,800], #default [-800,800,-800,800,-800,800]
  InjectionRadius = 700*I3Units.meter, #default 0
  LowerZenCut = 0.*I3Units.degree, #default 0*I3Units.degree,
  UpperZenCut = 180.*I3Units.degree, #default 180*I3Units.degree
  SensitiveHeight = 1300.*I3Units.meter, #default 0*I3Units.meter
  SensitiveRadius = 700.*I3Units.meter, #default 0*I3Units.meter
  UseElectrons = True, #default False
  UseMuons = True, #default True
  UseTaus = True, #default False
  UseNC = True, #default False
  )

# Output from the Reader is a stream of frames IQQQ...QI, where one event corresponds to Q-frame
# Leading and concluding Info-frames contain some useful information and event summaries
def prettyprint(frame):
  icetray.logging.log_info("=====================")
  icetray.logging.log_info(str(frame["I3EventHeader"]))
  icetray.logging.log_info(str(frame["I3WimpParams"]))
  icetray.logging.log_info(str(frame["I3MCTree"]))
tray.AddModule(prettyprint, "print",
  Streams = [icetray.I3Frame.DAQ])

#have a look at the outfile with "dataio-pyshovel $I3_BUILD/wimpsim-reader/resources/example.i3.bz2"
tray.AddModule("I3Writer","writer",
  #streams = [icetray.I3Frame.DAQ,icetray.I3Frame.Physics],
  filename = outfile)

#after the wimpsim-reader the normal simulation chain needs to be placed, that is: ParticleMC, PhotonMC, DetectorMC
#Have a look at simprod-scripts for inspiration

tray.AddModule("TrashCan","trashcan")
tray.Execute()  
tray.Finish()
