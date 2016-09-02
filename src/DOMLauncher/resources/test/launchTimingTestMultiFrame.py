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
n = 1

def generatePulses(frame):
    global n
    dom1 = OMKey(47,1)
    dom2 = OMKey(47,2)
    dom3 = OMKey(47,15)
    
    dom4 = OMKey(47,25)
    dom5 = OMKey(47,26)
  
    pulseWeight = 10
    pulses = simclasses.I3MCPulseSeriesMap()
    series = simclasses.I3MCPulseSeries()
    
    pulse = simclasses.I3MCPulse()
    pulse.charge = pulseWeight
  
    if(n==1):
        time1 = 0
        time2 = 29*icetray.I3Units.microsecond
        
        
    else:
        time1 = 2*29*icetray.I3Units.microsecond
        time2 = 3*29*icetray.I3Units.microsecond
        series2 = simclasses.I3MCPulseSeries()        
        series3 = simclasses.I3MCPulseSeries()
        
    pulse.time = time1
    series.append(pulse)
    pulse.time = time2
    series.append(pulse)
    
    pulses[dom1]=series
    pulses[dom2]=series
    pulses[dom3]=series

    if(n == 1):
        pulses[dom4]=series
    else:
        pulse.time = 29*icetray.I3Units.microsecond+200
        series = simclasses.I3MCPulseSeries()
        series.append(pulse)
        pulses[dom5]=series
        
        
    frame.Put("TestPulses",pulses)

tray = I3Tray()

tray.AddService("I3GSLRandomServiceFactory","RandomSource")

tray.AddModule("I3InfiniteSource","FrameMaker", Prefix= GCD_FN)

tray.AddModule(generatePulses,"generatePulses", Streams = [icetray.I3Frame.DAQ])

tray.AddModule("DOMLauncher","Guildenstern",
    Input="TestPulses",
    MergePulses = True,
    UseTabulatedPT = True,
    BeaconLaunches = False,
    MultiFrameEvents = True
    )

import unittest

#This is a rather complicated LC-logic test where several deadtimes and LC cases are tested.
class SanityCheck(unittest.TestCase):
    domLaunchMapName = "I3DOMLaunchSeriesMap"

    def testKeys(self):
        self.assert_(self.domLaunchMapName in self.frame, \
                   "The launches actually show up in the frame.")

    def testCompoundLaunches(self):
        global n
        print("This is frame number: %d"%n)
        if(n == 1):
            launchMap = self.frame[self.domLaunchMapName]
            omKeys = [icetray.OMKey(47,1),icetray.OMKey(47,2),icetray.OMKey(47,15), icetray.OMKey(47,25)]
            for key in omKeys:
                self.assert_( key in launchMap.keys(), "All DOMs launched at least once.")
            launches =  launchMap[icetray.OMKey(47,1)]
            self.assert_(len(launches) == 2 and launches[0].lc_bit == True, "First DOM correct")
            launches =  launchMap[icetray.OMKey(47,2)]
            self.assert_(len(launches) == 2 and launches[0].lc_bit == True, "Second DOM correct")
            launches =  launchMap[icetray.OMKey(47,15)]
            self.assert_(len(launches) == 1 and launches[0].lc_bit == False, "Third DOM correct")
            launches =  launchMap[icetray.OMKey(47,25)]
            self.assert_(len(launches) == 1 and launches[0].lc_bit == False, "Fourth DOM correct")
        else:
            launchMap = self.frame[self.domLaunchMapName]
            omKeys = [icetray.OMKey(47,1),icetray.OMKey(47,2),icetray.OMKey(47,15), icetray.OMKey(47,25), icetray.OMKey(47,26)]
            for key in omKeys:
                self.assert_( key in launchMap.keys(), "All DOMs launched at least once.")
            launches =  launchMap[icetray.OMKey(47,1)]
            self.assert_(len(launches) == 2 and launches[0].lc_bit == True, "First DOM correct")
            launches =  launchMap[icetray.OMKey(47,2)]
            self.assert_(len(launches) == 2 and launches[0].lc_bit == True, "Second DOM correct")
            launches =  launchMap[icetray.OMKey(47,15)]
            self.assert_(len(launches) == 2 and launches[0].lc_bit == False, "Third DOM correct")
            launches =  launchMap[icetray.OMKey(47,25)]
            self.assert_(len(launches) == 1 and launches[0].lc_bit == True, "Fourth DOM correct")
        n+=1    

tray.AddModule(icetray.I3TestModuleFactory(SanityCheck),'testy',
               Streams = [icetray.I3Frame.DAQ])

tray.AddModule('TrashCan', 'end')
tray.Execute(3 + 2)
tray.Finish()
