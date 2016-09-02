#!/usr/bin/env python
from I3Tray import *

from icecube import icetray, dataclasses, dataio, trigger_sim

import random
from random import choice, randint 

from os.path import expandvars
import sys

class TestSourceSpace(icetray.I3Module):

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter('StringN','',1)
        self.AddParameter('NDOMs', '', 60)
        self.AddParameter('TimeWindow', '', 1500.)
        self.AddParameter('TriggerThreshold', '', 5)
        self.AddParameter('GenerateInTimeWindow', '', True)
        self.AddParameter('TrigTest','',True)

    def Configure(self):
        self.stringN = self.GetParameter('StringN')
        self.nDOMs = self.GetParameter('NDOMs')
        self.timeWindow = self.GetParameter('TimeWindow')
        self.threshold = self.GetParameter('TriggerThreshold')
        self.generateInTimeWindow = self.GetParameter('GenerateInTimeWindow')
        self.trigTest = self.GetParameter('TrigTest')

    def DAQ(self, frame):
        print("DAQ!!!")

        geo = frame.Get("I3Geometry")

        omlist_l = list ()
        for omkey,omgeo in geo.omgeo :
            if omkey.GetString() == self.stringN:
                if omgeo.omtype == dataclasses.I3OMGeo.IceCube :
                    omlist_l.append(omkey)
        
        time=100.*I3Units.ns

        if not self.trigTest:
            print("You are TESTING <<THE NON-EXISTENSE>> of a STRING TRIGGER")
            
            
            #Get non adjacent  (self.threshold) OMs along a string
            noms = [random.randrange(1,58,7) for x in range(self.threshold-2)]
            #make sure that they are not adjacent
            noms.extend([0,59])
            
                
            
            #create a list of DOMLaunches which do fulfill a string trigger condition

            launchmap = dataclasses.I3DOMLaunchSeriesMap()

            for i in range(self.threshold):
                
                launch = dataclasses.I3DOMLaunch()
                launch.SetLCBit(True)

                time +=i*100*I3Units.ns
                launch.SetStartTime(time)
                
                #choose randomly 5/3 oms from doms between 50 and 56 in string number 1
                omkey = omlist_l[noms[i]]
                print(omkey)
             
                if not omkey in launchmap :
                    launchmap[omkey] = dataclasses.I3DOMLaunchSeries()

                launchmap[omkey].append(launch)
            
            frame["InIceRawData"] = launchmap
            
        else:
            print("You are TESTING <<THE- EXISTENSE>> of a STRING TRIGGER")
            
            #create a list of DOMLaunches which "DO NOT" fulfill a string trigger condition
            launchmap = dataclasses.I3DOMLaunchSeriesMap()

            for i in range(self.threshold):
                
                launch = dataclasses.I3DOMLaunch()
                launch.SetLCBit(True)
                
                time +=i*100.*I3Units.ns
                launch.SetStartTime(time)

                #choose randomly 5 oms from doms between 30 and 37 in string number 1
                omkey = random.sample(omlist_l[30:36],1)[0];
                
                if not omkey in launchmap :
                    launchmap[omkey] = dataclasses.I3DOMLaunchSeries()
                    
                launchmap[omkey].append(launch)
                    
            frame["InIceRawData"] = launchmap

        self.PushFrame(frame)




class TestSourceTime(icetray.I3Module):

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter('StringN','',1)
        self.AddParameter('NDOMs', '', 60)
        self.AddParameter('TimeWindow', '', 1500.)
        self.AddParameter('TriggerThreshold', '', 5)
        self.AddParameter('GenerateInTimeWindow', '', True)
        self.AddParameter('TrigTest','',True)
        self.AddParameter("PutWhere", "wher trigger length goes", "TriggerLength")

    def Configure(self):
        self.stringN = self.GetParameter('StringN')
        self.nDOMs = self.GetParameter('NDOMs')
        self.timeWindow = self.GetParameter('TimeWindow')
        self.threshold = self.GetParameter('TriggerThreshold')
        self.generateInTimeWindow = self.GetParameter('GenerateInTimeWindow')
        self.trigTest = self.GetParameter('TrigTest')
        self.where = self.GetParameter("PutWhere")
        
    def DAQ(self, frame):
        print("DAQ!!!")

        geo = frame.Get("I3Geometry")

        omlist_l = list ()
        for omkey,omgeo in geo.omgeo :
            if omkey.GetString() == self.stringN:
                if omgeo.omtype == dataclasses.I3OMGeo.IceCube :
                    omlist_l.append(omkey)


        oms_l = list ()
        noms = [random.randint(0,59) for r in range(self.nDOMs)]
        noms.sort()
        ajcent = True
        while(ajcent):
            k=random.randint(0,len(noms)-1)
            
            if (k+7) < len(noms):
        
                if ( noms[k+7] - noms[k] <= 7 and noms[k+7] - noms[k]>=0 ):
                    for i in range(k,k+7,1):
                        oms_l.append(noms[i])        
                    ajcent = False
                else:
                    continue
                
        noTimeCorr = True
        while(noTimeCorr):
            time_l = [random.random()*self.timeWindow*I3Units.ns for x in range(self.threshold)]
            time_l.sort()
            if ((time_l[-1] - time_l[0]) <= self.timeWindow*I3Units.ns):
                noTimeCorr = False
            else:
                continue

        TriggerLengthExpectation =  time_l[-1] - time_l[0]
        trigLength = dataclasses.I3Double(TriggerLengthExpectation*I3Units.ns)
        frame.Put(self.where, trigLength)
        

        time=100.*I3Units.ns
        if self.trigTest:

            print("You are TESTING <<THE EXISTENSE>> of a STRING TRIGGER")
            
            #create a list of DOMLaunches which do fulfill a string trigger condition

            launchmap = dataclasses.I3DOMLaunchSeriesMap()
            
            for i in range(self.threshold):
                
                omkey = omlist_l[random.sample(oms_l,1)[0]]
                
                # for the slow monopole case we had about 60 launches per OM
               
                launch = dataclasses.I3DOMLaunch()
                launch.SetLCBit(True)

                launch.SetStartTime(time_l[i])

                if not omkey in launchmap :
                    launchmap[omkey] = dataclasses.I3DOMLaunchSeries()
                    
                launchmap[omkey].append(launch)
            
            frame["InIceRawData"] = launchmap

            
        else:
            print("You are TESTING <<NON-EXISTENSE>> of a STRING TRIGGER")
            

            #create a list of DOMLaunches which "DO NOT" fulfill a string trigger condition
            launchmap = dataclasses.I3DOMLaunchSeriesMap()

            for i in range(self.threshold):

                launch = dataclasses.I3DOMLaunch()
                launch.SetLCBit(True)
                
                time +=i*1000.*I3Units.ns
                launch.SetStartTime(time)

                #choose randomly 5 oms from doms between 30 and 37 in string number 1
                omkey = random.sample(omlist_l[50:56],1)[0];
                
                if not omkey in launchmap :
                    launchmap[omkey] = dataclasses.I3DOMLaunchSeries()
                    
                launchmap[omkey].append(launch)
                    
            frame["InIceRawData"] = launchmap
        
        self.PushFrame(frame)
            


class TestModule(icetray.I3Module):

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter('Trigger', 'ENSURE a trigger exists if true', True)
        self.AddParameter('TriggerName', 'Name of trigger hierarchy in the frame', "I3Triggers")
        self.AddParameter('TriggerSourceID', '', 0)
        self.AddParameter('TriggerTypeID', '', 120)
        self.AddParameter('TriggerConfigID', '', 1007)
        self.AddParameter('TriggerLength','Name of the trigger length var. in the frame','TriggerLength')
        self.AddParameter('TriggerTolerance','timeLength diff. between the injected and calculated',1.)
        
    def Configure(self):
        self.trigger = self.GetParameter('Trigger')
        self.triggerName = self.GetParameter('TriggerName')
        self.sourceID = self.GetParameter('TriggerSourceID')
        self.typeID = self.GetParameter('TriggerTypeID')
        self.configID = self.GetParameter('TriggerConfigID')
        self.triggerLength = self.GetParameter('TriggerLength')
        self.tolerance = self.GetParameter('TriggerTolerance')

    def DAQ(self, frame):
        
        if frame.Has(self.triggerName):
            triggers = frame.Get(self.triggerName)
            triggerLengthExpectation = frame.Get(self.triggerLength)
            
        else:
            print("FAIL : trigger heirarchy not found in the frame")
            print(str(frame))
            sys.exit(1)
        
        print(str(triggers))

        t = triggers.FindTrigger(self.sourceID, self.typeID, self.configID)

        
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

            if not t.Fired :
                print("FAIL : trigger not fired")
                sys.exit(1)
            if abs(triggerLengthExpectation.value*I3Units.ns - t.Length*I3Units.ns) > self.tolerance*I3Units.ns:
                print("FAIL: Trigger Length %d, evaluated wrong" % t.Length)
                sys.exit(1)
                
        self.PushFrame(frame)
