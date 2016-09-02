/**
 * copyright  (C) 2006
 * the icecube collaboration
 * $Id:
 *
 * @file MultiplicityStringTriggerAlgorithm.cxx
 * @version
 * @date
 * @author toale
 */

#include <trigger-sim/algorithms/MultiplicityStringTriggerAlgorithm.h>
#include <trigger-sim/algorithms/TimeWindow.h>
#include <boost/foreach.hpp>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

MultiplicityStringTriggerAlgorithm::MultiplicityStringTriggerAlgorithm(double triggerWindow, unsigned int triggerThreshold,
								       unsigned int maxSpan, unsigned int vetoDepth) : 
  triggerWindow_(triggerWindow),
  triggerThreshold_(triggerThreshold),
  maxSpan_(maxSpan),
  vetoDepth_(vetoDepth),
  triggerCount_(0)
{

  log_debug("MultiplicityStringTriggerAlgorithm configuration:");
  log_debug("  TriggerWindow = %f", triggerWindow_);
  log_debug("  TriggerThreshold = %d", triggerThreshold_);
  log_debug("  MaxSpan = %d", maxSpan_);
  log_debug("  VetoDepth = %d", vetoDepth_);

}

MultiplicityStringTriggerAlgorithm::~MultiplicityStringTriggerAlgorithm() {}

void MultiplicityStringTriggerAlgorithm::AddHits(TriggerHitVectorPtr hits)
{

  log_debug("Adding %zd hits to MultiplicityStringTigger", hits->size());

  /*------------------------------------------------------------*
   * Check Trigger condition on this string
   *------------------------------------------------------------*/
  TimeWindow timeWindow(triggerThreshold_, triggerWindow_);
  triggers_.clear();
  triggerCount_ = 0;

  // Get time windows
  TriggerHitIterPairVectorPtr timeWindows = timeWindow.SlidingTimeWindows(hits);

  // If the vector is empty, there are no time windows for this string
  if (timeWindows->empty()) {
    log_debug("No valid time windows for this string");
    return;
  }
  log_debug("Found %zd triggered time windows", timeWindows->size());

  // Loop over the time windows and check the dom positions in each
  for (TriggerHitIterPairVector::const_iterator timeWindowIter = timeWindows->begin(); 
       timeWindowIter != timeWindows->end(); 
       timeWindowIter++) {

    // Create a vector of the hits in this time window
    TriggerHitVectorPtr timeHits(new TriggerHitVector);

    // Get the window boundaries
    TriggerHitVector::const_iterator firstHit = timeWindowIter->first;
    TriggerHitVector::const_iterator lastHit  = timeWindowIter->second;

    // Iterate over the hits and pull out the ones for this window
    for (TriggerHitVector::const_iterator hitIter = firstHit; 
	 hitIter != lastHit; 
	 hitIter++)
      timeHits->push_back(*hitIter);

    log_debug("Time window (%f, %f) has %zd hits", firstHit->time, (--lastHit)->time, timeHits->size());

    // Check the positions of these hits
    bool posTrigger = PosWindow(timeHits);
	  
    if (posTrigger) {
      triggers_.push_back(*timeHits);
      triggerCount_++;
      log_debug("Trigger! Count = %d", triggerCount_);
    }

  } 
  
}

unsigned int MultiplicityStringTriggerAlgorithm::GetNumberOfTriggers() {
  return triggerCount_;
}

TriggerHitVectorPtr MultiplicityStringTriggerAlgorithm::GetNextTrigger() {
  TriggerHitVectorPtr hits;
  if (triggerCount_ > 0) {
    triggerCount_--;
    log_debug("Returning trigger window %d", triggerCount_);
    hits = TriggerHitVectorPtr(new TriggerHitVector(triggers_.at(triggerCount_)));
  }
  return hits;
}

bool MultiplicityStringTriggerAlgorithm::PosWindow(TriggerHitVectorPtr inputHits)
{
  log_debug("  Position window trigger...");

  /*------------------------------------------------------------*
   * Check Veto condition
   *------------------------------------------------------------*/

  bool veto = false;
  BOOST_FOREACH(const TriggerHit& hit1, *inputHits) {
    unsigned int pos1 = hit1.pos;
    if (pos1 <= vetoDepth_) {
      log_debug("  VETO: Hit in veto region (<= %u): %u", vetoDepth_, pos1);
      veto = true;
    }
  }
  if (veto) return false;

  /*------------------------------------------------------------*
   * Check Topology
   *------------------------------------------------------------*/

  // loop over all hits and use each as the start of the trigger window
  unsigned int count = 0;
  BOOST_FOREACH(const TriggerHit& hit1, *inputHits){
    unsigned int startPos = hit1.pos;
    unsigned int stopPos = startPos + maxSpan_ - 1;
    if (stopPos > 60) stopPos = 60;

    log_debug("    New position window = (%d,%d)", startPos, stopPos);

    // now loop over all other hits and store any that fall in this window
    BOOST_FOREACH(const TriggerHit& hit2, *inputHits){
      unsigned int pos = hit2.pos;
      log_debug("      Checking hit at %d", pos);
      if ( (pos >= startPos) && (pos <= stopPos) ) {
	count++;
	log_debug("        Hit in window, count = %d", count);
      } else {
	log_debug("        Hit not in window");
      }
    }

    // next check if trigger is satisfied for this window
    if (count >= triggerThreshold_) {
      log_debug("  Found a position window over threshold: %d", count);
      return true;
    }

  }

  return false;
}

