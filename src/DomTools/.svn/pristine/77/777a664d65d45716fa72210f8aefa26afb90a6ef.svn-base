#!/usr/bin/env python

import unittest
from I3Tray import I3Tray, I3Units
from icecube import icetray, dataclasses, DomTools
from icecube.icetray import OMKey

doms = [icetray.OMKey(0,i) for i in range(4)]

def make_launch_map():
    launchmap = dataclasses.I3DOMLaunchSeriesMap()
    for omkey in doms:        
        launch_series = dataclasses.I3DOMLaunchSeries()
        launch_series.append(dataclasses.I3DOMLaunch())
        launchmap[omkey] = launch_series

    launchmap[icetray.OMKey(0,3)].append(dataclasses.I3DOMLaunch())
    return launchmap
    
def source(frame):
    frame["InIceRawData"] = make_launch_map()

class I3DOMLaunchCleaningTest(unittest.TestCase):

    def test_I3DOMLaunchCleaning(self):
        print self.frame
        launchmap = self.frame["CleanInIceRawData"]
        self.assertEqual(len(launchmap), 3, "Got %d DOMs.  Expected 3." % len(launchmap))
        for omkey, launch_series in launchmap:
            self.assertEqual(len(launch_series), 1, "Got %s launches on DOM %s.  Expected 1."\
                             % (len(launch_series), str(omkey)))

tray = I3Tray()
tray.Add("BottomlessSource", stream = icetray.I3Frame.DAQ)
tray.Add(source)
tray.Add("I3DOMLaunchCleaning",
         CleanedKeys = [icetray.OMKey(0,2)],
         FirstLaunchCleaning = True)
tray.Add(icetray.I3TestModuleFactory(I3DOMLaunchCleaningTest))
tray.Execute(100)
tray.Finish()

