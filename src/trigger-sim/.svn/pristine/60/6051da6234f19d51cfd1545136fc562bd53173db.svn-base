#!/usr/bin/env python
import random
import sys
from os.path import expandvars

from icecube.trigger_sim.trigger_hierarchy_recipes import find_triggers

from I3Tray import *

from icecube import icetray, dataclasses, dataio, trigger_sim
from icecube.trigger_sim.trigger_hierarchy_recipes import *

class TestSource(icetray.I3Module):

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter('NDOMs', '', 100)
        self.AddParameter('DOMSet', '', 4)
        self.AddParameter('GenerateInDOMSet', '', True)
        self.AddParameter('GenerateInTimeWindow', '', True)
        self.AddParameter('TimeWindow', '', 1000.)
        self.AddParameter('DOMSetsName', 'Name of the DOMSets frame object', 'DOMSets')
        self.AddOutBox("OutBox")
        
    def Configure(self):
        self.nDOMs = self.GetParameter('NDOMs')
        self.domSet = self.GetParameter('DOMSet')
        self.generateInDOMSet = self.GetParameter('GenerateInDOMSet')
        self.timeWindow = self.GetParameter('TimeWindow')
        self.generateInTimeWindow = self.GetParameter('GenerateInTimeWindow')
        self.domSetsName = self.GetParameter('DOMSetsName')

    def DAQ(self, frame):
        print("DAQ!!!")

        geo = frame.Get("I3Geometry")

        if self.domSetsName in frame:
            domSets = frame.Get(self.domSetsName)
        else:
            domSets = None # InDOMSet() will use the default table if there is no frame object

        omlist_l = list()
        for omkey,omgeo in geo.omgeo :
            if omgeo.omtype == dataclasses.I3OMGeo.IceCube and \
                  ( ( self.generateInDOMSet and trigger_sim.InDOMSet(omkey,4,domSets) ) or \
                    ( not self.generateInDOMSet and not trigger_sim.InDOMSet(omkey,4,domSets) ) ):
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

        frame["InIceRawData"] = launchmap

        self.PushFrame(frame)

class TestModule(icetray.I3Module):

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter('Trigger', 'ENSURE a trigger exists if true', True)
        self.AddParameter('TriggerName', 'Name of trigger hierarchy in the frame', "I3Triggers")
        self.AddParameter('TriggerSourceID', '', dataclasses.I3Trigger.IN_ICE)
        self.AddParameter('TriggerTypeID', '', dataclasses.I3Trigger.SIMPLE_MULTIPLICITY)
        self.AddParameter('TriggerConfigID', '', 1008)
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
            print("FAIL : trigger hierarchy not found in the frame")
            print(str(frame))
            sys.exit()

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
