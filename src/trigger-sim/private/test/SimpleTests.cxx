#include <I3Test.h>

#include <icetray/I3Units.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <trigger-sim/utilities/GTSUtils.h>
#include "phys-services/I3GSLRandomService.h"

#include <boost/foreach.hpp>

TEST_GROUP(SimpleTests);

TEST(test_less_than){

  I3Trigger t1;
  t1.SetTriggerTime(1.);

  I3Trigger t2;
  t2.SetTriggerTime(2.);

  ENSURE( GTSUtils::LessThan(t1,t2) );

}


TEST(test_subdetector){

  TriggerKey iiKey(TriggerKey::IN_ICE, TriggerKey::UNKNOWN_TYPE);
  TriggerKey itKey(TriggerKey::ICE_TOP, TriggerKey::UNKNOWN_TYPE);
  TriggerKey uKey;

  ENSURE( GTSUtils::KeyToSubDetector(iiKey) == I3TriggerStatus::INICE );
  ENSURE( GTSUtils::KeyToSubDetector(itKey) == I3TriggerStatus::ICETOP );
  ENSURE( GTSUtils::KeyToSubDetector(uKey) == I3TriggerStatus::NOT_SPECIFIED );

}
