#!/usr/bin/env python

"""
script that collects all the event-weight from wimpsim in a given dataset
functions like this
1. give it a filename with wildcards
2. files will be copied and unpacked
3. wimpsim-reader is run over all the files, collecting the file-weights
4. outout is the colected fileweight as written from wimpsim-reader

In contrast
"""
import os, glob, shutil, math, numpy
from optparse import OptionParser
usage = 'usage: %prog [options]'
parser = OptionParser(usage)
parser.add_option("-i", "--input", action="store", type="string", default="FCR", dest="INPUT", help="Input file to process")
parser.add_option("-o", "--output", action="store", type="string", default="", dest="OUTPUT", help="Output file to write to")
(options,args) = parser.parse_args()

SCRATCHDIR='/scratch/mzoll'

Infile_list = glob.glob(options.INPUT)
Infile_list.sort()

Unpacked_List= []
del_tmp = True

for infile in Infile_list:
  scratchdir = SCRATCHDIR
  #check if this needs unpacking
  if (infile.split('.')[-1]=='dat'):
    print ("Infile already unpacked")
    infile_unpacked=infile
    del_tmp = False
  elif (infile.split('.')[-1]=='gz'):
    shutil.copy(infile, scratchdir)
    print ("Unpacking dat.gz-file")
    infile_packed=os.path.join(scratchdir,os.path.split(infile)[1])
    os.system("gunzip %s" % infile_packed)
    infile_unpacked=os.path.join(scratchdir,os.path.split(infile)[1][:-3])
    del_tmp = del_tmp and True
  elif (infile.split('.')[-1]=='bz2'):
    shutil.copy(infile,scratchdir)
    print ("Unpacking dat.bz2-file")
    infile_packed=os.path.join(scratchdir,infile)
    os.system("bunzip2 %s" % infile_packed)
    infile_unpacked=os.path.join(scratchdir,os.path.split(infile)[1][:-4])
    del_tmp = del_tmp and True
  Unpacked_List.append(infile_unpacked)
  
  
#===tray  
from I3Tray import *
from icecube import icetray, phys_services, dataclasses

tray = I3Tray()

randomX = phys_services.I3SPRNGRandomService(0, 2, 1)

from icecube import wimpsim_reader
tray.AddModule("I3WimpSimReader","wimpsim-reader",
  FileNameList = Unpacked_List, #default ""                                                                                               
  RandomService = randomX,
#  NEvents = 0, #default 0
#  StartMJD = NAN, #default NAN
#  EndMJD = NAN, #default NAN
#  Oversampling = 0, #default 0
  PositionLimits = [-800,800,-800,800,-800,800], #default [0,0,0,0,0,0]
  InjectionRadius = 700*I3Units.meter, #default 0
#  LowZenCut = 0*I3Units.degree, #default 0*I3Units.degree,
#  UpperZenCut = 180*I3Units.degree, #default 180*I3Units.degree
  SensitiveHeight = 1300*I3Units.meter, #default 0*I3Units.meter
  SensitiveRadius = 700*I3Units.meter, #default 0*I3Units.meter
  UseElectrons = False, #default False                                                                                                     
  UseMuons = True, #default True
  UseTaus = False, #default False                                                                                                          
  UseNC = False, #default False                                                                                                            
  InfoFileName = options.OUTPUT+".info")

#tray.AddModule("Dump", "Dump")

tray.AddModule(wimpsim_reader.NuMuValueCollector, "NuMuValueCollector")

class SelectDates(icetray.I3ConditionalModule):
  def __init__(self, ctx):
    icetray.I3ConditionalModule.__init__(self, ctx)
    self.AddParameter("Intervals", "Take events starting from this data", [(-float("inf"),float("inf") )] )
    self.AddOutBox("OutBox")
  def Configure(self):
    icetray.I3ConditionalModule.Configure(self)
    self.intervals = self.GetParameter("Intervals")
  def DAQ(self, frame):
    t = frame["I3EventHeader"].start_time.mod_julian_day_double
    for i in self.intervals:
      if ((i[0] <= t) and (t <= i[1])):
        self.PushFrame(frame)
        return

tray.AddModule(SelectDates, "SelectDates",
               Intervals = [(55640.97260417,55827.37806713),(56006.21820602,56192.61736111111)])

tray.AddModule(wimpsim_reader.NuMuValueCollector, "NuMuValueCollector_TWO")

  
tray.AddModule("TrashCan","trashcan")
tray.Execute()
tray.Finish()

if (del_tmp):
  print "Deleting unpacked files"
  for f in Unpacked_List:
    os.system("rm %s"%f)
