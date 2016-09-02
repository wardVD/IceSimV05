#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
A script to analyse the vertex position and energy distribution of wimpsim-generated files
"""

import os, sys, glob
import random, numpy
from optparse import OptionParser

from I3Tray import *
from icecube import icetray, dataclasses, simclasses, dataio, phys_services
from icecube import wimpsim_reader
#from icecube import phys_services
from icecube.dataclasses import I3Constants

import unittest

n_Qframes=0
class RunParameters:
  def __init__(self):
    self.Infile = ''
    self.Outfile = ''
    self.GCDfile = ''
    self.Seed = 0
    self.NEvents = 0
    self.I3File = False
    self.RunTest = False

#__________________________________________________
class TestEarthEventContent(unittest.TestCase):
  def __init__(self, methodName='runTest'):
    super(TestEarthEventContent,self).__init__(methodName) 
    #all keys i must access with std-names
    self.NeutrinoOscParamsName = "Neutrino_Osc_Params"
    self.WimpSimParamsName = "WimpSim_Params"
    self.WimpSimReaderParamsName = "WimpSimReader_Params"

  def testEvent(self):
    if self.frame.Has("n_Qframes"):
      if self.frame["n_Qframes"].value==0:

        event_header = self.frame["I3EventHeader"]
        mctree = self.frame["I3MCTree"]
        wimp_params = self.frame["WIMP_params"]

        primary = mctree.primaries[0];
        daughters =  mctree.get_daughters(primary)
        lepton = daughters[0]
        hadron = daughters[1]

        self.assert_((lepton.time == hadron.time) and (hadron.time == 0.*I3Units.ns))
        self.assert_((primary.fit_status == lepton.fit_status) and (lepton.fit_status == hadron.fit_status) and (hadron.fit_status == dataclasses.I3Particle.OK))
        self.assert_((primary.location_type == lepton.location_type) and (lepton.location_type == hadron.location_type) and (hadron.location_type == dataclasses.I3Particle.InIce))
        self.assert_((lepton.pos == hadron.pos))

        self.assert_(primary.shape == dataclasses.I3Particle.Primary)
        self.assert_(hadron.shape == dataclasses.I3Particle.Cascade)

        self.assert_(hadron.type == dataclasses.I3Particle.Hadrons)

        self.assert_(wimp_params.source == simclasses.I3WimpParams.SourceType.EARTH)
        self.assert_(wimp_params.mass == 1000)
        self.assert_(wimp_params.channel == simclasses.I3WimpParams.DecayChannel.bottom)

        self.assert_(event_header.start_time == dataclasses.I3Time(56000.))
        self.assert_(event_header.run_id == 0)
        self.assert_(event_header.sub_run_id == 0)
        self.assert_(event_header.event_id == 1)
        self.assert_(event_header.sub_event_id == 0)

        #primary
        #self.assertAlmostEqual(primary.pos.x,-384.0513101255803,3)
        #self.assertAlmostEqual(primary.pos.y,557.37380386608424,3)
        #self.assertAlmostEqual(primary.pos.z,-620.76382096685438,3)
        self.assertAlmostEqual(primary.energy,1000.,0)
        self.assert_(primary.shape == (dataclasses.I3Particle.Primary))
        self.assert_(primary.type == (dataclasses.I3Particle.NuEBar))
        self.assertAlmostEqual(primary.dir.zenith,I3Constants.pi,5)
        self.assertAlmostEqual(primary.dir.azimuth,0.5*I3Constants.pi,5)

        #lepton
        #self.assertAlmostEqual(lepton.pos.x,-384.0513101255803,3)
        #self.assertAlmostEqual(lepton.pos.y,557.37380386608424,3)
        #self.assertAlmostEqual(lepton.pos.z,-620.76382096685438,3)
        self.assertAlmostEqual(lepton.energy,750.,0)
        self.assert_(lepton.shape == (dataclasses.I3Particle.Cascade))
        self.assert_(lepton.type == (dataclasses.I3Particle.EPlus))
        self.assertAlmostEqual(lepton.dir.zenith,I3Constants.pi,5)
        self.assertAlmostEqual(lepton.dir.azimuth,0.25*I3Constants.pi,5)

        #hadron
        #self.assertAlmostEqual(hadron.pos.x,-384.0513101255803,3)
        #self.assertAlmostEqual(hadron.pos.y,557.37380386608424,3)
        #self.assertAlmostEqual(hadron.pos.z,-620.76382096685438,3)
        self.assertAlmostEqual(hadron.energy,250.,0)
        self.assert_(hadron.shape == (dataclasses.I3Particle.Cascade))
        self.assert_(hadron.type == (dataclasses.I3Particle.Hadrons))
        self.assertAlmostEqual(hadron.dir.zenith,I3Constants.pi,5)
        self.assertAlmostEqual(hadron.dir.azimuth,0.75*I3Constants.pi,5)

        #wimp_params
        self.assert_(wimp_params.nu_weight == 0.100000E-24)
        self.assert_(wimp_params.lep_weight == 0.100000E-37)
        self.assert_(wimp_params.had_weight == 0.100000E-37)
        self.assertAlmostEqual(wimp_params.vgen,2463008640.4143977,1 )
        self.assert_(wimp_params.time == dataclasses.I3Time(0,0))

#__________________________________________________
@icetray.traysegment
def DoUnitTestEarthEvent(tray, name, options):
  #---------------- tray ---------------
  # run the module an tell me about what you find

  tray.AddService("I3SPRNGRandomServiceFactory","Random",
    NStreams = 2,
    Seed = options.Seed,
    StreamNum = 1,
    InstallServiceAs = "Random")

  tray.AddModule("I3WimpSimReader","wimpsim-reader",
    RandomServiceName = "Random",
    FileNameList = [options.Infile], #default [""]
    NEvents = 0, #default 0
    StartMJD = 56000., #default NAN
    EndMJD = (56000.), #default NAN
    Oversampling = 0, #default 0
    PositionLimits = [-800,800,-800,800,-800,800], #default [-800,800,-800,800,-800,800]
    InjectionRadius = 700*I3Units.meter, #default 0
    LowerZenCut = 0*I3Units.degree, #default 0*I3Units.degree,
    UpperZenCut = 180*I3Units.degree, #default 180*I3Units.degree
    SensitiveHeight = 1300*I3Units.meter, #default 0*I3Units.meter
    SensitiveRadius = 700*I3Units.meter, #default 0*I3Units.meter
    UseElectrons = True, #default False
    UseMuons = True, #default True
    UseTaus = True, #default False
    UseNC = True, #default False
  )

  def Qframecounter(frame):
    global n_Qframes
    frame.Put("n_Qframes", icetray.I3Int(n_Qframes))
    n_Qframes +=1

  tray.AddModule(Qframecounter, "Qframecounter",Streams=[icetray.I3Frame.Stream('Q')])

  if (options.RunTest):
    tray.AddModule(icetray.I3TestModuleFactory(TestEarthEventContent), "TestEarthEventContent", Streams=[icetray.I3Frame.DAQ])

  tray.AddModule("Delete", "delete", keys=["n_Qframes"])

#___________________PARSER__________________________
def parseOptions(parser, params):
  parser = OptionParser()
  parser.add_option("-g", "--GCDfile", action="store", default=os.path.join(os.getenv('I3_TESTDATA'), 'sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz'), dest="GCD", help="Name of the GCD file")
  parser.add_option("-i", "--infile", action="store", default=os.path.join(os.getenv('I3_SRC'), 'wimpsim-reader/resources/test_fake_earth.dat'), dest="INPUT", help="Name of the input (.dat) file")
  parser.add_option("-o", "--outfile", action="store", default=os.path.join(os.getcwd(), 'test_data_earth.i3'), dest="OUTPUT", help="Name of the output (.root) file")
  parser.add_option("-n", "--number", action="store", type="int", default="0", dest="NEVENTS", help="Number of events to process")
  parser.add_option("-s", "--seed", action="store", type="int", default="0", dest="SEED", help="Seed to propagate")
  parser.add_option("--i3file", action="store_true", default=False, dest="I3FILE", help="should write i3-file")
  parser.add_option("--runtest", action="store_true", default=True, dest="RUNTEST", help="should I run the testcase") #DANGER


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

  tray.AddSegment(DoUnitTestEarthEvent, "DoUnitTestEarthEvent", options = params)

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
