
#include "wrappers.h"
#include "icetray/I3Units.h"

#ifdef USE_NUMPY
#define NO_IMPORT_ARRAY /* Just use the headers */
#include <numpy/ndarrayobject.h>
#endif

using namespace boost::python;

tuple
Get(I3PhotonicsService & self, const double &delay,
    const PhotonicsSource & source)
{
	double amplitude = -1;
	double probability = -1;

	self.GetPhotorecInfo(amplitude, probability, delay, source);
	return make_tuple(amplitude, probability);
}

tuple
SelectSource(I3PhotonicsService & self, const PhotonicsSource & src, bool with_gradients)
{
	double meannpe = -1.0;
	double emissionPointDistance = -1.0;
	double geoTime = -1.0;	
#ifdef USE_NUMPY
	PyObject *gradient = NULL;
	npy_intp shape[1] = {6};

	if (with_gradients) {
		gradient = PyArray_SimpleNew(1, shape, NPY_DOUBLE);
		self.SelectSource(meannpe, (double *)PyArray_DATA(gradient),
		    emissionPointDistance, geoTime, src);
		return make_tuple(meannpe, object(handle<>(gradient)),
		   emissionPointDistance, geoTime);
	} else {
#endif
		self.SelectSource(meannpe, emissionPointDistance, geoTime, src);
		return make_tuple(meannpe, emissionPointDistance, geoTime);
#ifdef USE_NUMPY		
	}
#endif

}

double
GetTimeDelay(I3PhotonicsService & self, const double &random)
{
	double delay = -1.0;

	self.GetTimeDelay(random, delay);
	return delay;
}

double
GetProbabilityDensity(I3PhotonicsService & self, const double &delay)
{
	double prob = -1.0;

	self.GetProbabilityDensity(prob, delay);
	return prob;
}

#ifdef USE_NUMPY
object
GetProbabilityQuantiles(I3PhotonicsService &self, object time_edges,
    double t_0, bool with_gradients)
{
	PyObject *edge_arr = NULL, *quantiles = NULL, *gradients = NULL;
	double *edges;
	size_t n_bins;
	npy_intp shape[2];
	
	edge_arr = PyArray_FromObject(time_edges.ptr(), NPY_DOUBLE, 1, 1);
	/* FIXME: check for an error condition */
	edges = (double*)PyArray_DATA(edge_arr);
	n_bins = PyArray_DIMS(edge_arr)[0] - 1;
	
	shape[0] = n_bins;
	quantiles = PyArray_SimpleNew(1, shape, NPY_DOUBLE);
	
	if (with_gradients) {
		shape[1] = 6;
		gradients = PyArray_SimpleNew(2, shape, NPY_DOUBLE);
		self.GetProbabilityQuantiles(edges, t_0,
		    (double *)PyArray_DATA(quantiles), 
		    (double (*)[6])PyArray_DATA(gradients), n_bins);
	} else {
		self.GetProbabilityQuantiles(edges, t_0,
		    (double *)PyArray_DATA(quantiles), n_bins);
	}
	
	Py_XDECREF(edge_arr);
	
	if (with_gradients) {
		return object(make_tuple(handle<>(quantiles),
		    handle<>(gradients)));
	} else {
		return object(handle<>(quantiles));
	}
}

#else

std::vector<double >
GetProbabilityQuantiles(I3PhotonicsService & self,
    std::vector<double > &time_edges, double t_0)
{
	size_t n = time_edges.size();

	if (n < 2)
		return std::vector<double >();

	std::vector<double > amplitudes(n - 1, 0.0);

	self.GetProbabilityQuantiles(&time_edges.front(), t_0,
	    &amplitudes.front(), n - 1);
	return amplitudes;
}

#endif

std::vector<double >
GetTimeDelays(I3PhotonicsService & self, I3RandomServicePtr random, int n)
{	
	std::vector<double > delays(n, 0.0);

	self.GetTimeDelays(random, &delays.front(), n);
	return delays;
}

// I3PhotoSplineTable functions

#ifdef USE_PHOTOSPLINE

double
splinetableeval_gradient(I3PhotoSplineTable & self,
    std::vector<double > &coordinates, int deriv_dim)
{
	double retvalue;
	double *coords = &coordinates.front();

	self.EvalGradient(coords, &retvalue, deriv_dim);
	return retvalue;
}

#define PY_TYPESTRING(pyobj) \
	pyobj.ptr()->ob_type->tp_name

double
splinetableeval(I3PhotoSplineTable & self,
    object coordinates)
{
	double retvalue;
	double *coord_ptr;

#ifdef USE_NUMPY
	PyObject *coords;	
	coords = PyArray_ContiguousFromObject(coordinates.ptr(), NPY_DOUBLE, 0, 0);
	if (!coords) {
		PyErr_Format(PyExc_ValueError, "Can't convert object of type"
		    "'%s' to an array of doubles!", PY_TYPESTRING(coordinates));
		throw_error_already_set();
	}
	// redundant, but keeps clang-analyzer happy
	assert(coords);
	coord_ptr = (double*)PyArray_DATA(coords);
#else
	std::vector<double> &coords = extract<std::vector<double> & >(coordinates);
	coord_ptr = &coords.front();
#endif
	
	self.Eval(coord_ptr, &retvalue);
	
#ifdef USE_NUMPY
	Py_XDECREF(coords);
#endif

	return retvalue;
}

#ifdef USE_NUMPY
object
splinetableeval_hessian(I3PhotoSplineTable &self, object coordinates)
{
	PyObject *coords, *hess;
	npy_intp shape[2] = {6, 6};
	
	coords = PyArray_ContiguousFromObject(coordinates.ptr(), NPY_DOUBLE, 0, 0);
	if (!coords) {
		PyErr_Format(PyExc_ValueError, "Can't convert object of type"
		    "'%s' to an array of doubles!", PY_TYPESTRING(coordinates));
		throw_error_already_set();
	}
	assert(coords);
	hess = PyArray_SimpleNew(2, shape, NPY_DOUBLE);
	
	self.EvalHessian((double*)PyArray_DATA(coords),
	    (double (*)[6])PyArray_DATA(hess));
	
	Py_XDECREF(coords);
	
	return object(handle<>(hess));
}

object
splinetableeval_gradients(I3PhotoSplineTable &self, object coordinates)
{
	PyObject *coords, *grad;
	npy_intp shape[1] = {7};
	
	coords = PyArray_ContiguousFromObject(coordinates.ptr(), NPY_DOUBLE, 0, 0);
	if (!coords) {
		PyErr_Format(PyExc_ValueError, "Can't convert object of type"
		    "'%s' to an array of doubles!", PY_TYPESTRING(coordinates));
		throw_error_already_set();
	}
	assert(coords);
	grad = PyArray_SimpleNew(1, shape, NPY_DOUBLE);
	if (!grad) {
		PyErr_Format(PyExc_ValueError, "Can't convert object of type"
		    "'%s' to an array of doubles!", PY_TYPESTRING(coordinates));
		throw_error_already_set();
	}
	assert(grad);
	
	bzero((double *)PyArray_DATA(grad), shape[0]*sizeof(double));
	
	self.EvalGradients((double*)PyArray_DATA(coords),
	    (double *)PyArray_DATA(grad));
	
	Py_XDECREF(coords);
	
	return object(handle<>(grad));
}

#endif /* USE_NUMPY */

#endif /* USE_PHOTOSPLINE */

#ifdef USE_NUMPY
object
getJacobian(const double xOM, const double yOM, const double zOM,
    const PhotonicsSource &source, const geo_type geometry,
    const parity_type parity)
{
	const double dx = xOM - source.x;
	const double dy = yOM - source.y;
	const double dz = zOM - source.z;
	const double l = dx*source.dirx + dy*source.diry + dz*source.dirz;
	const double r = sqrt(dx*dx + dy*dy + dz*dz);
	const double rho = sqrt(r*r - l*l);
	double cosAzi(0);
	
	PyObject *arr;
	double *buf;
	npy_intp shape[2] = {6, 6};
	
	if (rho > 0)
		cosAzi = (dx*source.perpx + dy*source.perpy
		    + dz*source.perpz)/rho;
	if (parity == ODD)
		cosAzi *= -1;
	
	double jac[6][6];
	
	I3PhotonicsCommons::fillJacobian(xOM, yOM, zOM, r, rho, cosAzi, l,
	    source, jac, geometry, parity);

	arr = PyArray_SimpleNew(2, shape, NPY_DOUBLE);
	buf = (double*)PyArray_DATA(arr);
	memcpy(buf, jac, 6*6*sizeof(double));

	return object(handle<>(arr));
}

object
getHessian(const double xOM, const double yOM, const double zOM,
    const PhotonicsSource &source, const geo_type geometry,
    const parity_type parity)
{
	const double dx = xOM - source.x;
	const double dy = yOM - source.y;
	const double dz = zOM - source.z;
	const double l = dx*source.dirx + dy*source.diry + dz*source.dirz;
	const double r = sqrt(dx*dx + dy*dy + dz*dz);
	const double rho = sqrt(r*r - l*l);
	double cosAzi(0);
	
	PyObject *arr;
	double *buf;
	npy_intp shape[3] = {6, 6, 6};
	
	if (rho > 0)
		cosAzi = (dx*source.perpx + dy*source.perpy
		    + dz*source.perpz)/rho;
	if (parity == ODD)
		cosAzi *= -1;
	
	double hess[6][6][6];
	double jac[6][6]; /* this won't be returned for now */
	
	I3PhotonicsCommons::fillHessian(xOM, yOM, zOM, r, rho, cosAzi, l,
	    source, jac, hess, geometry, parity);

	arr = PyArray_SimpleNew(3, shape, NPY_DOUBLE);
	buf = (double*)PyArray_DATA(arr);
	memcpy(buf, hess, 6*6*6*sizeof(double));

	return object(handle<>(arr));
}

object
GetMeanAmplitudeGradient(I3PhotonicsService &self)
{
	PyObject* arr;
	npy_intp shape[1] = {6};
	
	arr = PyArray_SimpleNew(1, shape, NPY_DOUBLE);
	self.GetMeanAmplitudeGradient((double*)PyArray_DATA(arr));
	
	return object(handle<>(arr));
}

object
GetMeanAmplitudeHessian(I3PhotonicsService &self)
{
	PyObject *grad, *hess;
	npy_intp shape[2] = {6,6};
	
	grad = PyArray_SimpleNew(1, shape, NPY_DOUBLE);
	hess = PyArray_SimpleNew(2, shape, NPY_DOUBLE);
	self.GetMeanAmplitudeHessian((double*)PyArray_DATA(grad),
	    (double (*)[6])PyArray_DATA(hess));
	
	return make_tuple(handle<>(grad), handle<>(hess));
}

object
GetProbabilityQuantileGradients(I3PhotonicsService &self, object time_edges,
    double t_0)
{
	PyObject *edge_arr, *gradients;
	double *edges;
	size_t n_bins;
	npy_intp shape[2];
	
	edge_arr = PyArray_FromObject(time_edges.ptr(), NPY_DOUBLE, 1, 1);
	/* FIXME: check for an error condition */
	edges = (double*)PyArray_DATA(edge_arr);
	n_bins = PyArray_DIMS(edge_arr)[0] - 1;
	
	shape[0] = n_bins;
	shape[1] = 6;
	
	gradients = PyArray_SimpleNew(2, shape, NPY_DOUBLE);
	
	self.GetProbabilityQuantileGradients(edges, t_0,
	    (double (*)[6])PyArray_DATA(gradients), n_bins);
	
	Py_XDECREF(edge_arr);
	
	return object(handle<>(gradients));
}

object
GetProbabilityQuantileHessians(I3PhotonicsService &self, object time_edges,
    double t_0)
{
	PyObject *edge_arr, *values, *gradients, *hessians;
	double *edges;
	size_t n_bins;
	npy_intp shape[3] = {1, 6, 6};
	
	edge_arr = PyArray_FromObject(time_edges.ptr(), NPY_DOUBLE, 1, 1);
	/* FIXME: check for an error condition */
	edges = (double*)PyArray_DATA(edge_arr);
	n_bins = PyArray_DIMS(edge_arr)[0] - 1;
	
	shape[0] = n_bins;

	values = PyArray_SimpleNew(1, shape, NPY_DOUBLE);
	gradients = PyArray_SimpleNew(2, shape, NPY_DOUBLE);
	hessians = PyArray_SimpleNew(3, shape, NPY_DOUBLE);

	self.GetProbabilityQuantileHessians(edges, t_0,
	    (double *)PyArray_DATA(values),
	    (double (*)[6])PyArray_DATA(gradients), 
	    (double (*)[6][6])PyArray_DATA(hessians), n_bins);
	
	Py_XDECREF(edge_arr);
	
	return make_tuple(handle<>(values), handle<>(gradients), handle<>(hessians));
}

object
getPhotonicsCoordinates(const double x, const double y, const double z,
    const PhotonicsSource &source, const geo_type geometry,
    const parity_type parity)
{
	PyObject *arr;
	double *buf;
	const double dx = x - source.x;
	const double dy = y - source.y;
	const double dz = z - source.z;
	const double l = dx*source.dirx + dy*source.diry + dz*source.dirz;
	const double r = sqrt(dx*dx + dy*dy + dz*dz);
	const double rho = sqrt(r*r - l*l);
	const double costheta = l/r;
	double cosAzi(0);
	
	if (rho > 0)
		cosAzi = (dx*source.perpx + dy*source.perpy
		    + dz*source.perpz)/rho;
	
	npy_intp dims[1] = {6};
	
	arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
	buf = (double*)PyArray_DATA(arr);
	
	if (geometry == SPHERICAL) {
		buf[0] = r;
		buf[2] = costheta;
	} else if (geometry == CYLINDRICAL) {
		buf[0] = rho;
		buf[2] = l;
	}
	
	if (parity == ODD) {
		cosAzi *= -1;
		buf[4] = 180.0 - source.zenith;
	} else {
		buf[4] = source.zenith;	
	}
	
	buf[1] = acos(cosAzi)/I3Units::degree;
	buf[3] = 0;
	buf[5] = source.z;
	
	return(object(handle<>(arr)));
}

#endif
