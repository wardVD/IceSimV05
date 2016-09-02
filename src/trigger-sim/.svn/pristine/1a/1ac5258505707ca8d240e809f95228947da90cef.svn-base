/**
 * copyright  (C) 2006
 * the icecube collaboration
 * $Id: 
 *
 * @file MultiplicityStringTrigger.h
 * @version
 * @date
 * @author toale
 */

#ifndef MULTIPLICITY_STRING_TRIGGER_H
#define MULTIPLICITY_STRING_TRIGGER_H

#include <icetray/I3Module.h>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/I3Map.h>
#include <trigger-sim/algorithms/TriggerHit.h>

/**
 * This is useful for storing string trigger information.
 * There's really no place for in the TriggerKey, I3Trigger,
 * or I3TriggerHierarchy.
 */
typedef I3Map<int, std::vector<I3Trigger> > MultiplicityStringTriggerMap;
I3_POINTER_TYPEDEFS(MultiplicityStringTriggerMap);

class MultiplicityStringTrigger : public I3Module
{

 public:
  MultiplicityStringTrigger(const I3Context& context);
  ~MultiplicityStringTrigger();

  void Configure();
  void DAQ(I3FramePtr frame);
  void DetectorStatus(I3FramePtr frame);
  void Finish();

 private:

  std::string dataReadoutName_;
  std::string triggerName_;

  int configIDParam_;
  boost::optional<int> configID_;

  double triggerWindow_;
  unsigned int triggerThreshold_;
  unsigned int maxSpan_;
  std::string vetoDepthName_;
  boost::optional<unsigned> vetoDepth_;
  boost::optional<int> stringNumber_;
  int eventCount_;
  int triggerCount_;

  TriggerKey triggerKey_;

  void FillHits(I3DOMLaunchSeriesMapConstPtr fullMap, 
		IntTriggerHitVectorMapPtr stringMap);
  void Dump(TriggerHitVectorPtr input);

  int GetTrigStatusSetting(const I3TriggerStatus&, const std::string&);

  void ConfigureFromDetStatus(const I3DetectorStatus& detstatus);

  SET_LOGGER("MultiplicityStringTrigger");
};

#endif
