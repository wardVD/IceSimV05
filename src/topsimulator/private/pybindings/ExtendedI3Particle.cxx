#include <topsimulator/ExtendedI3Particle.h>
#include <boost/python.hpp>

void register_ExtendedI3Particle()
{
	using namespace boost::python;
	import("icecube.dataclasses"); // to load wrapper for I3Particle

	class_<ExtendedI3Particle, bases<I3Particle> >("ExtendedI3Particle")
		.def(init<I3Particle>())
		;
}
