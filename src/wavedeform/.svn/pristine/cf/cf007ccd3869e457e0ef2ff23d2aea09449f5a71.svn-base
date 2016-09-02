
#include <icetray/load_project.h>
#include <icetray/I3Logging.h>
#include <dataclasses/I3Matrix.h>

std::vector<double>
nnls_lawson_hanson_wrapper(const I3Matrix &A, const std::vector<double> &y,
    double tolerance, unsigned min_iterations, unsigned max_iterations,
    unsigned npos, bool normaleq, bool solve_with_normaleq, bool verbose);

I3_PYTHON_MODULE(wavedeform) {
	load_project("wavedeform", false);
	
	namespace bp = boost::python;
	bp::def("nnls_lawson_hanson", &nnls_lawson_hanson_wrapper,
	    (bp::args("A", "y", "tolerance"),
	    bp::arg("min_iterations")=0, bp::arg("max_iterations")=UINT_MAX,
	    bp::arg("npos")=0,
	    bp::arg("normaleq")=false, bp::arg("solve_with_normaleq")=true, bp::arg("verbose")=false),
	    ":param A: Response matrix\n"
	    ":param y: data vector\n"
	    ":param tolerance: terminate once the largest element of the gradient is smaller than the tolerance\n"
	    ":param min_iterations: minimum number of iterations before checking tolerance\n"
	    ":param max_iterations: maximum number of iterations before terminating\n"
	    ":param npos: maximum number of positive coefficients to find\n"
	    ":param normaleq: if True, the inputs are the normal equations At*A and At*y instead of A and y\n"
	    ":param solve_with_normaleq: if True, solve passive sets smaller than 15 elements via the normal equations\n"
	    ":param verbose: print debugging info\n"
	);
}