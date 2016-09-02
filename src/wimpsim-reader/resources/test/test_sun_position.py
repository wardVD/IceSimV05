#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
A script to analyse the vertex position and energy distribution of wimpsim-generated files
"""

import os, sys, glob
import random, numpy
from optparse import OptionParser

from I3Tray import *
from icecube import icetray, dataclasses, simclasses, dataio
from icecube import wimpsim_reader
from icecube import phys_services

import unittest

class RunParameters:
  def __init__(self):
    self.Infile = '' 
    self.Outfile = ''
    self.GCDfile = ''
    self.Seed = 0
    self.NEvents = 0
    self.I3File = False
    self.RunTest = False

from icecube import astro

#__________________________________________________
class SunPosition(unittest.TestCase):
  """test this sequence"""
  def __init__(self, methodName='runTest'):
    super(SunPosition,self).__init__(methodName)
  def testSequence(self):
    sun_from_timestamp  = astro.sun_dir(self.frame['I3EventHeader'].start_time.mod_julian_day_double)
    sun_from_wimpparams = astro.sun_dir(self.frame['WIMP_params'].time.mod_julian_day_double)
    nu                  = dataclasses.get_most_energetic_neutrino(self.frame['I3MCTree'])
    sun_from_nu         = nu.dir.zenith, nu.dir.azimuth
    icetray.logging.log_info("Sunpos from TimeStamp  :"+str(sun_from_timestamp))
    icetray.logging.log_info("Sunpos from WimpParams :"+str(sun_from_wimpparams))
    icetray.logging.log_info("Sunpos from neutrino   :"+str(sun_from_nu))
    
    self.assert_(sun_from_wimpparams[0] <= sun_from_nu[0]+0.01*I3Units.deg and sun_from_wimpparams[0] >= sun_from_nu[0]-0.01*I3Units.deg, "zenith match within precision 0.02deg")
    self.assert_(sun_from_wimpparams[1] <= sun_from_nu[1]+0.01*I3Units.deg and sun_from_wimpparams[1] >= sun_from_nu[1]-0.01*I3Units.deg, "azimuth match within precision 0.02deg")
    

#___________________PARSER__________________________
def parseOptions(parser, params):
  parser = OptionParser()
  parser.add_option("-g", "--GCDfile", action="store", default=os.path.join(os.getenv('I3_TESTDATA'), 'sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz'), dest="GCD", help="Name of the GCD file")
  parser.add_option("-i", "--infile", action="store", default=os.path.join(os.getenv('I3_SRC'), 'wimpsim-reader/resources/test_data_sun.dat'), dest="INPUT", help="Name of the input (.dat) file")
  parser.add_option("-o", "--outfile", action="store", default=os.path.join(os.getcwd(), 'test_data_sun.i3'), dest="OUTPUT", help="Name of the output (.root) file")
  parser.add_option("-n", "--number", action="store", type="int", default="0", dest="NEVENTS", help="Number of events to process")
  parser.add_option("-s", "--seed", action="store", type="int", default="0", dest="SEED", help="Seed to propagate")
  parser.add_option("--i3file", action="store_true", default=False, dest="I3FILE", help="should write i3-file")
  parser.add_option("--runtest", action="store_true", default=True, dest="RUNTEST", help="should I run the testcase")

  (options,args) = parser.parse_args()
  params.Infile = options.INPUT 
  params.Outfile = options.OUTPUT
  params.GCDfile = options.GCD
  params.Seed = options.SEED
  params.NEvents = options.NEVENTS
  params.I3File = options.I3FILE
  params.RunTest = options.RUNTEST

#___________________IF STANDALONE__________________________
if (__name__=='__main__'):
  from optparse import OptionParser

  params = RunParameters()

  usage = 'usage: %prog [options]'
  parser = OptionParser(usage)

  parseOptions(parser, params)
  
  from icecube import icetray, dataio
  
  tray = I3Tray()

  tray.AddService("I3SPRNGRandomServiceFactory","Random",
    NStreams = 2,
    Seed = params.Seed,
    StreamNum = 1,
    InstallServiceAs = "Random")

  tray.AddModule("I3WimpSimReader","wimpsim-reader",
    RandomServiceName = "Random",
    FileNameList = [params.Infile], #default [""]
    NEvents = 0, #default 0
    StartMJD = 55694, #default NAN
    EndMJD = (55694+365.25), #default NAN
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

  tray.AddModule(icetray.I3TestModuleFactory(SunPosition), "TestIFrameContent",
    Streams=[icetray.I3Frame.DAQ])

  if params.I3File:
    tray.AddModule("I3Writer","writer",
      #streams = [icetray.I3Frame.DAQ,icetray.I3Frame.Physics],
      filename = params.Outfile,)

  tray.AddModule("TrashCan","trashcan")
    
  if (params.NEvents==0):
    tray.Execute()
  else:
    tray.Execute(params.NEvents)
    
  tray.Finish()
