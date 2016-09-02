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

PATH = os.getenv('I3_TESTDATA') + '/sim/'
GCD_FN = PATH + 'GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz'
tray.AddModule("I3InfiniteSource","FrameMaker", Prefix= GCD_FN)


random_service = phys_services.I3GSLRandomService(2)

def testTiming(frame):
      global random_service


      #Choosing an arbitrary DOM that we know exists in the detector configuration.
      omKey = icetray.OMKey(42,2)
      #geo = self.frame["I3Geometry"].omgeo
      cal = frame["I3Calibration"].dom_cal
      stat = frame["I3DetectorStatus"].dom_status

      dom = DOMLauncher.I3InIceDOM(random_service, omKey)
      dom.configure(cal[omKey],stat[omKey])



      import time
      #
      pulses = simclasses.I3MCPulseSeries()
      pulse = simclasses.I3MCPulse()
      print("Testing pulses densly distributed pulses")
      N = 2000
      for n in xrange(0,N):
        pulse.charge = 3
        pulse.time = n*20
        pulses.append(pulse)

      tstream = DOMLauncher.DCStream()
      t1_st = time.clock()#stopwatch.Timer()
      dom.discriminator(pulses, tstream)
      t1_end = time.clock()

      print("Discriminator1 elapsed time: %f, and found DCs: %d"%((t1_end-t1_st), len(tstream)))


      print("Testing pulses far apart")
      pulses = simclasses.I3MCPulseSeries()
      pulse = simclasses.I3MCPulse()
      N = 2000
      for n in xrange(0,N):
        pulse.charge = 3
        pulse.time = n*29000
        pulses.append(pulse)

      tstream = DOMLauncher.DCStream()
      t1_st = time.clock()#stopwatch.Timer()
      dom.discriminator(pulses, tstream)
      t1_end = time.clock()

      print("Discriminator1 elapsed time: %f, and found DCs: %d"%((t1_end-t1_st), len(tstream)))


      print("Testing random pulses pulse distribution")
      pulses = simclasses.I3MCPulseSeries()
      pulse = simclasses.I3MCPulse()
      last_t = 0
      N = 2000
      for n in xrange(0,N):
        pulse.charge = 3
        pulse.time = last_t + np.random.uniform(10,29000)
        last_t = pulse.time
        pulses.append(pulse)

      tstream = DOMLauncher.DCStream()
      t1_st = time.clock()#stopwatch.Timer()
      dom.discriminator(pulses, tstream)
      t1_end = time.clock()

      print("Discriminator1 elapsed time: %f, and found DCs: %d"%((t1_end-t1_st), len(tstream)))

#test = SanityCheck()
tray.AddModule(testTiming,'testy',Streams = [icetray.I3Frame.DAQ])


tray.AddModule('TrashCan', 'end')
tray.Execute(3 + 1)
tray.Finish()
