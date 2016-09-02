/**
 * copyright  (C) 2006
 * the icecube collaboration
 * $Id: 
 *
 * @file ClusterTrigger.h
 * @version
 * @date
 * @author toale
 */

#ifndef CLUSTER_TRIGGER_H
#define CLUSTER_TRIGGER_H

#include <icetray/I3Module.h>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/I3Map.h>
#include <trigger-sim/algorithms/TriggerHit.h>

class ClusterTrigger : public I3Module
{

 public:
  ClusterTrigger(const I3Context& context);
  ~ClusterTrigger();

  void Configure();
  void DAQ(I3FramePtr frame);
  void DetectorStatus(I3FramePtr frame);
  
  void Finish();

 private:

  std::string dataReadoutName_;
  std::string triggerName_;
  std::string domSetsName_;

  int configIDParam_;
  boost::optional<int> configID_;
  TriggerKey triggerKey_;
  
  double triggerWindow_;
  unsigned int triggerThreshold_;
  unsigned int coherenceLength_;

  int eventCount_;
  int triggerCount_;


  void FillHits(I3DOMLaunchSeriesMapConstPtr fullMap, 
		TriggerHitVectorPtr hits, 
		I3MapKeyVectorIntConstPtr domSets);
  void Dump(TriggerHitVectorPtr input);

  SET_LOGGER("ClusterTrigger");
};

#endif
