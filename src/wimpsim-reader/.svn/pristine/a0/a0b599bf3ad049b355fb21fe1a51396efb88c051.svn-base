#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
This is really not a unit test but a regression test, ensuring that this project's algorithm continues
to produce exactly the same output as the original wimpsim-reader. If any change is made to the
algorithm this test will fail, and if the change is intentional the test data will need to be regenerated
to put this test back on track.
This script can also be used to generate the testcase, if you provide the path to an i3 file in the call
"""

import os, sys, glob
import random, numpy
from optparse import OptionParser

from I3Tray import *
from icecube import icetray, dataclasses, simclasses, dataio, phys_services
from icecube import wimpsim_reader
from icecube.dataclasses import I3Constants

import unittest
  
#__________________________________________________
class TestEventContent(unittest.TestCase):
  """ define what the test-case should actually see """
  def __init__(self, methodName='runTest'):
    super(TestEventContent,self).__init__(methodName) 
    #all keys i must access with std-names
    self.NeutrinoOscParamsName = "Neutrino_Osc_Params"
    self.WimpSimParamsName = "WimpSim_Params"
    self.WimpSimReaderParamsName = "WimpSimReader_Params"
      
  def testEvent(self):
    if (self.frame.Stop == icetray.I3Frame.DAQ):
      if (self.i3file_reference.more()):
        ref_frame = self.i3file_reference.pop_daq()
      else:
        self.assert_(self.i3file_reference.more(), "There here enough frames to compare to")
        #NOTE here could be a vilidity check for ref_frame
      
        #give some convenience keys
        event_header = self.frame["I3EventHeader"]
        mctree = self.frame["I3MCTree"]
        wimp_params = self.frame["WIMP_params"]

        primary = mctree.primaries[0];
        daughters =  mctree.get_daughters(primary)
        lepton = daughters[0]
        hadron = daughters[1]
        
        #also for the reference
        ref_event_header = ref_frame["I3EventHeader"]
        ref_mctree = ref_frame["I3MCTree"]
        ref_wimp_params = ref_frame["WIMP_params"]

        ref_primary = ref_mctree.primaries[0];
        ref_daughters =  ref_mctree.get_daughters(primary)
        ref_lepton = ref_daughters[0]
        ref_hadron = ref_daughters[1]

        #Start comparisions
        self.assert_(primary.time == ref_primary.time)
        self.assert_(lepton.time == ref_lepton.time)
        self.assert_(hadron.time == ref_hadron.time)
        
        self.assert_(primary.fit_status == ref_primary.fit_status)
        self.assert_(lepton.fit_status == ref_lepton.fit_status)
        self.assert_(hadron.fit_status == ref_hadron.fit_status)

        self.assert_(primary.location_type == ref_primary.location_type)
        self.assert_(lepton.location_type == ref_lepton.location_type)
        self.assert_(hadron.location_type == ref_hadron.location_type)

        self.assert_(primary.shape == ref_primary.shape)
        self.assert_(lepton.shape == ref_lepton.shape)
        self.assert_(hadron.shape == ref_hadron.shape)

        self.assert_(hadron.type == ref_hadron.type)

        self.assert_(wimp_params.source == ref_wimp_params.source)
        self.assert_(wimp_params.mass == ref_wimp_params.mass)
        self.assert_(wimp_params.channel == ref_wimp_params.channel)

        self.assert_(event_header.start_time == ref_event_header.start_time)

        self.assert_(event_header.start_time == ref_event_header.start_time)
        self.assert_(event_header.run_id == ref_event_header.run_id)
        self.assert_(event_header.sub_run_id == ref_event_header.sub_run_id)
        self.assert_(event_header.event_id == ref_event_header.event_id)
        self.assert_(event_header.sub_event_id == ref_event_header.sub_event_id)

        #primary
        self.assert_(primary.pos.x == ref_primary.pos.x)
        self.assert_(primary.pos.y == ref_primary.pos.y)
        self.assert_(primary.pos.z == ref_primary.pos.z)
        self.assert_(primary.energy == ref_primary.energy)
        self.assert_(primary.shape == ref_primary.shape)
        self.assert_(primary.type == ref_primary.type)
        self.assert_(primary.dir.zenith == ref_primary.dir.zenith)
        self.assert_(primary.dir.azimuth == ref_primary.dir.azimuth)

        #lepton
        self.assert_(lepton.pos.x == ref_lepton.pos.x)
        self.assert_(lepton.pos.y == ref_lepton.pos.y)
        self.assert_(lepton.pos.z == ref_lepton.pos.z)
        self.assert_(lepton.energy == ref_lepton.energy)
        self.assert_(lepton.shape == ref_lepton.shape)
        self.assert_(lepton.type == ref_lepton.type)
        self.assert_(lepton.dir.zenith == ref_lepton.dir.zenith)
        self.assert_(lepton.dir.azimuth == ref_lepton.dir.azimuth)

        #hadron
        self.assert_(hadron.pos.x == ref_hadron.pos.x)
        self.assert_(hadron.pos.y == ref_hadron.pos.y)
        self.assert_(hadron.pos.z == ref_hadron.pos.z)
        self.assert_(hadron.energy == ref_hadron.energy)
        self.assert_(hadron.shape == ref_hadron.shape)
        self.assert_(hadron.type == ref_hadron.type)
        self.assert_(hadron.dir.zenith == ref_hadron.dir.zenith)
        self.assert_(hadron.dir.azimuth == ref_hadron.dir.azimuth)

        #wimp_params
        self.assert_(wimp_params.nu_weight == ref_wimp_params.nu_weight)
        self.assert_(wimp_params.lep_weight == ref_wimp_params.lep_weight)
        self.assert_(wimp_params.had_weight == ref_wimp_params.had_weight)
        self.assert_(wimp_params.vgen == ref_wimp_params.vgen)


class TestSunEventContent(TestEventContent):
  def __init__(self, methodName='runTest'):
    TestEventContent.__init__(self, methodName)
    self.i3file_reference = dataio.I3File(os.path.expandvars("$I3_TESTDATA") +'/wimpsim-reader/wimp_sun_testcase.i3.bz2')

class TestEarthEventContent(TestEventContent):
  def __init__(self, methodName='runTest'):
    TestEventContent.__init__(self, methodName)
    self.i3file_reference = dataio.I3File(os.path.expandvars("$I3_TESTDATA") + '/wimpsim-reader/wimp_earth_testcase.i3.bz2')


#============== TRAY for sun ====================
from icecube import icetray, dataio

#if you run this script with the second argument a number, that many test-frame-packets will be delivered to you
test = (not (len(sys.argv) ==2 ))

tray = I3Tray()

Infile= os.path.join(os.getenv('I3_SRC'), 'wimpsim-reader/resources/test_data_sun.dat')

tray.AddService("I3SPRNGRandomServiceFactory","Random",
  NStreams = 2,
  Seed = 0,
  StreamNum = 1,
  InstallServiceAs = "Random")

tray.AddModule("I3WimpSimReader","wimpsim-reader",
  RandomServiceName = "Random",
  FileNameList = [Infile], #default [""]
  NEvents = 0, #default 0
  StartMJD = float('nan'), #default NAN
  EndMJD = float('nan'), #default NAN
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

if (test):
  tray.AddModule(icetray.I3TestModuleFactory(TestSunEventContent), "TestSunEventContent", Streams=[icetray.I3Frame.DAQ])
    
else:
  tray.AddModule("I3Writer", "Writer",
                  Filename= os.path.join(os.path.expandvars("$I3_BUILD"),"wimpsim-reader/resources/wimp_sun_testcase.i3.bz2"),
                  Streams = [#icetray.I3Frame.Geometry,
                            #icetray.I3Frame.Calibration,
                            #icetray.I3Frame.DetectorStatus,
                            icetray.I3Frame.DAQ,
                            #icetray.I3Frame.Physics
                            ])
                  
tray.AddModule("TrashCan","last")

tray.Execute()
tray.Finish()


del tray


#================= TRAY for earth =====================

Infile= os.path.join(os.getenv('I3_SRC'), 'wimpsim-reader/resources/test_data_earth.dat')

tray = I3Tray()

tray.AddService("I3SPRNGRandomServiceFactory","Random",
  NStreams = 2,
  Seed = 0,
  StreamNum = 1,
  InstallServiceAs = "Random")

tray.AddModule("I3WimpSimReader","wimpsim-reader",
  RandomServiceName = "Random",
  FileNameList = [Infile], #default [""]
  NEvents = 0, #default 0
  StartMJD = 55999., #default NAN
  EndMJD = 56001., #default NAN
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

if (test):
  tray.AddModule(icetray.I3TestModuleFactory(TestEarthEventContent), "TestEarthEventContent", Streams=[icetray.I3Frame.DAQ])
    
else:
  tray.AddModule("I3Writer", "Writer",
                  Filename= os.path.join(os.path.expandvars("$I3_BUILD"),"wimpsim-reader/resources/wimp_earth_testcase.i3.bz2"),
                  Streams = [#icetray.I3Frame.Geometry,
                            #icetray.I3Frame.Calibration,
                            #icetray.I3Frame.DetectorStatus,
                            icetray.I3Frame.DAQ,
                            #icetray.I3Frame.Physics
                            ])
                  
tray.AddModule("TrashCan","last")

tray.Execute()
tray.Finish()
