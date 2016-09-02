#!/usr/bin/env python

import sys
import math
import os
from I3Tray import I3Units
from I3Tray import I3Tray
from icecube import icetray
from icecube import dataclasses
from icecube import dataio
from icecube import phys_services
from icecube import simclasses
from icecube import DOMLauncher
import numpy as np
tray = I3Tray()

i3_testdata = os.path.expandvars("$I3_TESTDATA")    

GCD_FN = i3_testdata + '/sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz'
tray.AddModule("I3InfiniteSource","FrameMaker", Prefix= GCD_FN)

icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)

import unittest
#This test should check the operation of the discriminator function of the DOM.
#As of now only one test is implemented that check if the discriminator produces (doesn't produce)
#a DOMTrigger when the MCPulse has a charge bigger (smaller) than the discriminator threshold.
class SanityCheck(unittest.TestCase):
   def __init__(self,foobar):
      super(SanityCheck,self).__init__(foobar)
      self.random_service = phys_services.I3GSLRandomService(2)
   def testDiscriminatorThreshold(self):

      #Choosing an arbitrary DOM that we know exists in the detector configuration.
      omKey = icetray.OMKey(47,2)
      geo = self.frame["I3Geometry"].omgeo
      cal = self.frame["I3Calibration"].dom_cal
      stat = self.frame["I3DetectorStatus"].dom_status

      dom = DOMLauncher.I3InIceDOM(self.random_service, omKey)
      dom.configure(cal[omKey],stat[omKey])

      #Creating a sub threshold pulse.
      pulses = simclasses.I3MCPulseSeries()
      pulse = simclasses.I3MCPulse()
      pulse.charge = 0.99 * dom.discriminator_threshold_fraction
      pulse.time = 100
      pulses.append(pulse)

      dcstream = DOMLauncher.DCStream()
      dom.discriminator(pulses, dcstream)

      self.assert_( len(dcstream)==0,
      "No trigger if charge is less than threshold")

      #appending the pulse again so the total amplitude will be above the threshold.
      pulses.append(pulse)
      dom.discriminator(pulses,dcstream)

      self.assert_( len(dcstream)>0,
      "Trigger if charge is greater than threshold")

   def testDiscriminatorEndurance(self):

      #Choosing an arbitrary DOM that we know exists in the detector configuration.
      omKey = icetray.OMKey(42,2)
      geo = self.frame["I3Geometry"].omgeo
      cal = self.frame["I3Calibration"].dom_cal
      stat = self.frame["I3DetectorStatus"].dom_status

      dom = DOMLauncher.I3InIceDOM(self.random_service, omKey)
      dom.configure(cal[omKey],stat[omKey])

      
      for dt in np.linspace(30,130,100):
        for n in xrange(1,120):
            pulses = simclasses.I3MCPulseSeries()
            pulse = simclasses.I3MCPulse()
            dcstream = DOMLauncher.DCStream()
            for n in xrange(0,int(n)):
                pulse.charge = 3
                pulse.time = n*dt
                pulses.append(pulse)
            
            dom.discriminator(pulses, dcstream)
            self.assert_( len(dcstream)>=int(n),
            "Have not missed any discriminator crossings")
            dom.reset(True)
            #if(len(tstream)<int(n)):
                
                #print("Time between pulses: %f\n"%int(dt))
                #print("Number of injected pulses: %d"%n)
                #print("Number of DC: %d\n"%len(tstream))
                #break
            
   def testDiscriminatorStressTest(self):

      #Choosing an arbitrary DOM that we know exists in the detector configuration.
      omKey = icetray.OMKey(42,2)
      geo = self.frame["I3Geometry"].omgeo
      cal = self.frame["I3Calibration"].dom_cal
      stat = self.frame["I3DetectorStatus"].dom_status

      dom = DOMLauncher.I3InIceDOM(self.random_service, omKey)
      dom.configure(cal[omKey],stat[omKey])

      for n in xrange(0,10):
        pulses = simclasses.I3MCPulseSeries()
        pulse = simclasses.I3MCPulse()
        dcstream = DOMLauncher.DCStream()
        #print(n)
        last = 0
        for n in xrange(0,2000):
            pulse.charge = 1
            pulse.time = last + np.random.uniform(31,2000)
            last = pulse.time
            pulses.append(pulse)
          
        dom.discriminator(pulses, dcstream)
        #print(len(dcstream),int(n))
        self.assert_( len(dcstream)>=int(n),
            "Have not missed any discriminator crossings")
        dom.reset(True)


   def testDiscriminatorFindStressTest(self):

      #Choosing an arbitrary DOM that we know exists in the detector configuration.
      omKey = icetray.OMKey(42,2)
      geo = self.frame["I3Geometry"].omgeo
      cal = self.frame["I3Calibration"].dom_cal
      stat = self.frame["I3DetectorStatus"].dom_status

      dom = DOMLauncher.I3InIceDOM(self.random_service, omKey)
      dom.configure(cal[omKey],stat[omKey])
      for n in xrange(0,10):
        dcstream = DOMLauncher.DCStream()
        pulses = simclasses.I3MCPulseSeries()
        pulse = simclasses.I3MCPulse()
        last = 0
        seeds = list()
        for s in  xrange(0,5000):
            seed = last + np.random.uniform(10000,12000)
            seeds.append(seed)
            last = seed
            pulse_times = np.random.normal(seed,1.,10)#np.random.poisson(10))
            
            pulse_times = sorted(pulse_times)
            for pulse_time in pulse_times:
                pulse.charge = 2
                pulse.time =  pulse_time
                #last = pulse.time
                pulses.append(pulse)
        dom.discriminator(pulses, dcstream)
        notfound = 0
        
        #print(len(dcstream),int(5000))
        index = 0
        foundAll = True
        for seed in seeds:
            for i in xrange(index,len(dcstream)):
                    tr = dcstream[i]
                    index = i
                    if(np.abs(seed-tr.time)<400):
                        break
                    elif((tr.time-seed)>400):
                        notfound +=1
                        foundAll = False
                        break
        self.assert_( foundAll,
            "Missed %d discriminator crossings"%notfound)
        
        
tray.AddModule(icetray.I3TestModuleFactory(SanityCheck),'testy',Streams = [icetray.I3Frame.DAQ])


tray.AddModule('TrashCan', 'end')
tray.Execute(3 + 1)
tray.Finish()
