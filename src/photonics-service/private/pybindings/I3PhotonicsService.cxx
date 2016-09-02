#include "wrappers.h"

#include <icetray/python/list_indexing_suite.hpp>
#include <icetray/python/copy_suite.hpp>
#include <dataclasses/geometry/I3OMGeo.h>

#define SERVFIELDS \
    (SetAngularSelection)(GetAngularSelectionLow)(GetAngularSelectionHigh)\
    (SetDepthSelection)\
    (SelectModuleCoordinates)(SelectModule)(GetLmaxLevel1)(GetLmaxLevel2)\
    (CalculatePhotonicsInput)(GetPhotonicsInput)

#define PSFIELDS (x)(y)(z)(dirx)(diry)(dirz)(perpx)(perpy)(perpz)(zenith)\
    (E)(length)(speed)(type)(sintheta)(costheta)(sinphi)(cosphi)

using namespace boost::python;

void register_I3PhotonicsService() {
	class_<I3PhotonicsService,
	    boost::shared_ptr<I3PhotonicsService>,
	    boost::noncopyable>("I3PhotonicsService", boost::python::no_init)
	    BOOST_PP_SEQ_FOR_EACH(WRAP_DEF, I3PhotonicsService, SERVFIELDS)
            #define PROPS (TableCoordinates)(Geometry)(Parity)
            BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3PhotonicsService, PROPS)	
            #undef PROPS
	    .def("SelectSource", &SelectSource, SelectSource_overloads(
	        args("self", "source", "gradients"),
	        "Select a source for further queries. Returns the mean npe, "
	        "emission point distance and geotime."))
	    .def("GetTimeDelay", &GetTimeDelay, args("self", "random"),
	        "Returns a random delay time")
	    .def("GetTimeDelays", &GetTimeDelays, args("self",
	        "random_service", "size"), "Returns a vector of `size` random"
	        "random delay times")
	    .def("GetProbabilityDensity", &GetProbabilityDensity, args("self",
		"delay"), "Returns for a given delay time the delay time pdf.")
	    .def("GetPhotorecInfo", &Get, args("self", "delay", "source"),
	        "Get the time-delay probability density at delay time `delay`"
	        "and the total amplitude expected from light source `source`")
#ifndef USE_NUMPY
	    .def("GetProbabilityQuantiles", &GetProbabilityQuantiles,
	        args("self", "time_edges", "t0"),
	        "Returns a vector of mean amplitudes in each time bin "
	        "(relative to direct propagation time t_0)")
#else
	    .def("GetProbabilityQuantiles", &GetProbabilityQuantiles,
	        args("self", "time_edges", "t0", "with_gradients"),
	        "Returns a vector of mean amplitudes in each time bin "
	        "(relative to direct propagation time t_0)")
	    .def("GetMeanAmplitudeGradient", &GetMeanAmplitudeGradient,
	        args("self"), "Return the gradient of the total amplitude"
	        "with respect to source position (d/dx, d/dy, d/dz in PE/m),"
	        "direction of source origin (d/dtheta, d/dphi in PE/radian),"
	        "and source energy (d/dE in PE/GeV).")
	    .def("GetMeanAmplitudeHessian", &GetMeanAmplitudeHessian,
	        args("self"), "Return the gradient and a matrix of second "
	        "derivatives of the total amplitude with respect to source "
	        "position (d/dx, d/dy, d/dz in PE/m), direction of source "
	        "origin (d/dtheta, d/dphi in PE/radian), and source energy "
	        "(d/dE in PE/GeV).")
	    .def("GetProbabilityQuantileGradients", &GetProbabilityQuantileGradients,
	        args("self", "time_edges", "t_0"), "Return the gradient of",
	        "photon detection probability in the time windows given by"
	        "time_edges, given a direct propagation time t_0. Gradients "
	        "are given with respect to source position (d/dx, d/dy, d/dz"
	        " in 1/meter), event time (d/dt in 1/ns), and direction of "
	        "source origin (d/dtheta, d/dphi in 1/radian).")
	    .def("GetProbabilityQuantileHessians", &GetProbabilityQuantileHessians,
	        args("self", "time_edges", "t_0"), "Return a matrix of second derivatives of",
	        "photon detection probability in the time windows given by"
	        "time_edges, given a direct propagation time t_0. Derivatives "
	        "are given with respect to source position (d/dx, d/dy, d/dz"
	        " in 1/meter), event time (d/dt in 1/ns), and direction of "
	        "source origin (d/dtheta, d/dphi in 1/radian).")
#endif
	     ;

	class_<PhotonicsSource, boost::shared_ptr<PhotonicsSource> >("PhotonicsSource")
	    .def(init<const I3Particle &>())
	    .def(init<double, double, double, double, double, double, double,
	        double, int>(args("x", "y", "z", "zenith", "azimuth", "speed",
	        "length", "energy", "type")))
	    BOOST_PP_SEQ_FOR_EACH(WRAP_RW, PhotonicsSource, PSFIELDS);

	class_<std::vector<PhotonicsSource> >("vector_PhotonicsSource")
	    .def(list_indexing_suite<std::vector<PhotonicsSource> >())
	    .def(copy_suite<std::vector<PhotonicsSource> >())
	;
}

