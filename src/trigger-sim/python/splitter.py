"""
 * class: TriggerSplitter
 *
 * Version $Id: $
 *
 *
 * (c) 2009 IceCube Collaboration
 * @file TriggerSplitter.cxx
 * @date $Date: $
 * @author juancarlos
"""

from icecube import icetray,dataclasses


class Trig:
    def __init__(self,time,semaphore):
      self.time = time
      self.semaphore = semaphore # 1:start,-1:stop

def BuildSubTree(newtree,oldtree,parent):
    for daughter in oldtree.get_daughters(parent):
        newtree.append_child(parent,daughter)
        BuildSubTree(newtree,oldtree,daughter)


class EventSplitter(icetray.I3Module):

    eventID=0
    filterMode=True
    ReadoutWindowBefore = 10.0 
    ReadoutWindowAfter  = 10.0

    def __init__(self,ctx):

      icetray.I3Module.__init__(self,ctx)
      self.AddParameter("I3TriggerName","InIce Trigger name in frame","I3Triggers")
      self.AddParameter("I3ReadoutWindowBefore", 
                      "Readout window before the trigger",
                      6.0*icetray.I3Units.microsecond)
      self.AddParameter("I3ReadoutWindowAfter", 
                      "Readout window after the trigger",
                      4.0*icetray.I3Units.microsecond)
      self.AddParameter("I3MCTreeName", "Name of I3MCTree to split","I3MCTree")
      self.AddParameter("I3MCHitSeriesMapName", 
                      "Names of hitseries objects that to split","MCHitSeriesMap");
      domLaunchSeriesMapNames_ = ["InIceRawData","IceTopRawData"];
      self.AddParameter("I3DOMLaunchSeriesMapNames",
		 "This holds the DOM launches used to determine event lengths for untriggered events.",
		 domLaunchSeriesMapNames_);
      self.AddParameter("MCWeightMapName","name of weightmap","CorsikaWeightMap")
      self.AddParameter("MCTreeName","name of I3MCTree","I3MCTree")
 
      self.AddOutBox("OutBox");

    def Configure(self):

      self.i3TriggName           = self.GetParameter("I3TriggerName")
      self.ReadoutWindowBefore   = self.GetParameter("I3ReadoutWindowBefore")
      self.ReadoutWindowAfter    = self.GetParameter("I3ReadoutWindowAfter")
      self.domLaunchSeriesMapNames = self.GetParameter("I3DOMLaunchSeriesMapNames")
      self.hitSeriesMapName       = self.GetParameter("I3MCHitSeriesMapName")
      self.weights_name           = self.GetParameter("MCWeightMapName")
      self.mctreename             = self.GetParameter("MCTreename")

    def DAQ(self,frame):

      if not frame.Has(self.i3TriggName):
            print("No IceCube triggers found ('%s')." % self.i3TriggName)
            self.PushFrame(frame);
            return;

      i3Triggers = frame.Get(self.i3TriggName);
      if not i3Triggers:
            print("Zero triggers found ('%s')." % self.i3TriggName)
            return;

      #triggers = dataclasses.I3TriggerHierarchy();
      triggers = []
      triggertimes = []
      # build a semaphore queue to keep track of open triggers
      for i in i3Triggers:
         print(i);
         triggertimes.append(Trig(i.time,1))
         triggertimes.append(Trig(i.time+i.length,-1))
         triggers.append(i)

      triggertimes.sort(key=lambda t: t.time);
      triggers.sort(key=lambda t: t.time);

      mindthegap = []
      # Set the first gap at the beggining of the event
      mindthegap.append(triggertimes[0].time);
      semaphore = 0
      for t in triggertimes:
         semaphore += t.semaphore
         print(semaphore,t.time)
         if semaphore == 0: # This is true iff there are not active triggers
            mindthegap.append(t.time);
      
      """
      * Now that earliest and latest are set we can set the start and stop times
      * of the events accordingly. 
      """
      dt = dataclasses.I3Time();
      
      if frame.Has("I3EventHeader"):
        eh =frame.Get("I3EventHeader") #NOTE <mzoll> this has changed from DrivingTime to I3EventHeader.start_time
        dt =eh.start_time;
      else:
        eh =dataclasses.I3EventHeader()
        dt.set_mod_julian_time(55697, 43200, 0.0);
	
      tend, tstart = 0.0,0.0;
      tstart = mindthegap[0]
      from icecube import trigger_sim
      for k in mindthegap[1:]:
           tend=k;
           newhierarchy = dataclasses.I3TriggerHierarchy();

           drivingtime = dataclasses.I3Time(dt + tstart);
           size=0
           for j in triggers:
              if (j.time < tstart): continue
              if (j.time >= tend): break
              newhierarchy.insert(j)
              size += 1;

           t     = dataclasses.I3Double(tstart);
           count = dataclasses.I3Double(size);
           eh.start_time 	= drivingtime
	   
           newframe         = icetray.I3Frame(icetray.I3Frame.DAQ);

           newframe.Delete("I3EventHeader") #NOTE <mzoll> this has changed from DrivingTime to I3EventHeader.start_time
           newframe.Put("I3EventHeader", eh)
           newframe.Put("triggertime",t);
           newframe.Put("trigersize",count);
           newframe.Put("triggers",newhierarchy);
        
           # split launches
           for mapname in self.domLaunchSeriesMapNames:
               if frame.Has(mapname):
                  fullMap          = frame.Get(mapname)
                  InIceResponseMap = dataclasses.I3DOMLaunchSeriesMap()
                  self.SplitLaunches(fullMap, InIceResponseMap,
                                  tstart-self.ReadoutWindowBefore, tend+self.ReadoutWindowAfter)
                  newframe.Delete(mapname)
                  newframe.Put(mapname,InIceResponseMap)
           # split hits
           fullMap          = frame.Get(self.hitSeriesMapName)
           newmchitmap      = dataclasses.I3MCHitSeriesMap()
           self.SplitMCHits(fullMap, newmchitmap, 
                                  tstart-self.ReadoutWindowBefore, tend+self.ReadoutWindowAfter)
           newframe.Delete(mapname)
           newframe.Put(mapname,newmchitmap)

           if not newframe.Has(self.mctreename):
              oldtree = frame.Get(self.mctreename)
              newtree = self.PruneTree(oldtree,newmchitmap)
              newframe.Put(self.mctreename,newtree)
              weights = frame.Get(self.weights_name)
              weights["multiplicity"] = len(newtree.primaries)
              newframe.Put(self.weights_name,weights)

           self.eventID+=1
           self.PushFrame(newframe);
           tstart=tend;


    def SplitLaunches(self,fullMap, submap, tstart, tend):
        for omKey,fullSeries in fullMap.items():
            for fullSeriesIter in fullSeries:
	             if fullSeriesIter.time > tstart and fullSeriesIter.time <= tend:
	                   if not submap.has_key(omKey):
	                      submap[omKey]=dataclasses.I3DOMLaunchSeries();
	                   submap[omKey].append(fullSeriesIter);

    def SplitMCHits(self,fullMap, submap, tstart, tend):
        for omKey,fullSeries in fullMap.items():
            for fullSeriesIter in fullSeries:
	             if fullSeriesIter.time > tstart and fullSeriesIter.time <= tend:
	                  if not submap.has_key(omKey):
	                      submap[omKey]=dataclasses.I3MCHitSeries();
	                  submap[omKey].append(fullSeriesIter);

    def PruneTree(self,mctree,hitseries_map):

      keep = {}

      for omkey,hitseries in hitseries_map.items():
         for hit in hitseries:
           if hit.hit_source != dataclasses.I3MCHit.SPE: continue
           parent = mctree.get_particle_from_hit(hit)
           if parent:
              primary = mctree.get_primary(parent)
              keep[ (primary.major_id,primary.minor_id) ] = primary

      newtree = dataclasses.I3MCTree()
      for key,primary in keep.items():
          newtree.add_primary(primary)
          BuildSubTree(newtree,mctree,primary)

      return newtree
