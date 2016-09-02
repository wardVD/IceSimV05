#!/usr/bin/env python

#A script that cleans up after the WimpSim-Reader:
# Combine the Objects in a trailing W-frame to the front one with the settings
# Edit the I3EventHeader with the datasetinfo

import os, sys, getopt, glob
from icecube import icetray, dataio, dataclasses, phys_services
from optparse import OptionParser

from I3Tray import *

class RunParameters:
  def __init__(self):
    self.Infile = '' 
    self.Outfile = ''
    self.RunNumber = 0
    self.FileNumber = 0

def Sim_CLSim_cleanup(options):
  
  ################### check for possibel conflicts ##################
  if options.FileNumber>99999:
    exit("Cannot convert to corsika numbering conventions")
  ############################## peak in back #######################
  print("moving through file")

  delwmap = dataclasses.I3MapStringDouble()
  totwmap = dataclasses.I3MapStringDouble()

  infile = dataio.I3File(options.Infile)

  while infile.more():
    frame = infile.pop_frame()
    if frame.Has('DeliveredWeightMap') and frame.Has('TotalWeightMap'):
      delwmap=frame['DeliveredWeightMap']
      totwmap=frame['TotalWeightMap']
      break

  infile.close()

  ############################## tray 0 ##############################

  print("execute trays")

  load("libicetray")
  load("libinterfaces")
  load("libdataclasses")

  tray = I3Tray()

  tray.AddModule("I3Reader","reader",
    filenamelist = [options.INPUT],)

  writtenit=False

  def killthelastinfoframe(frame):
    global writtenit  
    if writtenit==True:
      if (frame.Has('DeliveredWeightMap') and frame.Has('TotalWeightMap')):
	return False

  tray.AddModule(killthelastinfoframe,"killthelastinfoframe",
    Streams=[])

  def pushwmapfront(frame):
    global writtenit
    if frame.Has('WimpSimReader_Params'):
      frame['DeliveredWeightMap']=delwmap
      frame['TotalWeightMap']=totwmap
      writtenit=True

  tray.AddModule(pushwmapfront,"pushwmapfront",
    Streams=[])

  def eventheadercrawler(frame):
    if frame.Has("I3EventHeader"):
      ev = frame['I3EventHeader']
      frame.Delete("I3EventHeader")
      run_id=(options.RunNumber*100000+options.FileNumber) #this is corsika conventions 4 digits dataset + 5 digits filenumber
      ev.run_id=run_id
      frame.Put("I3EventHeader",ev)

  tray.AddModule(eventheadercrawler,"crawler",
    Streams=[icetray.I3Frame.DAQ])
    
  tray.AddModule("I3Writer","writer",
    streams = [],
    filename = options.Outfile,)

  tray.AddModule("TrashCan","trashcan")
    
  tray.Execute()
    
  tray.Finish()

#___________________PARSER__________________________
def parseOptions(parser, params):
  parser.add_option("-i", "--input", action="store", type="string", default="", dest="INPUT", help="Input i3 file to process")
  parser.add_option("-o", "--output", action="store", type="string", default="", dest="OUTPUT", help="Output i3 file")
  parser.add_option("-r", "--run", action="store", type="int", default=0, dest="RUN", help="RUN or dataset-number")
  parser.add_option("-f", "--file", action="store", type="int", default=0, dest="FILE", help="file-number")
  (options,args) = parser.parse_args()
  params.Infile = options.INPUT 
  params.Outfile = options.OUTPUT
  params.RunNumber = options.RUN
  params.FileNumber = options.FILE

#___________________IF STANDALONE__________________________
if (__name__=='__main__'):
  from optparse import OptionParser

  params = RunParameters()

  usage = 'usage: %prog [options]'
  parser = OptionParser(usage)

  parseOptions(parser, params)
  
  from icecube import icetray
  icetray.load("libdataio", False)
    
  Sim_CLSim_cleanup(params)
  
