#include <I3Test.h>

#include "icetray/I3Tray.h"
#include "icetray/test/ConstructorTest.h"

#include "trigger-sim/modules/ClusterTrigger.h"
#include "trigger-sim/modules/SimpleMajorityTrigger.h"
#include "trigger-sim/modules/I3GlobalTriggerSim.h"
#include "trigger-sim/modules/I3Pruner.h"
#include "trigger-sim/modules/SlowMonopoleTrigger.h"
#include "trigger-sim/modules/CylinderTrigger.h"
#include "trigger-sim/modules/MultiplicityStringTrigger.h"

TEST_GROUP(IceCubeTests);

#define TEST_CC(MODULE)                \
  TEST(clean_construction_##MODULE){   \
    clean_constructor_test<MODULE>();} \

TEST_CC(ClusterTrigger);
TEST_CC(SimpleMajorityTrigger);
TEST_CC(I3GlobalTriggerSim);
TEST_CC(I3Pruner);
TEST_CC(SlowMonopoleTrigger);
TEST_CC(CylinderTrigger);
TEST_CC(MultiplicityStringTrigger);
