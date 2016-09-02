/**
 * copyright  (C) 2006
 * the icecube collaboration
 * $Id: 
 *
 * @file SimpleMajorityTriggerAlgorithm.h
 * @version
 * @date
 * @author toale
 */

#ifndef SIMPLE_MAJORITY_TRIGGER_ALGORITHM_H
#define SIMPLE_MAJORITY_TRIGGER_ALGORITHM_H

#include "icetray/I3Logging.h"
#include "trigger-sim/algorithms/TriggerHit.h"

/**

   The SimpleMajorityTriggerAlgorithm class is a port the DAQ trigger algorithm 
   of the same name. The algorithm has 2 configuration parameters:

   1) TriggerWindow: The length of the sliding time window used to define
                     an interesting time period.

   2) TriggerThreshold: The minimum number of hits that must be present within
                        a time period of TriggerWindow.

   A trigger is satisified if there are at least TriggerThreshold hits in a time
   window of length TriggerWindow.

   The algorithm works in one steps:

   1) Find a time window: A sliding time window is applied to each hit and the 
      number of hits falling in it is counted. As long as the threshold condition
      is meet, the widow will continue to slide. Once the number of hits drops
      below the threshold, a trigger is defined which includes all hits that were
      part of any valid window. A hit cannot be part of more than one active
      time window.

   When a trigger is found, the hits that are included are any that were in the 
   time window. The trigger start will be the earliest hit time in the time window
   and the trigger stop will be the latest hit time in the time window.

   A time window with multiple indepent clusters of hits will only produce a single
   trigger, but all hits in the time window will be used to define the length of the
   trigger.
   
 */

class SimpleMajorityTriggerAlgorithm
{

 public:
  SimpleMajorityTriggerAlgorithm(double triggerWindow, unsigned int triggerThreshold);
  ~SimpleMajorityTriggerAlgorithm();

  void AddHits(TriggerHitVectorPtr hits);

  unsigned int GetNumberOfTriggers();
  TriggerHitVectorPtr GetNextTrigger();

 private:

  double triggerWindow_;
  unsigned int triggerThreshold_;

  TriggerHitVectorVector triggers_;
  unsigned int triggerCount_;
  unsigned int triggerIndex_;

  SET_LOGGER("SimpleMajorityTriggerAlgorithm");
};

#endif
