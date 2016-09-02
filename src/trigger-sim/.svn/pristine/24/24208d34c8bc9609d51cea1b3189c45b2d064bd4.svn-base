#include <algorithm>
#include <vector>

#include <boost/foreach.hpp>

#include <trigger-sim/utilities/TimeShifterUtils.h>
#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3MCHit.h>
#include "dataclasses/physics/I3MCTree.h"
#include "simclasses/I3MMCTrack.h"
#include "dataclasses/physics/I3Trigger.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/I3Double.h"
#include "dataclasses/physics/I3FlasherInfo.h"
#include "simclasses/I3MCPE.h"
#include "simclasses/I3MCPulse.h"

I3MCHitSeriesMapPtr
Shift(I3MCHitSeriesMapConstPtr map, double dt){
  I3MCHitSeriesMapPtr newmap(new I3MCHitSeriesMap(*map));
  BOOST_FOREACH(I3MCHitSeriesMap::reference hs_pair, *newmap){
    BOOST_FOREACH(I3MCHit& h, hs_pair.second){
      h.SetTime(h.GetTime() - dt);
    }
  }
  return newmap;
}

I3MCTreePtr
Shift(I3MCTreeConstPtr tree, double dt){
  I3MCTreePtr new_tree(new I3MCTree(*tree));
  I3MCTree::iterator t_iter;
  for(t_iter = new_tree->begin();
      t_iter != new_tree->end();
      t_iter++){
    t_iter->SetTime(t_iter->GetTime() - dt);
  }
  return new_tree;
}

I3MMCTrackListPtr 
Shift(I3MMCTrackListConstPtr tracks, double dt){
  I3MMCTrackListPtr new_mmctracklist(new I3MMCTrackList(*tracks)); 
	
  BOOST_FOREACH(I3MMCTrackList::reference track, *new_mmctracklist){ 
    double t_0( track.GetI3Particle().GetTime() ); 
    I3Particle p( track.GetI3Particle() ); 
    p.SetTime(t_0 - dt); 
    track.SetParticle( p ); 
    
    track.ti -= dt ; 
    track.tc -= dt ; 
    track.tf -= dt ; 
  } 
  return new_mmctracklist;
}

I3DOMLaunchSeriesMapPtr
Shift(I3DOMLaunchSeriesMapConstPtr launches, double dt){
  I3DOMLaunchSeriesMapPtr new_launchmap(new I3DOMLaunchSeriesMap(*launches));
  
  BOOST_FOREACH(I3DOMLaunchSeriesMap::reference pair, *new_launchmap){
    BOOST_FOREACH(I3DOMLaunch& l, pair.second)
      l.SetStartTime(l.GetStartTime() - dt);
  }
  return new_launchmap;
}

I3TriggerHierarchyPtr
Shift(I3TriggerHierarchyConstPtr tree, double dt){
  I3TriggerHierarchyPtr new_tree(new I3TriggerHierarchy(*tree));
  I3TriggerHierarchy::iterator t_iter;
  for(t_iter = new_tree->begin();
      t_iter != new_tree->end();
      t_iter++){
    t_iter->SetTriggerTime(t_iter->GetTriggerTime() - dt);
  }
  return new_tree;
}

I3VectorI3TriggerPtr
Shift(I3VectorI3TriggerConstPtr triggers, double dt){
  I3VectorI3TriggerPtr new_triggers(new I3VectorI3Trigger(*triggers));
  I3VectorI3Trigger::iterator t_iter;
  for(t_iter = new_triggers->begin();
      t_iter != new_triggers->end();
      t_iter++){
    t_iter->SetTriggerTime(t_iter->GetTriggerTime() - dt);
  }
  return new_triggers;
}

I3DoublePtr
Shift(I3DoubleConstPtr i3double, double dt){
  return I3DoublePtr( new I3Double( i3double->value - dt));
}

I3ParticlePtr
Shift(I3ParticleConstPtr i3particle, double dt){
  I3ParticlePtr new_particle( new I3Particle(*i3particle) );
  new_particle->SetTime(i3particle->GetTime() - dt);
  return new_particle;
}

I3FlasherInfoVectPtr
Shift(I3FlasherInfoVectConstPtr oldflasher, double dt){
  I3FlasherInfoVectPtr flasherInfoVect(new I3FlasherInfoVect());
  I3FlasherInfoVect::const_iterator flashiter;
  for (flashiter = oldflasher->begin(); flashiter != oldflasher->end(); flashiter++) {
    I3FlasherInfoPtr newflasher(new I3FlasherInfo(*flashiter));
    newflasher->SetFlashTime(flashiter->GetFlashTime() - dt);
    flasherInfoVect->push_back(*newflasher);
  }
  return flasherInfoVect;
}

I3MCPESeriesMapPtr
Shift(I3MCPESeriesMapConstPtr map, double dt){
  I3MCPESeriesMapPtr newmap(new I3MCPESeriesMap(*map));
  BOOST_FOREACH(I3MCPESeriesMap::reference hs_pair, *newmap){
    BOOST_FOREACH(I3MCPE& h, hs_pair.second){
      h.time = h.time - dt;
    }
  }
  return newmap;
}

I3MCPulseSeriesMapPtr
Shift(I3MCPulseSeriesMapConstPtr map, double dt){
  I3MCPulseSeriesMapPtr newmap(new I3MCPulseSeriesMap(*map));
  BOOST_FOREACH(I3MCPulseSeriesMap::reference hs_pair, *newmap){
    BOOST_FOREACH(I3MCPulse& h, hs_pair.second){
      h.time = h.time - dt;
    }
  }
  return newmap;
}

I3VectorI3ParticlePtr
Shift(I3VectorI3ParticleConstPtr vect, double dt){
  I3VectorI3ParticlePtr newvect(new I3VectorI3Particle(*vect));
  BOOST_FOREACH(I3Particle p, *newvect){
    p.SetTime(p.GetTime() - dt);
  }
  return newvect;
}

template<class T>
bool ShiftAndReplaceInFrame(I3FramePtr frame, const std::string &key, double dt)
{
  typedef boost::shared_ptr<const T> TConstPtr;

  TConstPtr ptr;
  try{
    ptr = frame->Get<TConstPtr>(key);
  } catch (const boost::archive::archive_exception& e) {
    switch (e.code) {
    case boost::archive::archive_exception::unregistered_class:
      log_error("Object with frame key '%s' is not registered.", key.c_str());
      throw;
      break;
    default:
      throw;
    }
  }
  
  if (!ptr) return false; // not in frame or won't cast

  // exists. shift it and replace it.
  frame->Delete(key);
  frame->Put(key, Shift(ptr,dt));
  return true;
}

void TimeShifterUtils::ShiftFrameObjects(I3FramePtr frame, 
                                         double dt,
                                         const std::vector<std::string>& skip_keys,
                                         const std::vector<std::string>& shift_i3double_keys){
  
  BOOST_FOREACH(const std::string& key, frame->keys())
  {
    // don't shift mixed-in items
    if (frame->GetStop(key) != frame->GetStop()) 
      continue;    

    if(std::count(skip_keys.begin(),skip_keys.end(),key))
      continue;

    ShiftAndReplaceInFrame<I3MCTree>(frame, key, dt);
    ShiftAndReplaceInFrame<I3MCHitSeriesMap>(frame, key, dt);
    ShiftAndReplaceInFrame<I3MMCTrackList>(frame, key, dt);
    ShiftAndReplaceInFrame<I3DOMLaunchSeriesMap>(frame, key, dt);
    ShiftAndReplaceInFrame<I3VectorI3Trigger>(frame, key, dt);
    ShiftAndReplaceInFrame<I3TriggerHierarchy>(frame, key, dt);
    ShiftAndReplaceInFrame<I3FlasherInfoVect>(frame, key, dt);
    ShiftAndReplaceInFrame<I3MCPESeriesMap>(frame, key, dt);
    ShiftAndReplaceInFrame<I3MCPulseSeriesMap>(frame, key, dt);
    ShiftAndReplaceInFrame<I3Particle>(frame, key, dt);
    ShiftAndReplaceInFrame<I3VectorI3Particle>(frame, key, dt);

    if(std::count(shift_i3double_keys.begin(), shift_i3double_keys.end(), key))
      ShiftAndReplaceInFrame<I3Double>(frame, key, dt);
  }
}
