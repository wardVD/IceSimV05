#!/usr/bin/env python

import sys, math, os
from I3Tray import *
from icecube import icetray, dataclasses, dataio, phys_services
from icecube import simclasses

from icecube import DOMLauncher

tray = I3Tray()

i3_testdata = os.path.expandvars("$I3_TESTDATA")                    

tray.AddModule("I3InfiniteSource","FrameMaker",Prefix = i3_testdata + '/sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz')
icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)
import unittest

#This test module is testing the basics of the LC logic. Testing the LC time window for instance.
class SanityCheck(unittest.TestCase):
    def __init__(self,foobar):
        super(SanityCheck,self).__init__(foobar)
        #Setting up the globals for the DOMObject.
        self.random_service = phys_services.I3GSLRandomService(2)

    def testSLC(self):
        geo = self.frame["I3Geometry"].omgeo
        cal = self.frame["I3Calibration"].dom_cal
        stat = self.frame["I3DetectorStatus"].dom_status

        omKey1 = icetray.OMKey(47,2)
        dom1 = DOMLauncher.I3InIceDOM(self.random_service, omKey1)
        dom1.configure(cal[omKey1],stat[omKey1])

        omKey2 = icetray.OMKey(47,3)
        dom2 = DOMLauncher.I3InIceDOM(self.random_service, omKey2)
        dom2.configure(cal[omKey2],stat[omKey2])

        #making the doms neigbors
        dom2.get_neighbors().append(dom1)
        dom1.get_neighbors().append(dom2)

        pulses1 = simclasses.I3MCPulseSeries()
        pulse = simclasses.I3MCPulse()
        pulse.charge = 1
        pulse.time = 0
        pulses1.append(pulse)

        pulses2 = simclasses.I3MCPulseSeries()
        pulse.time = 1025
        pulses2.append(pulse)

        dcstream = DOMLauncher.DCStream()

        dom1.discriminator(pulses1,dcstream)
        dom2.discriminator(pulses2,dcstream)

        dcstream = sorted(dcstream, key = lambda t: t.time)

        for trigg in dcstream:
            #print(trigg.DOM, trigg.time)
            if(trigg.DOM == omKey1):
                dom1.add_trigger(trigg)
            elif(trigg.DOM == omKey2):
                dom2.add_trigger(trigg)

        dom1.trigger_launch(True)
        dom2.trigger_launch(True)

        launches1 = dom1.get_domlaunches()
        launches2 = dom2.get_domlaunches()
        
        self.assert_(len(launches1) == 1, "First frame: DOM 1 produced one launch.")
        self.assert_(len(launches2) == 1, "First frame: DOM 2 produced one launch.")
        
        self.assert_(launches1[0].lc_bit == False and launches2[0].lc_bit == False,
        "First frame: Launches do not have LC.")

        # Simulating that we are going to the next frame and thus reset the DOMObjects.
        dom1.reset(True)
        dom2.reset(True)
        # Now we resimulate the same event and should get the same result back if the reset was
        # correct.
        dcstream = DOMLauncher.DCStream()
        dom1.discriminator(pulses1,dcstream)
        dom2.discriminator(pulses2,dcstream)

        dcstream = sorted(dcstream, key = lambda t: t.time)

        for trigg in dcstream:
            if(trigg.DOM == omKey1):
                dom1.add_trigger(trigg)
            elif(trigg.DOM == omKey2):
                dom2.add_trigger(trigg)

        dom1.trigger_launch(True)
        dom2.trigger_launch(True)

        launches1 = dom1.get_domlaunches()
        launches2 = dom2.get_domlaunches()

        self.assert_(len(launches1) == 1, "Second frame: DOM 1 produced one launch.")
        self.assert_(len(launches2) == 1, "Second frame: DOM 2 produced one launch.")

        self.assert_(launches1[0].lc_bit == False and launches2[0].lc_bit == False,
        "Second frame: Launches do not have LC.")
        
        # Simulating once more. This time we will not force the DOMs to launch at the 
        # end of the frame.
        dom1.reset(True)
        dom2.reset(True)
        dcstream = DOMLauncher.DCStream()
        dom1.discriminator(pulses1,dcstream)
        dom2.discriminator(pulses2,dcstream)

        dcstream = sorted(dcstream, key = lambda t: t.time)

        for trigg in dcstream:
            if(trigg.DOM == omKey1):
                dom1.add_trigger(trigg)
            elif(trigg.DOM == omKey2):
                dom2.add_trigger(trigg)

        #Testing the option to not force launches at the end of the frame.
        #This option is used in Multi Frame Event mode. 
        dom1.trigger_launch(False)
        dom2.trigger_launch(False)

        launches1 = dom1.get_domlaunches()
        launches2 = dom2.get_domlaunches()

        self.assert_(len(launches1) == 1, "Second frame: DOM 1 produced one launch.")
        self.assert_(len(launches2) == 0, "Second frame: DOM 2 did not produce a launch.")

        self.assert_(launches1[0].lc_bit == False , "Third frame: Launch do not have LC.")
        
    def testHLC(self):

         geo = self.frame["I3Geometry"].omgeo
         cal = self.frame["I3Calibration"].dom_cal
         stat = self.frame["I3DetectorStatus"].dom_status

         omKey1 = icetray.OMKey(47,2)
         dom1 = DOMLauncher.I3InIceDOM(self.random_service,omKey1)
         dom1.configure(cal[omKey1],stat[omKey1])

         omKey2 = icetray.OMKey(47,3)
         dom2 = DOMLauncher.I3InIceDOM(self.random_service,omKey2)
         dom2.configure(cal[omKey2],stat[omKey2])

         #making the doms neigbors
         dom2.get_neighbors().append(dom1)
         dom1.get_neighbors().append(dom2)

         pulses1 = simclasses.I3MCPulseSeries()
         pulse = simclasses.I3MCPulse()
         pulse.charge = 1
         pulse.time = 100
         pulses1.append(pulse)
         pulse.time = 6800
         pulses1.append(pulse)
         pulse.time = 7850
         pulses1.append(pulse)
         pulse.time = 8300
         pulses1.append(pulse)
         pulse.time = 8500
         pulses1.append(pulse)

         pulses2 = simclasses.I3MCPulseSeries()
         pulse.time = 500
         pulses2.append(pulse)

         dcstream = DOMLauncher.DCStream()

         dom1.discriminator(pulses1,dcstream)
         dom2.discriminator(pulses2,dcstream)

         dcstream = sorted(dcstream, key = lambda t: t.time)

         for trigg in dcstream:
            if(trigg.DOM == omKey1):
               dom1.add_trigger(trigg)
            elif(trigg.DOM == omKey2):
               dom2.add_trigger(trigg)

         dom1.trigger_launch(True)
         dom2.trigger_launch(True)

         launches1 = dom1.get_domlaunches()
         launches2 = dom2.get_domlaunches()

         self.assert_(len(launches1)>0, "DOM 1 produced launch.")
         self.assert_(len(launches2)>0, "DOM 2 produced launch.")

         self.assert_(launches1[0].lc_bit == True and launches2[0].lc_bit == True,
         "First pair of launches have LC.")
         for launch in launches1:
            self.assert_(launch.time < 7000 ,"No launch during launch decision deadtime.")

tray.AddModule(icetray.I3TestModuleFactory(SanityCheck),'testy',Streams = [icetray.I3Frame.DAQ])


tray.AddModule('TrashCan', 'end')
tray.Execute(3 + 1)
tray.Finish()
