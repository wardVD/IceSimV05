/**
 * copyright  (C) 2010
 * the icecube collaboration
 * $Id:
 *
 * @file CylinderTriggerAlgorithm.cxx
 * @version
 * @date
 * @author danninger
 */

#include <trigger-sim/algorithms/CylinderTriggerAlgorithm.h>
#include <boost/foreach.hpp>

CylinderTriggerAlgorithm::CylinderTriggerAlgorithm(double triggerWindow, unsigned int triggerThreshold, unsigned int simpleMultiplicity,
					       I3GeometryConstPtr Geometry, double Radius , double Zdistance) : 
  triggerWindow_(triggerWindow),
  triggerThreshold_(triggerThreshold),
  simpleMultiplicity_(simpleMultiplicity),
  Radius_(Radius),
  Zdistance_(Zdistance),
  triggerCount_(0)
{

  log_debug("CylinderTriggerAlgorithm configuration:");
  log_debug("  TriggerWindow = %f", triggerWindow_);
  log_debug("  TriggerThreshold = %d", triggerThreshold_);
  log_debug("  Radius = %g", Radius_);
  log_debug("  ZDistanze = %g", Zdistance_);

  hitQueue_.clear();
  Geometry_ = Geometry;
}

CylinderTriggerAlgorithm::~CylinderTriggerAlgorithm() {}

void CylinderTriggerAlgorithm::AddHits(TriggerHitVectorPtr hits)
{

  log_debug("CylinderTrigger has %zd hits to process", hits->size());

  /*------------------------------------------------------------*
   * Check Trigger condition
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
    double startTime = hitQueue_.front().time;
    double stopTime = startTime + triggerWindow_;
    log_debug("    Current time window = (%f, %f)", startTime, stopTime);

    // Slide the window until next time is in window
    while ((nextTime - hitQueue_.front().time)  > triggerWindow_) {

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
	
	if(hitQueue_.size() > 0)
	{
	
	  TriggerHitVectorPtr triggerHits(new TriggerHitVector);
	  TriggerHitList::const_iterator queueIter;
	  for (queueIter = hitQueue_.begin(); queueIter != hitQueue_.end(); queueIter++)
	    triggerHits->push_back(*queueIter);
	  triggers_.push_back(*triggerHits);
	  triggerCount_++;
      	}
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

unsigned int CylinderTriggerAlgorithm::GetNumberOfTriggers() {
  return triggerCount_;
}

TriggerHitVectorPtr CylinderTriggerAlgorithm::GetNextTrigger() {
  TriggerHitVectorPtr hits;
  if (triggerCount_ > 0) {
    triggerCount_--;
    log_debug("Returning trigger window %d", triggerCount_);
    hits = TriggerHitVectorPtr(new TriggerHitVector(triggers_.at(triggerCount_)));
  }
  return hits;
}

bool CylinderTriggerAlgorithm::PosWindow()
{
  log_debug("    Checking position window trigger...");

  double x1; 
  double y1; 
  double z1;
  double x2; 
  double y2; 
  double z2;
  double dx;
  double dy;
  double dz;
  double dx2;
  double dy2;
  double dr2;
  double dr;
  
  I3OMGeoMap::const_iterator geo_iter; 
  TriggerHitList tempHits;

  if(hitQueue_.size() >= simpleMultiplicity_)
  {
    return true;
  }
  TriggerHitList::iterator hit1;
  for (hit1 = hitQueue_.begin(); hit1 != hitQueue_.end(); hit1++) {

    // the hit string and position
    int hitString = hit1->string;
    unsigned int hitPos = hit1->pos;
    log_debug("      Central hit at (%d, %d) - time: %f", hitString, hitPos, hit1->time);
    
    OMKey omkey(hitString, hitPos);

    /// find position ///
    geo_iter = Geometry_->omgeo.find(omkey);
    if(geo_iter == Geometry_->omgeo.end()){
      log_fatal("  Warning, OMKey not part of geometry"); // trigger algorithm  does not work when the geometry entry is not there
      continue;
    }
    x1 = geo_iter->second.position.GetX();
    y1 = geo_iter->second.position.GetY();
    z1 = geo_iter->second.position.GetZ();
   
    tempHits.clear();
    tempHits.push_back(*hit1);

    TriggerHitList::iterator hit2;
    for (hit2 = hitQueue_.begin(); hit2 != hitQueue_.end(); hit2++) {
      if (hit2 == hit1) continue;

      // see if this hit is in the volume
      unsigned int thisOM = hit2->pos;
      int thisString = hit2->string;
      OMKey thisKey(thisString, thisOM);
      
        /// find position of other DOMs///
      geo_iter = Geometry_->omgeo.find(thisKey);
      if(geo_iter == Geometry_->omgeo.end()){
	log_fatal("  Warning, OMKey not part of geometry");
	
	continue;
      }
      x2 = geo_iter->second.position.GetX();
      y2 = geo_iter->second.position.GetY();
      z2 = geo_iter->second.position.GetZ();
      
      dx = (x2-x1);
      dy = (y2-y1);
      dz = fabs(z2-z1); 
     
      dx2 = dx*dx;
      dy2 = dy*dy;
      
      dr2 = dx2+dy2;
      dr = sqrt(dr2);
      
      if (dr < Radius_ && dz < (0.5*Zdistance_) ) {
	tempHits.push_back(*hit2);
 	log_debug("      Hit at ( R = %f, Z = %f) OM(%d, %d) is in the volume - t: %f", dr, dz, thisString, thisOM, hit2->time);
      } else {
 	log_debug("      Hit at ( R = %f, Z = %f) OM(%d, %d) is not in the volume - t: %f", dr, dz, thisString, thisOM, hit2->time);
      }
    }
    log_debug("    There are %zd hits in this volume", tempHits.size());
   
    // how many hits do we have?
    if (tempHits.size() >= triggerThreshold_)
    {
      log_debug("    Found a volume over threshold (%d)", triggerThreshold_);
      
      tempHits.sort();
      hitQueue_.clear();
      for (TriggerHitList::iterator iter = tempHits.begin(); iter != tempHits.end(); iter++) 
      {
      	hitQueue_.push_back(*iter);
      }
      
      return true;  
    }

  }

  return false;
}
