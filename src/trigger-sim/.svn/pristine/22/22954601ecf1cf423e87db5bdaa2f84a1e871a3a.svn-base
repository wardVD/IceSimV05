#include <I3Test.h>
#include <iostream>
#include <trigger-sim/algorithms/SimpleMajorityTriggerAlgorithm.h>
#include "phys-services/I3GSLRandomService.h"
#include "trigger-sim/algorithms/TriggerHit.h"
#include "icetray/I3Units.h"

TEST_GROUP(SimpleMajorityTriggerTests);

const unsigned int MIN_SMT_COND(3);
const unsigned int MAX_SMT_COND(20);
const unsigned int MIN_TIME_WINDOW(100);
const unsigned int MAX_TIME_WINDOW(10000);
const unsigned int TIME_STEP(500);
const unsigned int NTRIALS(10);
//const unsigned int MIN_SMT_COND(3);
//const unsigned int MAX_SMT_COND(4);
//const unsigned int MIN_TIME_WINDOW(100);
//const unsigned int MAX_TIME_WINDOW(10000);
//const unsigned int TIME_STEP(10000);
//const unsigned int NTRIALS(1);

using namespace std;

void MultipleTest(unsigned int nTriggers) {

  cerr << endl;
  cerr << "MultipleTest (with nTriggers=" << nTriggers << ") details..." << endl;

  I3GSLRandomService rand(99999); 

  for(unsigned int timeWindow(MIN_TIME_WINDOW); 
      timeWindow < MAX_TIME_WINDOW ;  
      timeWindow += TIME_STEP){
    for(unsigned int multCond(MIN_SMT_COND); multCond <  MAX_SMT_COND; multCond++){

      cerr << "--------------------"         << endl;
      cerr << "Trigger Configuration:"       << endl;
      cerr << " timeWindow = " << timeWindow << endl;
      cerr << " multCond = "   << multCond   << endl;
  
      SimpleMajorityTriggerAlgorithm smTrigger(timeWindow,multCond);

      for(unsigned int nTrials(0); nTrials < NTRIALS; nTrials++){ 
	TriggerHitVectorPtr hits(new TriggerHitVector());

	for(unsigned int n(0); n < nTriggers ; n++){
	  double windowShift(2*n * (timeWindow + 1*I3Units::nanosecond));
	  
	  for(unsigned int nHits(0); nHits < multCond; nHits++){
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

	//SimpleMajorityTriggerAlgorithm::AddHits clears the hit vector
	//and the trigger count so, we only need one instantiation of the
	//trigger algoritmhm for each cycle.
	smTrigger.AddHits(hits);
	
	unsigned int nTrig = smTrigger.GetNumberOfTriggers();
	ENSURE(nTrig == nTriggers);

	cerr << " Triggers: " << nTrig << endl;
	for (unsigned int i=0; i<nTrig; i++) {
	  TriggerHitVectorPtr triggerHits = smTrigger.GetNextTrigger();
	  cerr << " Hits for Trigger " << i << ": ";
	  for (TriggerHitVector::const_iterator j = triggerHits->begin();
	       j != triggerHits->end(); j++) {
	    cerr << j->time << " ";
	  }
	  cerr << endl;
	}

      }
    }
  }

}

//Add stragglers to the end of an event and make
//sure the time window isn't extended
void StragglerTest() {

  cerr << endl;
  cerr << "StragglerTest details..." << endl;

  I3GSLRandomService rand(99999); 

  for(unsigned int timeWindow(MIN_TIME_WINDOW); 
      timeWindow < MAX_TIME_WINDOW ;  
      timeWindow += TIME_STEP){
    for(unsigned int multCond(MIN_SMT_COND); multCond <  MAX_SMT_COND; multCond++){

      cerr << "--------------------"         << endl;
      cerr << "Trigger Configuration:"       << endl;
      cerr << " timeWindow = " << timeWindow << endl;
      cerr << " multCond = "   << multCond   << endl;
  
      SimpleMajorityTriggerAlgorithm smTrigger(timeWindow,multCond);

      for(unsigned int nTrials(0); nTrials < NTRIALS; nTrials++){ 
	TriggerHitVectorPtr hits(new TriggerHitVector());

	for(unsigned int nHits(0); nHits < multCond -1; nHits++){
	  int pos(rand.Uniform(1,100));
	  int string(rand.Uniform(1,100));
	  double time(rand.Uniform(0,1*I3Units::nanosecond));
	
	  hits->push_back(TriggerHit(time,pos,string));
	}

	//these are the stragglers
	for(int n(1); n<100; n++){
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

	//SimpleMajorityTriggerAlgorithm::AddHits clears the hit vector
	//and the trigger count so, we only need one instantiation of the
	//trigger algoritmhm for each cycle.
	smTrigger.AddHits(hits);

	unsigned int nTrig = smTrigger.GetNumberOfTriggers();
	ENSURE(nTrig == 1);
	
	cerr << " Triggers: " << nTrig << endl;
	for (unsigned int i=0; i<nTrig; i++) {
	  TriggerHitVectorPtr triggerHits = smTrigger.GetNextTrigger();
	  cerr << " Hits for Trigger " << i << ": ";
	  for (TriggerHitVector::const_iterator j = triggerHits->begin();
	       j != triggerHits->end(); j++) {
	    cerr << j->time << " ";
	  }
	  cerr << endl;

	  // Ensure that the trigger has the correct number of hits
	  // All of the hits are included in the trigger
          ENSURE(triggerHits->size() == hits->size());

	}

      }
    }
  }
}

//Add a cluster of THRESH-1 hits followed by a single "connector"
//hit just inside the time window, then another cluster, and so on.
//This should create one really long event.
void ConnectorTest(){ 

  cerr << endl;
  cerr << "ConnectorTest details..." << endl;

  I3GSLRandomService rand(99999); 

  const unsigned int NCLUSTERS(5);

  for(unsigned int timeWindow(MIN_TIME_WINDOW); 
      timeWindow < MAX_TIME_WINDOW ;  
      timeWindow += TIME_STEP){
    for(unsigned int multCond(MIN_SMT_COND); multCond <  MAX_SMT_COND; multCond++){
  
      cerr << "--------------------"         << endl;
      cerr << "Trigger Configuration:"       << endl;
      cerr << " timeWindow = " << timeWindow << endl;
      cerr << " multCond = "   << multCond   << endl;

      SimpleMajorityTriggerAlgorithm smTrigger(timeWindow,multCond);

      for(unsigned int nTrials(0); nTrials < NTRIALS; nTrials++){ 
	TriggerHitVectorPtr hits(new TriggerHitVector());

	for(unsigned int n(0); n<NCLUSTERS; n++){
	  double clusterTime(2*n*0.99*timeWindow);
	  for(unsigned int nHits(0); nHits < multCond -1; nHits++){
	    int pos(rand.Uniform(1,100));
	    int string(rand.Uniform(1,100));
	    double time(rand.Uniform(0,1*I3Units::nanosecond) + clusterTime);
	    
	    hits->push_back(TriggerHit(time,pos,string));
	  }

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

	//SimpleMajorityTriggerAlgorithm::AddHits clears the hit vector
	//and the trigger count so, we only need one instantiation of the
	//trigger algoritmhm for each cycle.
	smTrigger.AddHits(hits);

	unsigned int nTrig = smTrigger.GetNumberOfTriggers();
	ENSURE(nTrig == 1);


	
	TriggerHitVectorPtr timeHits = smTrigger.GetNextTrigger();
	double startTime(timeHits->front().time);
	double stopTime(timeHits->back().time);
	double triggerLength(stopTime - startTime);
	cerr<<"triggerLength = "<<triggerLength<<endl;
	ENSURE( triggerLength > timeWindow);
	ENSURE( triggerLength < 2*NCLUSTERS*0.999*timeWindow);

      }
    }
  }
}


TEST(simple_test) {
  MultipleTest(1);
}

TEST(multiple_test) {
  for(int nTrigger(2); nTrigger < 10; nTrigger++)
    MultipleTest(nTrigger);
}

TEST(straggler_test){
  StragglerTest();
}

TEST(connector_test){
  ConnectorTest();
}
