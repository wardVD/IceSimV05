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

from icecube.icetray import OMKey

i3_testdata = os.path.expandvars("$I3_TESTDATA")    

GCD_FN = i3_testdata + '/sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz'

def generatePulses(frame):
	# this dom is the one which should launch while its only LC partner (dom 2) is
	# dead from previous launches
	dom1 = OMKey(47,1)
	# this dom will be hit at all three times, first to ensure that both digitizers
	# are busy and then to supply the LC high to dom1
	dom2 = OMKey(47,2)
	# this dom will be pulse twice (time1 and time 2) to ensure that dom2 has LC and
	# makes HLC readouts (to maximize deadtime)
	dom3 = OMKey(47,3)

	#This is a IT DOM to test IT SLC
	dom4 = OMKey(47,62)

	pulseWeight = 1000;

	time1 = 0
	time2 = time1 + 6600
	time3 = time2 + 8000

	pulses = simclasses.I3MCPulseSeriesMap()
	series1 = simclasses.I3MCPulseSeries()
	series2 = simclasses.I3MCPulseSeries()
	series3 = simclasses.I3MCPulseSeries()

	pulse = simclasses.I3MCPulse()
	pulse.charge = pulseWeight
	pulse.time = time1
	series3.append(pulse)
	series2.append(pulse)

	pulse.time = time2
	series3.append(pulse)
	series2.append(pulse)

	pulse.time = time3
	series2.append(pulse)
	series1.append(pulse)

	pulses[dom1]=series1
	pulses[dom2]=series2
	pulses[dom3]=series3

	pulse.charge = 5000
	pulse.time = 500000
	series2.append(pulse)
	pulses[dom4]=series2

	frame.Put("TestPulses",pulses)

tray = I3Tray()

tray.AddService("I3GSLRandomServiceFactory","RandomSource")

tray.AddModule("I3InfiniteSource","FrameMaker", Prefix= GCD_FN)

tray.AddModule(generatePulses,"generatePulses", Streams = [icetray.I3Frame.DAQ])

tray.AddModule("DOMLauncher","Guildenstern",
    Input="TestPulses",
    MergePulses = True,
    UseTabulatedPT = True,
	BeaconLaunches = False
	)

import unittest

#This is a rather complicated LC-logic test where several deadtimes and LC cases are tested.
class SanityCheck(unittest.TestCase):
   domLaunchMapName = "I3DOMLaunchSeriesMap"

   def testKeys(self):
      self.assert_(self.domLaunchMapName in self.frame, \
                   "The launches actually show up in the frame.")

   def testCompoundLaunches(self):
      launchMap = self.frame[self.domLaunchMapName]
      omKeys = [icetray.OMKey(47,1),icetray.OMKey(47,2),icetray.OMKey(47,3)]
      for key in omKeys:
         self.assert_( key in launchMap.keys(), "All DOMs launched at least once.")
      launches =  launchMap[icetray.OMKey(47,1)]
      #print(len(launches), launches[0].lc_bit)
      self.assert_(len(launches) == 1 and launches[0].lc_bit == True, "First DOM correct")

      launch1 = launches[0]
      launches =  launchMap[icetray.OMKey(47,2)]
      self.assert_(len(launches) == 2 and abs(launches[1].time-launch1.time) > 1000, \
                   "Second DOM correct")

tray.AddModule(icetray.I3TestModuleFactory(SanityCheck),'testy',
               Streams = [icetray.I3Frame.DAQ])

tray.AddModule('TrashCan', 'end')
tray.Execute(3 + 10)
tray.Finish()
