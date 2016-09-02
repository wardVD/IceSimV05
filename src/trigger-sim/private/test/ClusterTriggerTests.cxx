#include <I3Test.h>

#include "trigger-sim/algorithms/ClusterTriggerAlgorithm.h"
#include "trigger-sim/algorithms/TriggerHit.h"
#include <phys-services/I3GSLRandomService.h>

TEST_GROUP(ClusterTriggerTests);

void SimpleCTTest() {

  ClusterTriggerAlgorithm stringTrigger(1500, 3, 7);
  TriggerHitVectorPtr hits(new TriggerHitVector());

  TriggerHit h1(1,1,1);
  TriggerHit h2(2,2,1);
  TriggerHit h3(3,3,1);
  TriggerHit h4(2001,4,1);
  TriggerHit h5(2002,5,1);
  TriggerHit h6(2003,6,1);

  hits->push_back(h1);
  hits->push_back(h2);
  hits->push_back(h3);
  hits->push_back(h4);
  hits->push_back(h5);
  hits->push_back(h6);

  stringTrigger.AddHits(hits);
  unsigned int nTrig = stringTrigger.GetNumberOfTriggers();
  ENSURE(nTrig == 2);

}

void TestPruningHits() {

  ClusterTriggerAlgorithm stringTrigger(1500, 3, 7);
  TriggerHitVectorPtr hits(new TriggerHitVector());

  // 6 hits, all in 1 time window, but two of them are far away
  TriggerHit h1(1,1,2);
  TriggerHit h2(2,2,2);
  TriggerHit h3(3,3,2);
  TriggerHit h4(4,1,1);
  TriggerHit h5(5,2,1);
  TriggerHit h6(6,3,1);
  TriggerHit h7(7,4,3);

  hits->push_back(h1);
  hits->push_back(h2);
  hits->push_back(h3);
  hits->push_back(h4);
  hits->push_back(h5);
  hits->push_back(h6);
  hits->push_back(h7);

  stringTrigger.AddHits(hits);
  unsigned int nTrig = stringTrigger.GetNumberOfTriggers();
  ENSURE(nTrig == 1);

}

void TestNoTrigger() {

  ClusterTriggerAlgorithm stringTrigger(1500, 3, 7);
  TriggerHitVectorPtr hits(new TriggerHitVector());

  // 6 hits, all in 1 time window, but two of them are far away
  TriggerHit h1(1,1,1);
  TriggerHit h2(2001,2,1);
  TriggerHit h3(4001,3,1);
  TriggerHit h4(5001,4,1);
  TriggerHit h5(6001,5,1);
  TriggerHit h6(8001,6,1);


  hits->push_back(h1);
  hits->push_back(h2);
  hits->push_back(h3);
  hits->push_back(h4);
  hits->push_back(h5);
  hits->push_back(h6);

  stringTrigger.AddHits(hits);
  unsigned int nTrig = stringTrigger.GetNumberOfTriggers();
  ENSURE(nTrig == 0);

}

void TestHashing() {

  ClusterTriggerAlgorithm stringTrigger(1500, 3, 7);

  for (int s = 1; s < 100; s++) {
    for (unsigned int p = 1; p <= 60; p++) {
      int hash = stringTrigger.GetHash(s, p);

      int string = stringTrigger.GetString(hash);
      unsigned int position = stringTrigger.GetPosition(hash);

      ENSURE( string == s );
      ENSURE( position == p );

    }
  }


}


void TriggerTest() {

  ClusterTriggerAlgorithm stringTrigger(2500, 5, 7);

  I3GSLRandomService rand(99999); 

  TriggerHitVectorPtr hits(new TriggerHitVector());
  for ( int i(0) ; i < 5; i++){
    double time( rand.Uniform(1,2500) );
    unsigned pos( rand.Uniform(27,34) );
    hits->push_back( TriggerHit( time, pos, 21) );    
  }

  stringTrigger.AddHits(hits);
  unsigned int nTrig = stringTrigger.GetNumberOfTriggers();
  ENSURE(nTrig == 1);
}


TEST(simple_ct_test) {
  SimpleCTTest();
}

TEST(test_pruning_hits) {
  TestPruningHits();
}

TEST(test_no_trigger) {
  TestNoTrigger();
}

TEST(test_hashing) {
  TestHashing();
}


TEST(at_threshold){
  for( int i(0); i < 10000; i++)
    TriggerTest();
}
