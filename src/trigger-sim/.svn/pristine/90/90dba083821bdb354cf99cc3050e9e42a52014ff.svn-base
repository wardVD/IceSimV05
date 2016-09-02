/**
 * copyright  (C) 2006
 * the icecube collaboration
 * $Id: 
 *
 * @file SimpleMajorityTrigger.h
 * @version
 * @date
 * @author toale
 */

#ifndef SIMPLE_MAJORITY_TRIGGER_H
#define SIMPLE_MAJORITY_TRIGGER_H

#include <icetray/I3Module.h>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3Trigger.h>
#include <trigger-sim/algorithms/TriggerHit.h>

typedef std::vector<I3Trigger> SimpleMajorityTriggerList;
I3_POINTER_TYPEDEFS(SimpleMajorityTriggerList);

class SimpleMajorityTrigger : public I3Module
{

 public:
  SimpleMajorityTrigger(const I3Context& context);
  ~SimpleMajorityTrigger();

  void Configure();
  void DetectorStatus(I3FramePtr frame);
  void DAQ(I3FramePtr frame);
  
  void Finish();

 private:

  std::string dataReadoutName_;
  std::string triggerName_;
  std::string domSetsName_;

  int triggerSourceParam_;
  TriggerKey::SourceID triggerSource_;
  int configIDParam_;
  boost::optional<int> configID_;
  double triggerWindow_;
  unsigned int triggerThreshold_;
  boost::optional<int> domSet_;
  TriggerKey triggerKey_;

  int eventCount_;
  int triggerCount_;

  void FillHits(I3DOMLaunchSeriesMapConstPtr fullMap, 
		TriggerHitVectorPtr hitList, 
		I3MapKeyVectorIntConstPtr domSets);
  
  void Dump(TriggerHitVectorPtr input);

  SET_LOGGER("SimpleMajorityTrigger");
};

#endif
