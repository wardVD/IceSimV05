#include <I3Test.h>

#include "neutrino-generator/legacy/I3NuGInjector.h"
#include "neutrino-generator/Particle.h"
#include "TestUtils.h"

TEST_GROUP(Particle);

TEST(MinimumVolume)
{
	using namespace nugen;
	boost::shared_ptr<I3NuGInjector> injector = test::make_injector(nugen::CIRCLE);
	
	for (int i = 0; i < 1000; i++) {
		I3FramePtr frame(new I3Frame);
                I3MapStringDoublePtr wmap(new I3MapStringDouble());
		if (!injector->InjectInitialNeutrino(frame, wmap))
			continue;
		ParticlePtr p = injector->GetIncidentNeutrino();
		InteractionGeo &geo = p->GetInteractionGeoRef();
		InteractionInfo &info __attribute__((unused)) = p->GetInteractionInfoRef();
		ENSURE(geo.GetModDetLenBefore() > 1200);
		ENSURE_DISTANCE(geo.GetModDetLenAfter(), 1200, 1e-2);
		
	}
}
