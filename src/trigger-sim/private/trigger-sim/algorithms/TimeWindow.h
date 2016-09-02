#ifndef TIME_WINDOW_H
#define TIME_WINDOW_H

#include <string>
#include "trigger-sim/algorithms/TriggerHit.h"
#include "icetray/I3Logging.h"

/**
 * @brief A simple timewindow class used by the triggers.
 */

class TimeWindow
{
 public:

  /**
   * Constructor requires a threshold and a time window (in ns)
   */
  TimeWindow(unsigned int threshold, double window);

  ~TimeWindow();

  /**
   * Sliding time windows
   */
  TriggerHitIterPairVectorPtr SlidingTimeWindows(TriggerHitVectorPtr hits);
  TriggerHitIterPairVectorPtr SlidingTimeWindows(TriggerHitVector& hits);

  /**
   * Fixed time windows
   */
  TriggerHitIterPairVectorPtr FixedTimeWindows(TriggerHitVectorPtr hits);

 private:

  /**
   * Default constructor is private until we define defaults
   */
  TimeWindow();

  void DumpHits(TriggerHitList& hits, const std::string& head, const std::string& pad);
  void CopyHits(TriggerHitList& sourceHits, TriggerHitList& destHits);
  bool Overlap(TriggerHitList& hits1, TriggerHitList& hits2);

  unsigned int threshold_;
  double window_;

  TriggerHitListPtr timeWindowHits_;
  TriggerHitListPtr triggerWindowHits_;

  SET_LOGGER("TimeWindow");
};

#endif // TIME_WINDOW_H
