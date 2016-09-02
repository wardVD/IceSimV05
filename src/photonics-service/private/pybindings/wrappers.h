#ifndef _I3PHOTONICSSERVICE_PYBINDINGS_WRAPPERS_H
#define _I3PHOTONICSSERVICE_PYBINDINGS_WRAPPERS_H

#include <photonics-service/I3PhotonicsService.h>
#include <photonics-service/I3PhotonicsServiceCommons.h>
#ifdef USE_PHOTOSPLINE
#include <photonics-service/I3PhotoSplineService.h>
#include <photonics-service/I3PhotoSplineTable.h>
#endif

#include <string>
#include <vector>

/*
 * When using the Numpy C API from multiple files, we have to 
 * tell it where to find the array of function pointers that
 * defines the API (this is static for single-file extensions).
 */
#define PY_ARRAY_UNIQUE_SYMBOL PhotonicsService_PyArray_API

using namespace boost::python;

/* Table lookup functions */
tuple SelectSource(I3PhotonicsService & self, const PhotonicsSource & src,
    bool with_gradients=false);

/* Sampling functions */
double GetTimeDelay(I3PhotonicsService & self, const double &random);
std::vector<double > GetTimeDelays(I3PhotonicsService & self,
    I3RandomServicePtr random, int n);

/* Differential probabilities */
tuple Get(I3PhotonicsService & self, const double &delay,
    const PhotonicsSource & source);
double GetProbabilityDensity(I3PhotonicsService & self, const double &delay);

/* Integral probabilities */
#ifdef USE_NUMPY
object GetProbabilityQuantiles(I3PhotonicsService &self, object time_edges,
    double t_0, bool with_gradients = false);
#else /* USE_NUMPY */
std::vector<double > GetProbabilityQuantiles(I3PhotonicsService & self,
    std::vector<double > &time_edges, double t_0);
#endif /* USE_NUMPY */

#ifdef USE_NUMPY
/* Gradients */
object GetMeanAmplitudeGradient(I3PhotonicsService &self);
object GetProbabilityQuantileGradients(I3PhotonicsService &self, object time_edges,
    double t_0);
object GetMeanAmplitudeHessian(I3PhotonicsService &self);
object GetProbabilityQuantileHessians(I3PhotonicsService &self, object time_edges,
    double t_0);

/* Coordinate transformations */
object getPhotonicsCoordinates(const double x, const double y, const double z,
    const PhotonicsSource &source, const geo_type geometry = CYLINDRICAL,
    const parity_type parity = EVEN );
object getJacobian(const double xOM, const double yOM, const double zOM,
    const PhotonicsSource &source, const geo_type geometry = CYLINDRICAL,
    const parity_type parity = EVEN);
object getHessian(const double xOM, const double yOM, const double zOM,
    const PhotonicsSource &source, const geo_type geometry = CYLINDRICAL,
    const parity_type parity = EVEN);

#endif /* USE_NUMPY */

#ifdef USE_PHOTOSPLINE
/* I3PhotoSplineTable functions */
double splinetableeval(I3PhotoSplineTable & self,
    object coordinates);
double splinetableeval_gradient(I3PhotoSplineTable & self,
    std::vector<double > &coordinates, int deriv_dim);
#ifdef USE_NUMPY
object splinetableeval_hessian(I3PhotoSplineTable &self,
    object coordinates);
object splinetableeval_gradients(I3PhotoSplineTable &self,
    object coordinates);
#endif /* USE_NUMPY */
#endif /* USE_PHOTOSPLINE */

/* Universal overloads */
BOOST_PYTHON_FUNCTION_OVERLOADS(SelectSource_overloads,
    SelectSource, 2, 3);

#ifdef USE_NUMPY
/* Numpy-dependent overloads */
BOOST_PYTHON_FUNCTION_OVERLOADS(getPhotonicsCoordinates_overloads,
    getPhotonicsCoordinates, 4, 6);
BOOST_PYTHON_FUNCTION_OVERLOADS(getJacobian_overloads,
    getJacobian, 4, 6);
BOOST_PYTHON_FUNCTION_OVERLOADS(getHessian_overloads,
    getHessian, 4, 6);

BOOST_PYTHON_FUNCTION_OVERLOADS(GetProbabilityQuantiles_overloads,
    GetProbabilityQuantiles, 3, 4);
#define GetProbabilityQuantiles_ARGS args("self", "time_edges", "t0", "gradients")

#else

BOOST_PYTHON_FUNCTION_OVERLOADS(GetProbabilityQuantiles_overloads,
    GetProbabilityQuantiles, 3, 4);
#define GetProbabilityQuantiles_ARGS args("self", "time_edges", "t0")

#endif

#endif /* _I3PHOTONICSSERVICE_PYBINDINGS_WRAPPERS_H */
