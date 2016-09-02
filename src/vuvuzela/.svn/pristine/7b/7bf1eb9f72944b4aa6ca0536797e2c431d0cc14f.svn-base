#include <I3Test.h>

#include "vuvuzela/VuvuzelaFunctions.h"

TEST_GROUP(CompareHits);

TEST(CompareHits){

  I3MCPE hitA;
  hitA.time = 1;
  I3MCPE hitB;
  hitB.time = 2;

  ENSURE( CompareMCPEs(hitA, hitB), "Test says 1 >= 2. Clearly that's not good.");
}
