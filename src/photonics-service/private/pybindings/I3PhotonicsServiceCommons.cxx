#include "wrappers.h"

#include <icetray/python/copy_suite.hpp>

typedef enum {
	L                = 1 << 0,
	RHO              = 1 << 1,
	PHI              = 1 << 2,
	TIME             = 1 << 3,
	DEPTH            = 1 << 4,
	ZENITH           = 1 << 5,
	CLOSEST_APPROACH = 1 << 7
} Interpolation;

#define INTERP (L)(RHO)(PHI)(TIME)(DEPTH)(ZENITH)(CLOSEST_APPROACH)

#define GEOMETRY (CYLINDRICAL)(SPHERICAL)(CUBIC)
#define PARITY (EVEN)(ODD)

using namespace boost::python;

void register_I3PhotonicsServiceCommons() {
	
#ifdef USE_NUMPY
	def("getJacobian", &getJacobian, getJacobian_overloads(
	    args("x", "y", "z", "source", "geometry", "parity")));
	def("getHessian", &getHessian, getHessian_overloads(
	    args("x", "y", "z", "source", "geometry", "parity")));
	def("getPhotonicsCoordinates", &getPhotonicsCoordinates,
	    getPhotonicsCoordinates_overloads(
	    args("x", "y", "z", "source", "geometry", "parity")));
#endif

	enum_<Interpolation >("Interpolation")
	    BOOST_PP_SEQ_FOR_EACH(WRAP_ENUM_VALUE, , INTERP);

	enum_<geo_type >("Geometry")
	    BOOST_PP_SEQ_FOR_EACH(WRAP_ENUM_VALUE, , GEOMETRY);

	enum_<parity_type >("Parity")
	    BOOST_PP_SEQ_FOR_EACH(WRAP_ENUM_VALUE, , PARITY);
	
}
