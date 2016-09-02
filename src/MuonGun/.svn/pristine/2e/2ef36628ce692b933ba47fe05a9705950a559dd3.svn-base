
#include <I3Test.h>

#include "MuonGun/EnergyDistribution.h"
#include "common.h"

TEST_GROUP(EnergyDistribution);

TEST(Equality)
{
	using namespace I3MuonGun;
	
	{
		OffsetPowerLaw p1(2, 500, 1, 1e10);
		OffsetPowerLaw p2(2, 500, 2, 1e10);
	
		ENSURE(p1 == p1);
		ENSURE(p2 == p2);
		ENSURE(!(p1 == p2));
	}
	
	{
		BundleModel p1 = load_model("Hoerandel5_atmod12_SIBYLL");
		BundleModel p2 = load_model("GaisserH4a_atmod12_SIBYLL");
		ENSURE(*p1.energy == *p1.energy);
		ENSURE(*p2.energy == *p2.energy);
		ENSURE(!(*p1.energy == *p2.energy));
	}
}
