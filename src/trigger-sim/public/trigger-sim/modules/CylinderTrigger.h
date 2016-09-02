/**
 * copyright  (C) 2010
 * the icecube collaboration
 * $Id: 
 *
 * @file CylinderTrigger.h
 * @version
 * @date
 * @author danninger
 */

#ifndef CYLINDER_TRIGGER_H
#define CYLINDER_TRIGGER_H

#include <icetray/I3Module.h>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/I3Map.h>
#include <trigger-sim/algorithms/TriggerHit.h>

class CylinderTrigger : public I3Module
{

 public:
  CylinderTrigger(const I3Context& context);
  ~CylinderTrigger();

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

  double triggerWindow_;
  unsigned int triggerThreshold_;
  double cylinderRadius_; 
  double cylinderHeight_; 
  unsigned int simpleMultiplicity_;
  boost::optional<int> domSet_;

  int eventCount_;
  int triggerCount_;
  TriggerKey triggerKey_;
  
  void FillHits(I3DOMLaunchSeriesMapConstPtr fullMap, 
		TriggerHitVectorPtr hits, 
		I3MapKeyVectorIntConstPtr domSets);

  void Dump(TriggerHitVectorPtr input);

  SET_LOGGER("CylinderTrigger");
};

#endif
