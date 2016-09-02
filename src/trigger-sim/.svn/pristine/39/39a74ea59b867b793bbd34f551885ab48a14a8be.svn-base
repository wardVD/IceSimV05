/**
 * copyright  (C) 2006
 * the icecube collaboration
 * $Id:
 *
 * @file ClusterTriggerAlgorithm.cxx
 * @version
 * @date
 * @author toale
 */

#include <trigger-sim/algorithms/ClusterTriggerAlgorithm.h>
#include <trigger-sim/algorithms/TimeWindow.h>
#include <boost/foreach.hpp>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

ClusterTriggerAlgorithm::ClusterTriggerAlgorithm(double triggerWindow, unsigned int triggerThreshold,
						 unsigned int coherenceLength) : 
  triggerWindow_(triggerWindow),
  triggerThreshold_(triggerThreshold),
  triggerCount_(0)
{

  coherenceUp_   = (coherenceLength - 1) / 2;
  coherenceDown_ = coherenceLength / 2;

  log_debug("ClusterTriggerAlgorithm configuration:");
  log_debug("  TriggerWindow = %f", triggerWindow_);
  log_debug("  TriggerThreshold = %d", triggerThreshold_);
  log_debug("  CoherenceUp = %d", coherenceUp_);
  log_debug("  CoherenceDown = %d", coherenceDown_);

  hitQueue_.clear();
}

ClusterTriggerAlgorithm::~ClusterTriggerAlgorithm() {}

void ClusterTriggerAlgorithm::AddHits(TriggerHitVectorPtr hits)
{

  log_debug("ClusterTrigger has %zd hits to process", hits->size());

  /*------------------------------------------------------------*
   * Check Trigger condition on this string
   *------------------------------------------------------------*/
  triggers_.clear();
  triggerCount_ = 0;

  hitQueue_.clear();

  // Iterate over all the hits
  TriggerHitVector::const_iterator nextHit;
  for (nextHit = hits->begin(); nextHit != hits->end(); nextHit++) {
    double nextTime = nextHit->time;
    log_debug("  Processing hit at time %f", nextTime);

    // Check for an empty queue
    if (hitQueue_.empty()) {  
      log_debug("Queue is empty, adding new hit");
      hitQueue_.push_back(*nextHit);
      continue;  
    }      

    // Check time window
 //   double startTime = hitQueue_.front().time;
 //   double stopTime = startTime + triggerWindow_;
 //   log_debug("    Current time window = (%f, %f)", startTime, stopTime); // Commented these 3 lines out, no use for them anymore

    // Slide the window until next time is in window
    while ( (nextTime - hitQueue_.front().time)  > triggerWindow_) {  // Changed here to DAQ analogue - Thorsten

      log_debug("    Hit is outside window, checking for trigger...");

      bool timeTrigger = (hitQueue_.size() >= triggerThreshold_);
      log_debug("     TimeTrigger = %s", timeTrigger ? "T" : "F");

      bool posTrigger = PosWindow();

      log_debug("     PosTrigger = %s", posTrigger ? "T" : "F");

      // Check for threshold in the time window and in position window
      if ( timeTrigger && posTrigger ) {

	// We have a trigger
	log_debug("  We have a trigger!");

	// Copy hits in hitQueue into the vector of vectors
	TriggerHitVectorPtr triggerHits(new TriggerHitVector);
	TriggerHitList::const_iterator queueIter;
	for (queueIter = hitQueue_.begin(); queueIter != hitQueue_.end(); queueIter++)
	  triggerHits->push_back(*queueIter);
	triggers_.push_back(*triggerHits);
	triggerCount_++;

	hitQueue_.clear();
	break;

      } else {

	// remove the head
	log_debug("    No trigger, shift the queue");
	if(hitQueue_.size() > 0) //////////////// CHANGE THORSTEN --- Here error occured before
	{
		hitQueue_.pop_front();
	}
	else // if size  is 0 break!!!
	{
		break;
	}
      }

    }

    // Add nextHit to queue
    log_debug("    Hit is in window, adding it to queue");
    hitQueue_.push_back(*nextHit);  

  }

  // After the last hit, check the queue again
  log_debug("    Last hit, checking for trigger...");

  bool timeTrigger = (hitQueue_.size() >= triggerThreshold_);
  log_debug("     TimeTrigger = %s", timeTrigger ? "T" : "F");
      
  bool posTrigger = PosWindow();

  log_debug("     PosTrigger = %s", posTrigger ? "T" : "F");

  // Check for threshold in the time window and in position window
  if ( timeTrigger && posTrigger ) {

    // We have a trigger
    log_debug("  We have a trigger!");
    // Copy hits in hitQueue into the vector of vectors
    TriggerHitVectorPtr triggerHits(new TriggerHitVector);
    TriggerHitList::const_iterator queueIter;
    for (queueIter = hitQueue_.begin(); queueIter != hitQueue_.end(); queueIter++)
      triggerHits->push_back(*queueIter);
    triggers_.push_back(*triggerHits);
    triggerCount_++;
  }
  
}

unsigned int ClusterTriggerAlgorithm::GetNumberOfTriggers() {
  return triggerCount_;
}

TriggerHitVectorPtr ClusterTriggerAlgorithm::GetNextTrigger() {
  TriggerHitVectorPtr hits;
  if (triggerCount_ > 0) {
    triggerCount_--;
    log_debug("Returning trigger window %d", triggerCount_);
    hits = TriggerHitVectorPtr(new TriggerHitVector(triggers_.at(triggerCount_)));
  }
  return hits;
}

bool ClusterTriggerAlgorithm::PosWindow()
{
  log_debug("    Checking position window trigger...");

  bool trigger = false;

  std::map<int, unsigned> coherenceMap;

  // Iterate over the hit queue
  TriggerHitList::iterator centralHit;
  for (centralHit = hitQueue_.begin(); centralHit != hitQueue_.end(); centralHit++) {

    // Get the lower and upper bounds
    unsigned int centralPos = centralHit->pos;
    int centralString = centralHit->string;
    int lower = centralPos - coherenceUp_;
    if (lower < 1) lower = 1;
    int upper = centralPos + coherenceDown_;
    if (upper > 60) upper = 60;

    log_debug("      Central hit at (%d, %d)   Pos window = (%d, %d)", centralString, centralPos, lower, upper);

    // Iterate over the doms in the coherence window
    for (int dom = lower; dom <= upper; dom++) {

      // create the key
      int key = GetHash(centralString, dom);

      // Update counter for this dom
      unsigned counter = 0;
      std::map<int, unsigned>::const_iterator iter = coherenceMap.find(key);
      if (iter != coherenceMap.end()) counter = iter->second;
      counter += 1;
      coherenceMap[key] = counter;

      log_debug("        Dom (%d, %d) now has count %d", centralString, dom, counter);

      if (counter >= triggerThreshold_) trigger = true;
    }
    
  }

  if (!trigger) return false;

  // Now remove sites from the map that are below threshold
  std::map<int, unsigned>::iterator mapIter;
  for (mapIter = coherenceMap.begin(); mapIter != coherenceMap.end();) {
    if (mapIter->second < triggerThreshold_) {
      // must increment iterator before erasing it
      coherenceMap.erase(mapIter++);
    } else {
      ++mapIter;
    }
  }

  log_debug("Pruned coherence map:");
  for (mapIter = coherenceMap.begin(); mapIter != coherenceMap.end(); mapIter++) {
    int string = GetString(mapIter->first);
    int pos = GetPosition(mapIter->first);
    log_debug("  Count(%d,%d) = %d", string, pos, mapIter->second);
  }

  // Now remove hits from the queue that are not at valid sites in the map
  log_debug("Pruning the hit queue:");
  TriggerHitList::iterator hitIter;
  for (hitIter = hitQueue_.begin(); hitIter != hitQueue_.end();) { // CHANGE THORSTEN remove hitIter++ from the loop start to the end

    // check hit
    unsigned int hitPos = hitIter->pos;
    int hitString = hitIter->string;

    log_debug("  Hit at (%d,%d)", hitString, hitPos);

    bool near = false;

    // check the map
    std::map<int, unsigned>::iterator mapIter;
    for (mapIter = coherenceMap.begin(); mapIter != coherenceMap.end(); mapIter++) {
      int string = GetString(mapIter->first);
      int pos = GetPosition(mapIter->first);

      int delta = hitPos - pos;

      bool sameString = (hitString == string);
      bool closePos = false;
      if (delta < 0) 
	closePos = (-delta <= static_cast<int>(coherenceUp_));
      else if (delta > 0)
	closePos = (delta <= static_cast<int>(coherenceDown_));
      else
	closePos = true;
      
      if (sameString && closePos) {
	near = true;
	log_debug("    is near site (%d,%d)", string, pos);
      } else {
	log_debug("    is NOT near site (%d,%d)", string, pos);
      }
    }
    
    if (!near) {
      log_debug("  Hit is not near a site, deleting from queue");
      hitIter = hitQueue_.erase(hitIter);
    } else {
      hitIter++;
      log_debug("  Hit is near a site, keep it");
    }
    
  }
  
  return true;
}

int ClusterTriggerAlgorithm::GetHash(int string, unsigned int position) {
  return string*100 + position;
}

int ClusterTriggerAlgorithm::GetString(int hash) {
  return hash/100;
}

unsigned int ClusterTriggerAlgorithm::GetPosition(int hash) {
  return hash - 100*GetString(hash);
}
