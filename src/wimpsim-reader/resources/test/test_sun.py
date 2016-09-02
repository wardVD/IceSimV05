#!/usr/bin/env python
# -*- coding: utf-8 -*-
###
# A script to analyse the vertex position and energy distribution of wimpsim-generated files
###

import os, sys, glob
import random, numpy
from optparse import OptionParser

from I3Tray import *
from icecube import icetray, dataclasses, simclasses, dataio
from icecube import wimpsim_reader
from icecube import phys_services

import unittest

n_Wframes=0
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
class TestQFrameContent(unittest.TestCase):
  """ Test teh content of the Q-frame """
  def __init__(self, methodName='runTest'):
    super(TestQFrameContent,self).__init__(methodName)
    #all keys i must access with std-names
    self.WimpParamsName = "WIMP_params"
    self.WimpMCTreeName = "I3MCTree"
    self.EventHeaderName = "I3EventHeader"
    self.count_up=1
   
  def testKeys(self):
    self.assert_(self.WimpParamsName in self.frame, "The params are written to the frame.")
    self.assert_(self.WimpMCTreeName in self.frame, "The mctree is written to the frame.")
    self.assert_(self.EventHeaderName in self.frame, "The EventHeader is written to the frame.")
   
  def testTypes(self):
    #try to access all keys of the params
    WimpParams = self.frame[self.WimpParamsName]
    self.assert_(isinstance(WimpParams, simclasses.I3WimpParams) and WimpParams!=simclasses.I3WimpParams(), "WimpParams exists and not the default constructor")
    self.assert_(isinstance(WimpParams.mass, float) and WimpParams.mass!=0., "WimpParams.mass exists and is set")
    self.assert_(isinstance(WimpParams.channel, simclasses.I3WimpParams.DecayChannel) and WimpParams.channel!=simclasses.I3WimpParams.DecayChannel.unknown , "WimpParams.channel exists and is set")
    self.assert_(isinstance(WimpParams.source, simclasses.I3WimpParams.SourceType)and WimpParams.source!=simclasses.I3WimpParams.SourceType.UNKNOWN, "WimpParams.time exists and is set")
    self.assert_(isinstance(WimpParams.nu_weight, float) and WimpParams.nu_weight!=0., "WimpParams.nu_weight exists and is set")
    self.assert_(isinstance(WimpParams.lep_weight, float) and WimpParams.lep_weight!=0. , "WimpParams.lep_weight exists and is set")
    self.assert_(isinstance(WimpParams.had_weight, float)  and WimpParams.had_weight!=0., "WimpParams.had_weight exists and is set")
    self.assert_(isinstance(WimpParams.vgen, float) and WimpParams.vgen!=0., "WimpParams.vgen exists and is set")
    self.assert_(isinstance(WimpParams.time, dataclasses.I3Time) and WimpParams.time!=dataclasses.I3Time(), "WimpParams.time exists and is set")
    #try to access all the particles in the mctree
    WimpMCTree = self.frame[self.WimpMCTreeName]
    self.assert_(isinstance(WimpMCTree, dataclasses.I3MCTree) and WimpMCTree!=dataclasses.I3MCTree(), "WimpMCTree exists and is not the default constructor")
    #try to access all set parameters in the I3EventHeader
    EventHeader = self.frame[self.EventHeaderName]
    self.assert_(isinstance(EventHeader, dataclasses.I3EventHeader) and EventHeader!=dataclasses.I3EventHeader(), "EventHeader exists and is not the default constructor")
    #try to access all set parameters in the I3EventHeader
    #test values
    self.assert_(WimpParams.mass == 1000, "The wimpMss is correctly 1000")
    self.assert_(WimpParams.channel == 5, "WIMPs have corectly annihilated trough the 5(w) channel")  #simclasses.I3WimpParams.w
    self.assert_(WimpParams.source == simclasses.I3WimpParams.SUN, "it is the SUN")
    #test the values: MCTree
    self.assert_(len(WimpMCTree.primaries)==1, "found exactly one primary")
    self.assert_(len(WimpMCTree.get_daughters(WimpMCTree.primaries[0]))==2, "found my two daughter particles and they are asserted")
    #test the values EentHeader:
    self.assert_(EventHeader.start_time != dataclasses.I3Time(), "EventHeader.start_time is set")
    self.assert_(EventHeader.run_id == 0, "EventHeader.run_id is correctly set")
    self.assert_(EventHeader.event_id == self.count_up, "EventHeader.event_id is correctly set")
    self.count_up+=1

#__________________________________________________
class TestIFrameContent(unittest.TestCase):
  """ Test the content of the Info-frame """
  def __init__(self, methodName='runTest'):
    super(TestIFrameContent,self).__init__(methodName)
    #all keys i must access with std-names
    self.NeutrinoOscParamsName = "Neutrino_Osc_Params"
    self.WimpSimParamsName = "WimpSim_Params"
    self.WimpSimReaderParamsName = "WimpSimReader_Params"
    self.FileName = "File"
    self.SourceName = "Source"

  def testKeys(self):
    if self.frame.Has("n_Wframes"):
      if self.frame["n_Wframes"].value==0:
        self.assert_(self.NeutrinoOscParamsName in self.frame, "The NeutrinoOscParams are written to the frame.")
        self.assert_(self.WimpSimParamsName in self.frame, "The WimpSimParams are written to the frame.")
        self.assert_(self.WimpSimReaderParamsName in self.frame, "The WimpSimReaderParams are written to the frame.")
        self.assert_(self.FileName in self.frame, "The Name of the read fiel is written to the frame.")
        self.assert_(self.SourceName in self.frame, "The String of the WIpSource is written to the frame.")
  
  def testTypes(self):
    #try to access all keys of the params
    if self.frame.Has("n_Wframes"):
      if self.frame["n_Wframes"].value==0:
        NeutrinoOscParams = self.frame[self.NeutrinoOscParamsName]
        self.assert_(isinstance(NeutrinoOscParams, dataclasses.I3MapStringDouble) and NeutrinoOscParams!=dataclasses.I3MapStringDouble(), "NeutrinoOscParamsexists and not the default constructor")
        self.assert_(set(["theta_12","theta_13","theta_23","delta","delta_m^2_21","delta_m^2_31"]).issubset(list(NeutrinoOscParams.keys())), "All keys in NeutrinoOscParams are accessable")

        WimpSimParams = self.frame[self.WimpSimParamsName]
        self.assert_(isinstance(WimpSimParams, dataclasses.I3MapStringDouble) and WimpSimParams!=dataclasses.I3MapStringDouble(), "WimpSimParams exists and not the default constructor")
        self.assert_(set(["WIMP_mass","Annihilation_Channel","Number_of_Annihilations","Source","Detector_Longitude","Detector_Latitude","mjd1","mjd2"]).issubset(list(WimpSimParams.keys())), "All keys in WimpSimParams are accessable")
        
        WimpSimReaderParams = self.frame[self.WimpSimReaderParamsName]
        self.assert_(isinstance(WimpSimReaderParams, dataclasses.I3MapStringDouble) and WimpSimReaderParams!=dataclasses.I3MapStringDouble(), "WimpSimReaderParams exists and not the default constructor")
        self.assert_(set(["NEvents","PositionLimitsXmin","PositionLimitsXmax","PositionLimitsYmin","PositionLimitsYmax","PositionLimitsZmin","PositionLimitsZmax","InjectionRadius","LowZenCut","UpperZenCut","UseElectrons","UseMuons","UseTaus","UseNC","Oversampling","SensitiveHeight","SensitiveRadius"]).issubset(list(WimpSimReaderParams.keys())), "All keys in WimpSimReaderParams are accessable")

        File= self.frame[self.FileName]
        self.assert_(isinstance(File, dataclasses.I3String) and File!=dataclasses.I3String(), "File exists and is the not the default constructor")
        
        Source= self.frame[self.SourceName]
        self.assert_(isinstance(Source, dataclasses.I3String) and Source!=dataclasses.I3String(), "Source exists and is not the default constructor")

  def testValues(self):
    if self.frame.Has("n_Wframes"):
      if self.frame["n_Wframes"].value==0:
        NeutrinoOscParams = self.frame[self.NeutrinoOscParamsName]
        self.assert_(NeutrinoOscParams["theta_12"]==0.33890000E+02,"")
        self.assert_(NeutrinoOscParams["theta_13"]==0.91200000E+01,"")
        self.assert_(NeutrinoOscParams["theta_23"]==0.38540000E+02 ,"")
        self.assert_(NeutrinoOscParams["delta"]==0.00000000E+00,"")
        self.assert_(NeutrinoOscParams["delta_m^2_21"]==0.75000000E-04,"")
        self.assert_(NeutrinoOscParams["delta_m^2_31"]==0.23200000E-02,"")

        WimpSimParams = self.frame[self.WimpSimParamsName]
        self.assert_(WimpSimParams["WIMP_mass"] == 0.10000000E+04, "")
        self.assert_(WimpSimParams["Annihilation_Channel"] == 5, "")
        self.assert_(WimpSimParams["Number_of_Annihilations"] == 1000000, "")
        self.assert_(WimpSimParams["Source"] == 1, "")
        self.assert_(WimpSimParams["Detector_Longitude"] == 0.00000000E+00, "")
        self.assert_(WimpSimParams["Detector_Latitude"] == -.90000000E+02, "")
        self.assert_(WimpSimParams["mjd1"] == 55694.00000000000, "")
        self.assert_(WimpSimParams["mjd2"] == 56063.00000000000, "")
        
        WimpSimReaderParams = self.frame[self.WimpSimReaderParamsName]
        self.assert_(WimpSimReaderParams["NEvents"] == 0,"")
        self.assert_(WimpSimReaderParams["StartMJD"] == 55694,"")
        self.assert_(WimpSimReaderParams["EndMJD"] == (55694+365.25),"")
        self.assert_(WimpSimReaderParams["Oversampling"] == 0,"")
        self.assert_(WimpSimReaderParams["PositionLimitsXmin"] == -800,"")
        self.assert_(WimpSimReaderParams["PositionLimitsXmax"] == 800,"")
        self.assert_(WimpSimReaderParams["PositionLimitsYmin"] == -800,"")
        self.assert_(WimpSimReaderParams["PositionLimitsYmax"] == 800,"")
        self.assert_(WimpSimReaderParams["PositionLimitsZmin"] == -800,"")
        self.assert_(WimpSimReaderParams["PositionLimitsZmax"] == 800,"")
        self.assert_(WimpSimReaderParams["InjectionRadius"] == 700*I3Units.meter,"")
        self.assert_(WimpSimReaderParams["LowZenCut"] == 0*I3Units.degree,"")
        self.assert_(WimpSimReaderParams["UpperZenCut"] == 180*I3Units.degree,"")
        self.assert_(WimpSimReaderParams["SensitiveHeight"] == 1300*I3Units.meter,"")
        self.assert_(WimpSimReaderParams["SensitiveRadius"] == 700*I3Units.meter,"")
        self.assert_(WimpSimReaderParams["UseElectrons"] == True,"")
        self.assert_(WimpSimReaderParams["UseMuons"] == True,"")
        self.assert_(WimpSimReaderParams["UseTaus"] == True,"")
        self.assert_(WimpSimReaderParams["UseNC"] == True,"")
        
        File= self.frame[self.FileName]
        #self.assert_(File.value == options.Infile, "") #TODO can not check this without serious hacking
        
        Source= self.frame[self.SourceName]
        self.assert_(Source.value == "Sun", "")

#__________________________________________________
class TestWFrameContent(unittest.TestCase):
  """ Test the content of the W-frame """
  def __init__(self, methodName='runTest'):
    super(TestWFrameContent,self).__init__(methodName)
    #all keys i must access with std-names
    self.DeliveredWeightName= "DeliveredWeightMap"
    self.TotalWeightName = "TotalWeightMap"
  
  def testKeys(self):
    if self.frame.Has("n_Wframes"):
      if self.frame["n_Wframes"].value==1:
        self.assert_(self.DeliveredWeightName in self.frame, "The DeliveredWeight Map is written to the frame.")
        self.assert_(self.TotalWeightName in self.frame, "The TotalWeight Map is written to the frame.")
  
  def testTypes(self):
    #try to access all keys of the params
    if self.frame.Has("n_Wframes"):
      if self.frame["n_Wframes"].value==1:
        DeliveredWeight = self.frame[self.DeliveredWeightName]
        self.assert_(isinstance(DeliveredWeight, dataclasses.I3MapStringDouble) and DeliveredWeight!=dataclasses.I3MapStringDouble(), "NeutrinoOscParams and not the default constructor")
        self.assert_(set(["nu_weight","lowe_weight","lep_weight","e_weight","mu_weight","tau_weight","nc_weight","had_weight"]).issubset(list(DeliveredWeight.keys())), "All keys in DeliveredWeigh are accessable")

        TotalWeight = self.frame[self.TotalWeightName]
        self.assert_(isinstance(TotalWeight, dataclasses.I3MapStringDouble) and TotalWeight!=dataclasses.I3MapStringDouble(), "NeutrinoOscParams and not the default constructor")
        self.assert_(set(["nu_weight","lowe_weight","lep_weight","e_weight","mu_weight","tau_weight","nc_weight","had_weight"]).issubset(list(TotalWeight.keys())), "All keys in DeliveredWeigh are accessable")
    
  def testValues(self):
    if self.frame.Has("n_Wframes"):
      if self.frame["n_Wframes"].value==1:
        DeliveredWeight = self.frame[self.DeliveredWeightName]
        self.assert_(DeliveredWeight["nu_weight"]!=0,"")
        self.assert_(DeliveredWeight["lowe_weight"]==0,"") #this is hardcoded
        self.assert_(DeliveredWeight["lep_weight"]!=0,"")
        self.assert_(DeliveredWeight["e_weight"]!=0,"")
        self.assert_(DeliveredWeight["mu_weight"]!=0,"")
        self.assert_(DeliveredWeight["tau_weight"]!=0,"")
        self.assert_(DeliveredWeight["nc_weight"]!=0,"")
        self.assert_(DeliveredWeight["had_weight"]!=0,"")

        TotalWeight = self.frame[self.TotalWeightName]
        self.assert_(TotalWeight["nu_weight"]!=0,"")
        self.assert_(TotalWeight["lowe_weight"]!=0,"")
        self.assert_(TotalWeight["lep_weight"]!=0,"")
        self.assert_(TotalWeight["e_weight"]!=0,"")
        self.assert_(TotalWeight["mu_weight"]!=0,"")
        self.assert_(TotalWeight["tau_weight"]!=0,"")
        self.assert_(TotalWeight["nc_weight"]!=0,"")
        self.assert_(TotalWeight["had_weight"]!=0,"")
        #...(from above)
 
@icetray.traysegment
def DoUnitTestSun(tray, name, options):
  #---------------- tray ---------------
  # run the module an tell me about what you find
  from icecube import icetray, dataio, dataclasses

  tray.AddService("I3SPRNGRandomServiceFactory","Random",
    NStreams = 2,
    Seed = options.Seed,
    StreamNum = 1,
    InstallServiceAs = "Random")

  tray.AddModule("I3WimpSimReader","wimpsim-reader",
    RandomServiceName = "Random",
    FileNameList = [options.Infile], #default [""]
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
  

  def Wframecounter(frame):
    global n_Wframes
    frame.Put("n_Wframes", icetray.I3Int(n_Wframes))
    n_Wframes +=1
  
  tray.AddModule(Wframecounter, "Wframecounter",Streams=[icetray.I3Frame.Stream('W')])
  
  if (options.RunTest): 
    tray.AddModule(icetray.I3TestModuleFactory(TestIFrameContent), "TestIFrameContent", Streams=[icetray.I3Frame.Stream('W')],
      If = lambda frame: frame["n_Wframes"].value==0)  #this will not work, have to Hack

    tray.AddModule(icetray.I3TestModuleFactory(TestQFrameContent), "TestQFrameContent", Streams=[icetray.I3Frame.DAQ]) 
    
    tray.AddModule(icetray.I3TestModuleFactory(TestWFrameContent), "TestWFrameContent", Streams=[icetray.I3Frame.Stream('W')],
      If = lambda frame: frame["n_Wframes"].value==1) #this will not work, have to Hack

    tray.AddModule("Delete", "delete", keys=["n_Wframes"])
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
  
  tray.AddSegment(DoUnitTestSun, "DoUnitTestSun", options = params)
  
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
