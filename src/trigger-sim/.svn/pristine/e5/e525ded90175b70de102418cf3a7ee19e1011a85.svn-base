#!/usr/bin/env python
from I3Tray import *

from icecube import icetray, dataclasses, dataio, trigger_sim
from icecube.trigger_sim.trigger_hierarchy_recipes import find_triggers

import random

from os.path import expandvars
import sys

class TestSource(icetray.I3Module):

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter('NDOMs', '', 100)
        self.AddParameter('TimeWindow', '', 1000.)
        self.AddParameter('GenerateInTimeWindow', '', True)
        self.AddParameter('DOMType', '', dataclasses.I3OMGeo.IceCube)
        self.AddOutBox("OutBox")
        
    def Configure(self):
        self.nDOMs = self.GetParameter('NDOMs')
        self.timeWindow = self.GetParameter('TimeWindow')
        self.generateInTimeWindow = self.GetParameter('GenerateInTimeWindow')
        self.domType = self.GetParameter('DOMType')

    def DAQ(self, frame):
        print("DAQ!!!")

        geo = frame.Get("I3Geometry")

        omlist_l = list()
        for omkey,omgeo in geo.omgeo :
            if omgeo.omtype == self.domType :
                omlist_l.append(omkey)

        launchmap = dataclasses.I3DOMLaunchSeriesMap()
        launch_counter = 0
        for i in range(self.nDOMs):
            launch = dataclasses.I3DOMLaunch()
            launch.lc_bit = True

            # set start times according to uniform distribution
            time = random.uniform(0,self.timeWindow)
            if not self.generateInTimeWindow :
                time += launch_counter * self.timeWindow

            launch.time = time

            ## select from the available list
            omkey = random.sample(omlist_l,1)[0];

            if not omkey in launchmap :
                launchmap[omkey] = dataclasses.I3DOMLaunchSeries()

            launchmap[omkey].append(launch)

            launch_counter += 1

        if self.domType == dataclasses.I3OMGeo.IceCube :
            frame["InIceRawData"] = launchmap
        if self.domType == dataclasses.I3OMGeo.IceTop :
            frame["IceTopRawData"] = launchmap


        self.PushFrame(frame)

class TestModule(icetray.I3Module):

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter('Trigger', 'ENSURE a trigger exists if true', True)
        self.AddParameter('TriggerName', 'Name of trigger hierarchy in the frame', "I3Triggers")
        self.AddParameter('TriggerSourceID', '', dataclasses.I3Trigger.IN_ICE)
        self.AddParameter('TriggerTypeID', '', dataclasses.I3Trigger.SIMPLE_MULTIPLICITY)
        self.AddParameter('TriggerConfigID', '', 1006)
        self.AddOutBox("OutBox")
        
    def Configure(self):
        self.trigger = self.GetParameter('Trigger')
        self.triggerName = self.GetParameter('TriggerName')
        self.sourceID = self.GetParameter('TriggerSourceID')
        self.typeID = self.GetParameter('TriggerTypeID')
        self.configID = self.GetParameter('TriggerConfigID')

    def DAQ(self, frame):
        print("TestModule : DAQ!!!")

        if frame.Has(self.triggerName):
            triggers = frame.Get(self.triggerName)
        else:
            print("FAIL : trigger heirarchy not found in the frame")
            print(str(frame))
            sys.exit(1)

        print(str(triggers))

        test_key = dataclasses.TriggerKey(self.sourceID, self.typeID, self.configID)
        t = find_triggers(triggers, test_key)
        if not self.trigger :
            # we're testing for the lack of a trigger
            if t : 
                print("FAIL: this was not supposed to trigger")
                sys.exit(1)
        else:
            if not t :
                print("FAIL : trigger (%d,%d,%d) not found in the heirarchy" % \
                      (self.sourceID, self.typeID, self.configID))
                sys.exit(1)

            if not t[0].fired :
                print("FAIL : trigger not fired")
                sys.exit(1)
        
        self.PushFrame(frame)
