#!/usr/bin/env python

"""
Just sum all Vgen in this file and count the number of events
"""

import os, sys, getopt, glob, math
from icecube import icetray, dataclasses
from optparse import OptionParser

class RunParameters:
  def __init__(self):
    self.Infile = ''

#___________________IMPLEMENTATION__________________________
from icecube import wimpsim_reader

#___________________ACTION__________________________
@icetray.traysegment
def Process(tray, name,
            Params = RunParameters()):

  tray.AddModule(wimpsim_reader.VeffCollector, "VeffCollector")

#___________________PARSER__________________________
def parseOptions(parser, params):
  parser.add_option("-i", "--input", action="store", type="string", default=None, dest="INPUT", help="Input i3 file to process")
  (options,args) = parser.parse_args()
  params.Infile = options.INPUT

#___________________IF STANDALONE__________________________
if (__name__=='__main__'):
  from optparse import OptionParser

  params = RunParameters()
  usage = 'usage: %prog [options]'
  parser = OptionParser(usage)
  parseOptions(parser, params)

  Infile_List = glob.glob(params.Infile)
  Infile_List.sort()

  from icecube import icetray, dataio
  from I3Tray import *
  tray = I3Tray()

  tray.AddModule("I3Reader", "reader",
    filenamelist= Infile_List)

  tray.AddSegment(Process, "WnC",
          Params = params)

#  tray.AddModule("I3Writer","writer",
#    streams = [icetray.I3Frame.DAQ, icetray.I3Frame.Physics],
#    filename = params.Outfile,)

  tray.AddModule("TrashCan","trashcan")
  tray.Execute()
  tray.Finish()
