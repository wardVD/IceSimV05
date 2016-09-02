#include <topsimulator/GeoFunctions.h>
#include <boost/python.hpp>

namespace {
	bool (*intersect_cylinder)(
		const I3Position&,
		const double&, const double&,
		const I3Particle&,
		I3Particle&
	) = GeoFunctions::IntersectCylinder;
}

void register_GeoFunctions() {
	using namespace boost::python;

	def("intersect_cylinder", intersect_cylinder);
}
