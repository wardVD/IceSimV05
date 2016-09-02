
#define NO_IMPORT_ARRAY /* Just use the headers */
#define PY_ARRAY_UNIQUE_SYMBOL photospline_PyArray_API
#include <numpy/numpyconfig.h>
#ifdef NPY_1_7_API_VERSION
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#include <numpy/ndarrayobject.h>

#include "photospline/I3SplineTable.h"

namespace bp = boost::python;

#define PY_TYPESTRING(pyobj) \
	pyobj.ptr()->ob_type->tp_name

static double
splinetableeval(I3SplineTable &self, bp::object coordinates, int derivatives)
{
	double retvalue(NAN);
	double *coord_ptr;

	PyObject *coords;	
	coords = PyArray_ContiguousFromObject(coordinates.ptr(), NPY_DOUBLE, 0, 0);
	if (!coords) {
		PyErr_Format(PyExc_ValueError, "Can't convert object of type"
		    "'%s' to an array of doubles!", PY_TYPESTRING(coordinates));
		bp::throw_error_already_set();
	}
	coord_ptr = (double*)PyArray_DATA((PyArrayObject *)coords);
	
	self.Eval(coord_ptr, &retvalue, derivatives);
	Py_XDECREF(coords);

	return retvalue;
}

static double
splinetableeval_deriv2(I3SplineTable &self, bp::object coordinates, int derivatives)
{
	double retvalue(NAN);
	double *coord_ptr;

	PyObject *coords;	
	coords = PyArray_ContiguousFromObject(coordinates.ptr(), NPY_DOUBLE, 0, 0);
	if (!coords) {
		PyErr_Format(PyExc_ValueError, "Can't convert object of type"
		    "'%s' to an array of doubles!", PY_TYPESTRING(coordinates));
		bp::throw_error_already_set();
	}
	coord_ptr = (double*)PyArray_DATA((PyArrayObject *)coords);
	
	self.EvalDeriv2(coord_ptr, &retvalue, derivatives);
	Py_XDECREF(coords);

	return retvalue;
}

static bp::list
GetExtents(const I3SplineTable &self)
{
	bp::list extents;
	for (unsigned i=0; i < self.GetNDim(); i++) {
		std::pair<double, double> ext = self.GetExtents(i);
		extents.append(bp::make_tuple(ext.first, ext.second));
	}
	
	return extents;
}

void register_I3SplineTable() {
	bp::class_<I3SplineTable, boost::shared_ptr<I3SplineTable>, boost::noncopyable>
	    ("I3SplineTable", bp::init<const std::string&>(bp::arg("path")))
	    .def("eval", splinetableeval, (bp::args("coordinates"), bp::arg("derivatives")=0),
	        "Evaluate the spline surface at the given coordinates.\n\n"
	        ":param coordinates: N-dimensonal coordinates at which to evaluate\n"
	        ":param derivatives: A bitmask indicating the type of basis to use "
	                          "in each dimension. If the bit corresponding to "
	                          "a dimension is set, the basis in that "
	                          "dimension will consist of the derivatives of "
	                          "the usual B-spline basis, and result "
	                          "will be the gradient of the surface in that "
	                          "dimension.")
	    .def("eval_deriv2", splinetableeval_deriv2, (bp::args("coordinates"), bp::arg("derivatives")=0))
	    .add_property("ndim", &I3SplineTable::GetNDim)
	    .add_property("extents", &GetExtents)
	;
}

