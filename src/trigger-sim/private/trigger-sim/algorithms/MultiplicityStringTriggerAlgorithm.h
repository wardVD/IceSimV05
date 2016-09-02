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

#ifndef MULTIPLICITY_STRING_TRIGGER_ALGORITHM_H
#define MULTIPLICITY_STRING_TRIGGER_ALGORITHM_H

#include "icetray/I3Logging.h"
#include "trigger-sim/algorithms/TriggerHit.h"

/**

   The MultiplicityStringTrigger class is a port the DAQ trigger algorithm 
   of the same name. The algorithm has 4 configuration parameters:

   1) TriggerWindow: The length of the sliding time window used to define
                     an interesting time period.

   2) TriggerThreshold: The minimum number of hits that must be present within
                        a time period of TriggerWindow.

   3) MaxSpan: The largest separation of hit DOMs on the string.

   4) VetoDepth: Lower edge of veto region. Any DOM with a position less than
                 or equal to VetoDepth causes the time window to be vetoed.

   A trigger is satisified if there are at least TriggerThreshold hits in a time
   window of length TriggerWindow AND there is at least one span of DOMS MaxSpan
   long with at least TriggerThreshold hits in it.

   The algorithm works in two steps:

   1) Find a time window: A sliding time window is applied to each hit and the 
      number of hits falling in it is counted. As long as the threshold condition
      is meet, the widow will continue to slide. Once the number of hits drops
      below the threshold, a trigger is defined which includes all hits that were
      part of any valid window. A hit cannot be part of more than one active
      time window.

   2) Check the positions: For every triggered time window, a check on positions
      is performed. First the veto is checked. If there are any DOMs in the window
      with a position that is less than or equal to the veto depth, the whole time
      window is vetoed. Otherwise, an outer loop iterates over all hits (they should
      be time ordered) and uses each as the start of a position window (P_start). 
      To this starting position, the maximum span is added to define a stopping 
      position (P_stop = P_start + maxSpan - 1). An inner loop than iterates
      over all hits (including the current outer loop hit) and checks if its position
      falls between the starting and stopping positions. If it does, a counter is
      incremented. After the inner loop completes, the value of the counter is
      compared to the threshold, and if its at least equal to it, a trigger is formed.
      As soon as a trigger is found, the outer loop ends.


   When a trigger is found, the hits that are included are any that were in the 
   time window. The trigger start will be the earliest hit time in the time window
   and the trigger stop will be the latest hit time in the time window.

   A time window will multiple indepent clusters of hits will only produce a single
   trigger, but all hits in the time window will be used to define the length of the
   trigger.
   
 */

class MultiplicityStringTriggerAlgorithm
{

 public:
  MultiplicityStringTriggerAlgorithm(double triggerWindow, unsigned int triggerThreshold,
				     unsigned int maxSpan, unsigned int vetoDepth);
  ~MultiplicityStringTriggerAlgorithm();

  void AddHits(TriggerHitVectorPtr hits);

  unsigned int GetNumberOfTriggers();
  TriggerHitVectorPtr GetNextTrigger();

 private:

  double triggerWindow_;
  unsigned int triggerThreshold_;
  unsigned int maxSpan_;
  unsigned int vetoDepth_;

  TriggerHitVectorVector triggers_;
  unsigned int triggerCount_;

  bool PosWindow(TriggerHitVectorPtr timeHits);

  SET_LOGGER("MultiplicityStringTriggerAlgorithm");
};

#endif
