
#include <icetray/I3Logging.h>
#include <dataclasses/I3Matrix.h>
#include <cholmod.h>

extern "C" {
cholmod_dense *
nnls_lawson_hanson(cholmod_sparse *A, cholmod_dense *y, double tolerance,
    unsigned min_iterations, unsigned max_iterations, unsigned npos,
    int normaleq, int solve_with_normaleq, int verbose, cholmod_common *c);
}

static boost::shared_ptr<cholmod_common> python_common;

// Self-destroying cholmod pointers.
namespace detail {

template <typename T>
struct cholmod_deleter {
	typedef int (*deleter_t)(T**, cholmod_common *c);
	cholmod_common *common;
	deleter_t deleter;
	cholmod_deleter(deleter_t d, cholmod_common *c) : common(c), deleter(d) {}
	void operator()(T *ptr)
	{
		if (ptr != NULL)
			deleter(&ptr, common);
	}
};

template <typename T>
typename cholmod_deleter<T>::deleter_t
get_deleter(T *object);

template <>
cholmod_deleter<cholmod_dense>::deleter_t
get_deleter(cholmod_dense*) { return cholmod_l_free_dense; }
template <>
cholmod_deleter<cholmod_sparse>::deleter_t
get_deleter(cholmod_sparse*) { return cholmod_l_free_sparse; }

template <typename T>
boost::shared_ptr<T>
make_shared(T *ptr, cholmod_common *c)
{
	return boost::shared_ptr<T>(ptr,
	    cholmod_deleter<T>(get_deleter(ptr), c));
}

}

std::vector<double>
nnls_lawson_hanson_wrapper(const I3Matrix &A, const std::vector<double> &y,
    double tolerance, unsigned min_iterations, unsigned max_iterations,
    unsigned npos, bool normaleq, bool solve_with_normaleq, bool verbose)
{
	if (python_common == NULL) {
		python_common.reset(new cholmod_common, &cholmod_l_finish);
		if (!cholmod_l_start(python_common.get()))
			log_fatal("Couldn't initialize cholmod_common!");
	}
	
	// Dress up our matrix to look like a cholmod_dense
	cholmod_dense dense;
	dense.nrow  = A.size1();
	dense.ncol  = A.size2();
	dense.d     = A.size1();
	dense.nzmax = A.data().size();
	dense.xtype = CHOLMOD_REAL;
	dense.dtype = 0;
	dense.x     = (double*)&A.data()[0];
	
	cholmod_sparse *sparseA = cholmod_l_dense_to_sparse(&dense, 1,
	    python_common.get());
	if (sparseA == NULL)
		log_fatal("Couldn't convert basis matrix to sparse form");
	
	cholmod_dense dense_y;
	dense_y.nrow  = y.size();
	dense_y.ncol  = 1;
	dense_y.d     = y.size();
	dense_y.nzmax = y.size();
	dense_y.xtype = CHOLMOD_REAL;
	dense_y.dtype = 0;
	dense_y.x     = (double*)&y.front();
	
	cholmod_dense *dense_x = nnls_lawson_hanson(sparseA, &dense_y,
	    tolerance, min_iterations, max_iterations, npos,
	    normaleq, solve_with_normaleq, verbose, python_common.get());
	if (dense_x == NULL)
		log_fatal("nnls_lawson_hanson() failed");
	
	std::vector<double> x(dense_x->nrow);
	std::copy((double*)(dense_x->x), (double*)(dense_x->x)+dense_x->nrow, &x.front());
	
	return x;
}