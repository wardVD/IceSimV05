
#include <I3Test.h>

#include "neutrino-generator/legacy/I3NuGInjector.h"
#include "neutrino-generator/Particle.h"
#include "dataclasses/I3Map.h"
#include "TestUtils.h"
#include "phys-services/surfaces/Cylinder.h"

TEST_GROUP(NuGInjector);

TEST(CircleInjection)
{
	boost::shared_ptr<I3NuGInjector> injector = nugen::test::make_injector(nugen::CIRCLE);
	
        // for old injector
	for (int i = 0; i < 1000; i++) {
		I3FramePtr frame(new I3Frame);
                I3MapStringDoublePtr wmap(new I3MapStringDouble());
		if (!injector->InjectInitialNeutrino(frame, wmap))
			continue;
		I3Particle p = *injector->GetIncidentNeutrino();
		// distance to closest approach;
		double d = -p.GetDir()*p.GetPos();
		double impact = (d*p.GetDir() + p.GetPos()).Magnitude();
		ENSURE(impact <= 1e3, "Neutrino is inside injection circle");
	}

}

TEST(CylinderInjection)
{
	boost::shared_ptr<I3NuGInjector> injector = nugen::test::make_injector(nugen::SURFACE);
	
	I3Surfaces::Cylinder surface(1000, 500);
	for (int i = 0; i < 1000; i++) {
		I3FramePtr frame(new I3Frame);
                I3MapStringDoublePtr wmap(new I3MapStringDouble());
		if (!injector->InjectInitialNeutrino(frame, wmap))
			continue;
		I3Particle p = *injector->GetIncidentNeutrino();
		std::pair<double, double> d = surface.GetIntersection(p.GetPos(), p.GetDir());
		ENSURE(std::isfinite(d.first), "Neutrino intersects sampling surface");
	}
}
