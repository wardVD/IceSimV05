#include <I3Test.h>

#include "trigger-sim/algorithms/MultiplicityStringTriggerAlgorithm.h"
#include "trigger-sim/algorithms/TriggerHit.h"

TEST_GROUP(MultiplicityStringTriggerTests);

void SimpleMSTTest() {

  MultiplicityStringTriggerAlgorithm stringTrigger(1500, 3, 7, 0);
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

void MultipleClusterTest() {

  MultiplicityStringTriggerAlgorithm stringTrigger(1500, 3, 7, 0);
  TriggerHitVectorPtr hits(new TriggerHitVector());

  TriggerHit h1(1,1,1);
  TriggerHit h2(2,2,1);
  TriggerHit h3(3,3,1);
  TriggerHit h4(4,58,1);
  TriggerHit h5(5,59,1);
  TriggerHit h6(6,60,1);

  hits->push_back(h1);
  hits->push_back(h2);
  hits->push_back(h3);
  hits->push_back(h4);
  hits->push_back(h5);
  hits->push_back(h6);

  stringTrigger.AddHits(hits);
  unsigned int nTrig = stringTrigger.GetNumberOfTriggers();
  ENSURE(nTrig == 1);

}

TEST(simple_mst_test) {
  SimpleMSTTest();
}

TEST(multiple_cluster_test) {
  MultipleClusterTest();
}


