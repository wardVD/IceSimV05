#!/usr/bin/env python

import unittest
from I3Tray import I3Tray, I3Units
from icecube import icetray, dataclasses, DomTools
from icecube.icetray import OMKey

class I3TimeWindowFilterTest(unittest.TestCase):
    def test_NoCleaning(self):
        self.assertTrue(self.frame.Has("NoCleaningTestCleaned"))
        rpmap = dataclasses.I3RecoPulseSeriesMap.from_frame(self.frame, "NoCleaningTestCleaned")
        self.assertEqual(len(rpmap), 4, "Got %d DOMs.  Expected 4." % len(rpmap))

    def test_EarlyHit(self):
        self.assertTrue(self.frame.Has("EarlyHitTestCleaned"))
        rpmap = dataclasses.I3RecoPulseSeriesMap.from_frame(self.frame, "EarlyHitTestCleaned")
        self.assertEqual(len(rpmap), 3, "Got %d DOMs.  Expected 3." % len(rpmap))

    def test_LateHit(self):
        self.assertTrue(self.frame.Has("LateHitTestCleaned"))
        rpmap = dataclasses.I3RecoPulseSeriesMap.from_frame(self.frame, "LateHitTestCleaned")
        self.assertEqual(len(rpmap), 3, "Got %d DOMs.  Expected 3." % len(rpmap))

class SourceModule(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("HitTiming","List of 4 times to use to set the source pulses.", list())
        self.AddParameter("OutputMapName", "Frame key", "")
        self.AddOutBox("OutBox")
        
    def Configure(self):
        self.times = self.GetParameter("HitTiming")
        self.output_map_name = self.GetParameter("OutputMapName")

    def Physics(self, frame):
        rpsmap = dataclasses.I3RecoPulseSeriesMap()
        doms = [icetray.OMKey(0,i) for i in range(4)]
        for omkey, time in zip(doms, self.times):
            rp = dataclasses.I3RecoPulse()
            rp.time = time
            rps = dataclasses.I3RecoPulseSeries()
            rps.append(rp)
            rpsmap[omkey] = rps
        frame[self.output_map_name] = rpsmap
        self.PushFrame(frame)        

tray = I3Tray()
tray.Add("BottomlessSource")
tray.Add(SourceModule, 
         OutputMapName = "NoCleaningTest",
         HitTiming = [0,400,-400,0])
tray.Add("I3TimeWindowCleaning<I3RecoPulse>",
         InputResponse = "NoCleaningTest",
         OutputResponse = "NoCleaningTestCleaned",
         TimeWindow = 1000.) # Units?
tray.Add(SourceModule,
         OutputMapName = "EarlyHitTest",
         HitTiming = [-1200, 0, 0, 0])
tray.Add("I3TimeWindowCleaning<I3RecoPulse>",
         InputResponse = "EarlyHitTest",
         OutputResponse = "EarlyHitTestCleaned",
         TimeWindow = 1000.) # Units?
tray.Add(SourceModule,
         OutputMapName = "LateHitTest",
         HitTiming = [1200, 0, 0, 0])
tray.Add("I3TimeWindowCleaning<I3RecoPulse>",
         InputResponse = "LateHitTest",
         OutputResponse = "LateHitTestCleaned",
         TimeWindow = 1000.) # Units?
tray.Add(icetray.I3TestModuleFactory(I3TimeWindowFilterTest))

tray.Execute(100)
tray.Finish()

