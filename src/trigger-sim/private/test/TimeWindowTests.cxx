#include <I3Test.h>
#include <iostream>
#include "trigger-sim/algorithms/TimeWindow.h"
#include "trigger-sim/algorithms/TriggerHit.h"
#include "phys-services/I3GSLRandomService.h"
#include "icetray/I3Units.h"

using namespace std;

// Try 20 thresholds from 3 to 53
const int MIN_SMT_COND(3);
const int MAX_SMT_COND(20);

// Try 10 time windows from 1000 ns to 11000 ns
const int MIN_TIME_WINDOW(1000);
const int MAX_TIME_WINDOW(11000);
const int TIME_STEP(1000);

//for each setting run it 10 times
const int NTRIALS(10);

TEST_GROUP(TimeWindowTests);

/**
 * Test conditions in which possibly multiple, separate triggers can occur.
 *   nTriggers can be anything > 0
 */
namespace TimeWindowTests{
void MultipleTest(unsigned int nTriggers) {

  cerr << endl;
  cerr << "MultipleTest (with nTriggers=" << nTriggers << ") details..." << endl;

  I3GSLRandomService rand(99999); 
  TriggerHitVectorPtr hits(new TriggerHitVector());

  for(int timeWindow(MIN_TIME_WINDOW); 
      timeWindow < MAX_TIME_WINDOW ;  
      timeWindow += TIME_STEP){
    for(int multCond(MIN_SMT_COND); multCond <  MAX_SMT_COND; multCond++){

      cerr << "--------------------"         << endl;
      cerr << "TimeWindow Configuration:"    << endl;
      cerr << " timeWindow = " << timeWindow << endl;
      cerr << " multCond = "   << multCond   << endl;
  
      TimeWindow timeWindowClass(multCond, timeWindow);

      // For each of these configurations, do 50 trials => Total of 10,000
      for(int nTrials(0); nTrials < NTRIALS; nTrials++){ 

        // Create nTriggers clusters of well separated hits
	if (!hits->empty()) hits->clear();
        for(unsigned int n(0); n < nTriggers ; n++){
          double windowShift(2*n * (timeWindow + 1*I3Units::nanosecond));
          
          for(int nHits(0); nHits < multCond; nHits++){
            int pos(rand.Uniform(1,100));
            int string(rand.Uniform(1,100));
            double time(rand.Uniform(0,timeWindow));

            hits->push_back(TriggerHit(time + windowShift,pos,string));
          }
        }
        sort(hits->begin(),hits->end());

	cerr << "***" << endl;
        cerr << " Trial " << nTrials << " Hits: ";
        for(TriggerHitVector::const_iterator i = hits->begin();
            i != hits->end(); i++)
          cerr << i->time << " ";
        cerr << endl;

        TriggerHitIterPairVectorPtr timeWindows = timeWindowClass.SlidingTimeWindows(hits);
        unsigned int nWindows = timeWindows->size();

	// Ensure that the correct number of windows were produced
        ENSURE(nWindows == nTriggers);

        cerr << " TimeWindows: " << nWindows << endl;
        int n=0;
        for (TriggerHitIterPairVector::const_iterator i = timeWindows->begin();
             i != timeWindows->end(); i++) {

          TriggerHitVector::const_iterator firstHit = i->first;
          TriggerHitVector::const_iterator lastHit  = i->second;

          double startTime = firstHit->time;
          double endTime = (--lastHit)->time;
          cerr << " Window " << n << ": " << startTime << " to " << endTime << endl;

	  // Ensure that the start and end times match the correct hit times
	  int ii = n*multCond;
          ENSURE(startTime == hits->at(ii).time);
          ENSURE(endTime  == hits->at(ii+multCond-1).time);

          n++;
        }

      }
    }
  }

}

/**
 * Test to ensure late stragglers do not extend a single trigger.
 */
void StragglerTest() {

  cerr << endl;
  cerr << "StragglerTest details..." << endl;

  I3GSLRandomService rand(99999); 

  for(int timeWindow(MIN_TIME_WINDOW); 
      timeWindow < MAX_TIME_WINDOW ;  
      timeWindow += TIME_STEP){
    for(int multCond(MIN_SMT_COND); multCond <  MAX_SMT_COND; multCond++){

      cerr << "--------------------"         << endl;
      cerr << "TimeWindow Configuration:"    << endl;
      cerr << " timeWindow = " << timeWindow << endl;
      cerr << " multCond = "   << multCond   << endl;
  
      TimeWindow timeWindowClass(multCond, timeWindow);

      // For each of these configurations, do 10 trials => Total of 2,000
      for(int nTrials(0); nTrials < NTRIALS; nTrials++){ 

        // Create M-1 hits clustered near t=0
        TriggerHitVectorPtr hits(new TriggerHitVector());
        for(int nHits(0); nHits < multCond-1; nHits++){
          int pos(rand.Uniform(1,100));
          int string(rand.Uniform(1,100));
          double time(rand.Uniform(0,1*I3Units::nanosecond));
        
          hits->push_back(TriggerHit(time,pos,string));
        }

        //these are the stragglers
        for(int n(1); n<50; n++){
          int pos(rand.Uniform(1,100));
          int string(rand.Uniform(1,100));

          //just inside the time window
          //the very first straggler contributes to the trigger
          //the rest should not
          double time(n*0.999*timeWindow);
        
          hits->push_back(TriggerHit(time,pos,string));
        }
        sort(hits->begin(),hits->end());

	cerr << "***" << endl;
        cerr << " Trial " << nTrials << " Hits: ";
        for(TriggerHitVector::const_iterator i = hits->begin();
            i != hits->end(); i++)
          cerr << i->time << " ";
        cerr << endl;

        TriggerHitIterPairVectorPtr timeWindows = timeWindowClass.SlidingTimeWindows(hits);
        unsigned int nWindows = timeWindows->size();

	// Ensure that only 1 window was produced
        ENSURE(nWindows == 1);

        cerr << " TimeWindows: " << nWindows << endl;
        int n=0;
        for (TriggerHitIterPairVector::const_iterator i = timeWindows->begin();
             i != timeWindows->end(); i++) {

          TriggerHitVector::const_iterator firstHit = i->first;
          TriggerHitVector::const_iterator lastHit  = i->second;

          double startTime = firstHit->time;
          double endTime = (--lastHit)->time;
          cerr << " Window " << n << ": " << startTime << " to " << endTime << endl;

	  // Ensure that the start and end times match the correct hit times
	  // In this version of the straggler test the event length is the full
	  // span of hits.  The time window is extended until the time difference
	  // between consecutive hits is outside the time window
          ENSURE(startTime == hits->front().time);
          ENSURE(endTime  == hits->back().time);

          n++;
        }

      }
    }
  }
}

/**
 * Test to ensure that single hits connecting clusters cause the clusters to merge.
 */
void ConnectorTest(){ 

  cerr << endl;
  cerr << "ConnectorTest details..." << endl;

  I3GSLRandomService rand(99999); 

  const int NCLUSTERS(5);

  for(int timeWindow(MIN_TIME_WINDOW); 
      timeWindow < MAX_TIME_WINDOW ;  
      timeWindow += TIME_STEP){
    for(int multCond(MIN_SMT_COND); multCond <  MAX_SMT_COND; multCond++){

      cerr << "--------------------"         << endl;
      cerr << "TimeWindow Configuration:"    << endl;
      cerr << " timeWindow = " << timeWindow << endl;
      cerr << " multCond = "   << multCond   << endl;
  
      TimeWindow timeWindowClass(multCond, timeWindow);

      // For each of these configurations, do 25 trials => Total of 10,000
      for(int nTrials(0); nTrials < NTRIALS; nTrials++){ 

	// Create NCLUSTERS clusters of hits, each with multCond hits within the timeWindow
        TriggerHitVectorPtr hits(new TriggerHitVector());
        for(int n(0); n<NCLUSTERS; n++){
          double clusterTime(2*n*0.99*timeWindow);
          for(int nHits(0); nHits < multCond -1; nHits++){
            int pos(rand.Uniform(1,100));
            int string(rand.Uniform(1,100));
            double time(rand.Uniform(0,1*I3Units::nanosecond) + clusterTime);
            
            hits->push_back(TriggerHit(time,pos,string));
          }

	  // This is the connector hit
          int pos(rand.Uniform(1,100));
          int string(rand.Uniform(1,100));
          double connectorTime((2*n+1)*0.99*timeWindow);                
          hits->push_back(TriggerHit(connectorTime,pos,string));
        }
        sort(hits->begin(),hits->end());

        cerr << "***" << endl;
        cerr << " Trial " << nTrials << " Hits: ";
        for(TriggerHitVector::const_iterator i = hits->begin();
            i != hits->end(); i++)
          cerr << i->time << " ";
        cerr << endl;

        TriggerHitIterPairVectorPtr timeWindows = timeWindowClass.SlidingTimeWindows(hits);
        unsigned int nWindows = timeWindows->size();

	// Ensure that there is just 1 window
	ENSURE(nWindows == 1);

        cerr << " TimeWindows: " << nWindows << endl;
        int n=0;
        for (TriggerHitIterPairVector::const_iterator i = timeWindows->begin();
             i != timeWindows->end(); i++) {

          TriggerHitVector::const_iterator firstHit = i->first;
          TriggerHitVector::const_iterator lastHit  = i->second;

          double startTime = firstHit->time;
          double endTime = (--lastHit)->time;
          cerr << " Window " << n << ": " << startTime << " to " << endTime << endl;

          // Ensure that the start and end times match the correct hit times
          ENSURE(startTime == hits->at(0).time);
          ENSURE(endTime  == hits->at(hits->size()-1).time);

          n++;
        }

      }
    }
  }
}

void TestThreshold(unsigned int threshold) {

  TimeWindow timeWindowClass(threshold, 15);
  TriggerHitVectorPtr hits(new TriggerHitVector());

  TriggerHit h1(1,1,1);
  TriggerHit h2(2,2,1);
  TriggerHit h3(3,3,1);
  TriggerHit h4(4,4,1);
  TriggerHit h5(5,5,1);
  TriggerHit h6(6,6,1);
  TriggerHit h7(7,7,1);
  TriggerHit h8(8,8,1);
  TriggerHit h9(9,9,1);
  TriggerHit h10(10,10,1);

  hits->push_back(h1);
  hits->push_back(h2);
  hits->push_back(h3);
  hits->push_back(h4);
  hits->push_back(h5);
  hits->push_back(h6);
  hits->push_back(h7);
  hits->push_back(h8);
  hits->push_back(h9);
  hits->push_back(h10);

  TriggerHitIterPairVectorPtr timeWindows = timeWindowClass.SlidingTimeWindows(hits);

  bool trigger = (timeWindows->size() != 0);
  bool pass = (threshold <= 10);
  ENSURE(trigger == pass);

  if (trigger) {
    TriggerHitIterPair window = timeWindows->at(0);
    TriggerHitVector::const_iterator windowStart = window.first;
    TriggerHitVector::const_iterator windowStop  = window.second;
    ENSURE(windowStart->time == h1.time);
    ENSURE((--windowStop)->time  == h10.time);
  }

}// namespace TimeWindowTests

}

TEST(simple_test) {
  TimeWindowTests::MultipleTest(1);
}

TEST(multiple_test) {
  for(int nTrigger(2); nTrigger < 7; nTrigger++)
    TimeWindowTests::MultipleTest(nTrigger);
}

TEST(straggler_test){
  TimeWindowTests::StragglerTest();
}

TEST(connector_test){
  TimeWindowTests::ConnectorTest();
}

TEST(theshold_high) {
  TimeWindowTests::TestThreshold(11);
}

TEST(theshold_equal) {
  TimeWindowTests::TestThreshold(10);
}

TEST(theshold_low) {
  TimeWindowTests::TestThreshold(9);
}
