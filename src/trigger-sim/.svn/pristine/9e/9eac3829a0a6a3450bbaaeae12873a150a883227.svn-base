#include "trigger-sim/algorithms/TimeWindow.h"
#include <boost/foreach.hpp>

using namespace std;

TimeWindow::TimeWindow(unsigned int threshold, double window) 
  : threshold_(threshold), window_(window) 
{
  timeWindowHits_ = TriggerHitListPtr( new TriggerHitList() );
  triggerWindowHits_ = TriggerHitListPtr( new TriggerHitList() );
}

TimeWindow::~TimeWindow() {}

/**
   Implementation of a sliding time window algorithm.
   Input is a vector of TriggerHit objects that should be time ordered:
      std::vector<TriggerHit>
   Output is a std::vector of pairs comprised of iterators pointing to the begining and end
   of each valid time window:
      std::vector<pair<TriggerHitVector::const_iterator,TriqggerHitVector::const_iterator> >

 */
TriggerHitIterPairVectorPtr TimeWindow::SlidingTimeWindows(TriggerHitVectorPtr hits)
{
  // The return variable is a std::vector of pairs, each pair is the begin/end iterators for the time window
  TriggerHitIterPairVectorPtr triggerWindows(new TriggerHitIterPairVector());

  // Initialize the trigger condition
  bool trigger = false;

  // Keep track of hits in the sliding time window
  timeWindowHits_->clear();

  // Keep track of hits in the current trigger window
  triggerWindowHits_->clear();

  // Create the begin/end pointers
  TriggerHitVector::const_iterator beginHit;
  TriggerHitVector::const_iterator endHit;

  // Outer loop over all hits except the last
  // The iteration of startingHit is controlled by the sliding of the time window
  TriggerHitVector::const_iterator startingHit = hits->begin();
  if(startingHit==hits->end())
  {
    return triggerWindows;
  }
 // while (startingHit != hits->end()) {

  // Define the times of this trigger window
  double startTime = startingHit->time;
  double stopTime  = startTime + window_;
  log_debug("New starting hit! TimeWindow = (%f, %f)", startTime, stopTime);

  // Push the startingHit into the timeWindowHits if its empty
  //if (timeWindowHits_->empty())
  timeWindowHits_->push_back(*startingHit);

  DumpHits(*timeWindowHits_, " TimeWindowHits:", "   ");

  // Initialize the counter (including startingHit)
  unsigned int count = timeWindowHits_->size(); // holds the number of hits within the sliding timewindow
//  unsigned int trigger_hit_count=0;

  // Initialize the begin/end pointers if the trigger is not active
  if (!trigger) {
    beginHit = startingHit;
    endHit = hits->end();
  }

  // Inner loop over all later hits
  TriggerHitVector::const_iterator nextHit;
  for (nextHit = startingHit + 1; nextHit != hits->end(); nextHit++) {

    // The time of the next hit
    double nextTime = nextHit->time;
    log_debug("  NextTime = %f", nextTime);

    // Check if it falls in the time window
    if (nextTime < startTime)
    {
      log_debug("ERROR HITS NOT TIMEORDERED_");
      // error, hits should be time ordered
    } 
    else if (nextTime <= stopTime)
    {
      // in window, increment counter
      timeWindowHits_->push_back(*nextHit);
      count = timeWindowHits_->size();

      if(trigger)
      {
      	triggerWindowHits_->push_back(*nextHit);



      //   trigger_hit_count=triggerWindowHits_->size();
      }

      if(nextHit == (hits->end()-1)) // we are at the last hit, this is in simulation only.... form a trigger if there is one...
      {
      	log_debug("special case;;;;;; we are at the last hit adn still in window");
      	if(trigger || (!trigger && (timeWindowHits_->size() >= threshold_)) )
      	{
	  log_debug("yeah"); 

	  if(!trigger)
	  {
	    CopyHits(*timeWindowHits_, *triggerWindowHits_); 
	  }

      	  double trigger_start=triggerWindowHits_->front().time;
      	  double trigger_end=triggerWindowHits_->back().time;

      	  for(beginHit=hits->begin(); beginHit!=hits->end(); beginHit++)
      	  {
      	    if(beginHit->time == trigger_start)
      	    {
	      log_debug("trigger_start: %f", trigger_start);
      	      break;
      	    }
      	  }
      	  for(endHit=hits->begin(); endHit!=hits->end(); endHit++)
      	  {
      	    if(endHit->time == trigger_end)
      	    {
	      log_debug("trigger_end: %f", trigger_end);
      	      break;
      	    }
      	  }


       //   beginHit=triggerWindowHits_->begin();
	//  endHit=(triggerWindowHits_->end()-1);

	  TriggerHitIterPair triggerWindow(beginHit, (endHit+1));
	  triggerWindows->push_back(triggerWindow);
	  DumpHits(*triggerWindowHits_, "      TriggerWindowHits:", "      ");
	 }

       //  break; // break the whole loop

       }  


      log_debug("    Hit inside window, counter = %d", count);
      DumpHits(*timeWindowHits_, "    TimeWindowHits:", "      ");
    } else {
      // Hit is beyond window, must slide window
      log_debug("    Hit outside window, sliding...");

      // First check if the current window is above threshold
      if (count >= threshold_) { 
	log_debug("      Window is above threshold");
	// First time we are above threshold, so no trigger yet, 
	// then define beginHit for this trigger!!
	if(!trigger){
	 // beginHit =  startingHit;
	  CopyHits(*timeWindowHits_, *triggerWindowHits_); 
      //    trigger_hit_count=triggerWindowHits_->size();
	  DumpHits(*triggerWindowHits_, "      TriggerWindowHits:", "      ");


	  log_debug("StartTime of trigger: %lf",beginHit->time);
	}
	trigger = true;
	//endHit = nextHit;  
      }

      // Copy new hits into the triggerWindowHits

      // Now slide the window
      //  slide until either next hit is inside or count goes to zero
      bool inWindow = true;
      if (nextTime > stopTime)
      {
	 inWindow=false;
      }

      log_debug("nextTime: %f, stopTime: %f", nextTime, stopTime);

      while ((!inWindow) && (count > 1))
      {
	//startingHit++;
        timeWindowHits_->pop_front();
        count = timeWindowHits_->size();
      	log_debug("nextTime: %f, stopTime: %f", nextTime, stopTime);
	// new time window
	startTime = timeWindowHits_->front().time;
	stopTime = startTime + window_;
	log_debug("      New TimeWindow = (%f, %f)  Count = %d", startTime, stopTime, count);
	DumpHits(*timeWindowHits_, "      TimeWindowHits:", "        ");

	if (nextTime <= stopTime) {
	  inWindow = true;
	}
      }

      if(!inWindow) // the prior while loop broke because count was 1
      {
	timeWindowHits_->pop_front();

	timeWindowHits_->push_back(*nextHit);
	startTime = timeWindowHits_->front().time;
	stopTime = startTime + window_;

	count = timeWindowHits_->size();


      }
      else // Hit really falls into the window and count is bigger than 1
      {
	// just push it back
	timeWindowHits_->push_back(*nextHit);
	count = timeWindowHits_->size();
      }

      if(trigger)
      {
	log_debug("am in trigger...");

	DumpHits(*triggerWindowHits_, "  overlap..     TriggerWindowHits:", "        ");

	bool overlap = Overlap(*timeWindowHits_, *triggerWindowHits_);
	if( ((count < threshold_) && (!overlap)) || (count==1 && threshold_== 1) || nextHit == (hits->end()-1))
	{
	  log_debug("form a trigger...");
	  if( nextHit == (hits->end()-1) && (overlap)) // if we overlap we have to take it - simulation/daq issue
	  {
	    triggerWindowHits_->push_back(*nextHit);
	  }
      /*    for(int i = 0; i < (int)triggerWindowHits_->size(); i++)
	  {



	  }*/

	  double trigger_start=triggerWindowHits_->front().time;
	  double trigger_end=triggerWindowHits_->back().time;

	  for(beginHit=hits->begin(); beginHit!=hits->end(); beginHit++)
	  {
	    if(beginHit->time == trigger_start)
	    {
	      break;
	    }
	  }
	  for(endHit=hits->begin(); endHit!=hits->end(); endHit++)
	  {
	    if(endHit->time == trigger_end)
	    {
	      break;
	    }
	  }


       //   beginHit=triggerWindowHits_->begin();
	//  endHit=(triggerWindowHits_->end()-1);

	  TriggerHitIterPair triggerWindow(beginHit, (endHit+1));
	  triggerWindows->push_back(triggerWindow);
	  DumpHits(*triggerWindowHits_, "       TriggerWindowHits:", "        ");
	  trigger=false;
	  triggerWindowHits_->clear();


	}
	else
	{
	  triggerWindowHits_->push_back(*nextHit);

	}

      }
      // Check for overlap
    } // end time window check

  } // end inner loop
  timeWindowHits_->clear();
  log_debug("      Reached end of inner loop...");
  DumpHits(*timeWindowHits_, "       TimeWindowHits:", "        ");

  return triggerWindows;
}

/**
   Implementation of a sliding time window algorithm.
   Input is a std::vector of TriggerHit objects that should be time ordered:
      std::vector<TriggerHit>
   Output is a std::vector of pairs comprised of iterators pointing to the begining and end
   of each valid time window:
      std::vector<pair<TriggerHitVector::const_iterator,TriggerHitVector::const_iterator> >

 */
TriggerHitIterPairVectorPtr TimeWindow::FixedTimeWindows(TriggerHitVectorPtr hits)
{
  // The return variable is a std::vector of pairs, each pair is the begin/end iterators for the time window
  TriggerHitIterPairVectorPtr triggerWindows(new TriggerHitIterPairVector());

  // Initialize the trigger condition
  // bool trigger = false;

  // Create the begin/end pointers
  TriggerHitVector::const_iterator beginHit;
  TriggerHitVector::const_iterator endHit;

  // Outer loop over all hits except the last
  // The iteration of startingHit is controlled by the sliding of the time window
  TriggerHitVector::const_iterator startingHit = hits->begin();
  while (startingHit != (hits->end()-1)) {

    // Define the times of this trigger window
    double startTime = startingHit->time;
    double stopTime  = startTime + window_;
    log_debug("TimeWindow = (%f, %f)", startTime, stopTime);

    // Initialize the counter (including startingHit)
    unsigned int count = 1;

    // Initialize the begin/end pointers
    beginHit = startingHit;
    endHit = hits->end();

    // Inner loop over all later hits
    TriggerHitVector::const_iterator nextHit;
    for (nextHit = startingHit + 1; nextHit != hits->end(); nextHit++) {

      // The time of the next hit
      double nextTime = nextHit->time;
      log_debug("  NextTime = %f", nextTime);

      // Check if it falls in the time window
      if (nextTime < startTime) {
	// error, hits should be time ordered
      } else if (nextTime <= stopTime) {
	// in window, increment counter
	count++;
	log_debug("    Hit inside window, counter = %d", count);
      } else {
	// Hit is beyond window, check for trigger
	log_debug("    Hit outside window...");

	if (count >= threshold_) {
	  log_debug("      but we have a trigger, save pointers");
	  endHit = nextHit;

	  // Save the pointers
	  TriggerHitIterPair triggerWindow(beginHit, endHit);
	  triggerWindows->push_back(triggerWindow);

	  // Set startingHit to endHit
	  startingHit = endHit;
	} else {
	  // Increment startingHit by 1
	  startingHit++;
	}

      } // end time window check

    } // end inner loop

  } // end outer loop

  return triggerWindows;
}

TriggerHitIterPairVectorPtr TimeWindow::SlidingTimeWindows(TriggerHitVector& hits)
{

  TriggerHitVectorPtr hitsPtr(new TriggerHitVector(hits));
  return SlidingTimeWindows(hitsPtr);

}

void TimeWindow::DumpHits(TriggerHitList& hits, const string& head, const string& pad)
{

  log_debug("%s", head.c_str());
  int n = 0;
  BOOST_FOREACH( const TriggerHit& hit, hits ) {
    log_debug("%sHit %d @ Time %f", pad.c_str(), n, hit.time);
    n++;
  }

}

void TimeWindow::CopyHits(TriggerHitList& sourceHits, TriggerHitList& destHits)
{

  // Copy from source to dest IFF source does not already exist in dest
  BOOST_FOREACH( TriggerHit sourceHit, sourceHits ) {
    if (find(destHits.begin(), destHits.end(), sourceHit) == destHits.end())
      destHits.push_back(sourceHit);
  }

}

bool TimeWindow::Overlap(TriggerHitList& hits1, TriggerHitList& hits2)
{
  bool overlap = false;
  BOOST_FOREACH( TriggerHit hit1, hits1 ) {
    if (find(hits2.begin(), hits2.end(), hit1) != hits2.end()) {
      overlap = true;
      break;
    }
  }
  return overlap;
}
