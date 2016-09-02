
#ifndef PHOTOSPLINE_I3SPLINETABLE_H_INCLUDED
#define PHOTOSPLINE_I3SPLINETABLE_H_INCLUDED

#include <string>
#include <photospline/splinetable.h>

class I3SplineTable {
public:
	/**
	 * @param[in] path Path to a FITS file
	 */ 
	I3SplineTable(const std::string &path);
	virtual ~I3SplineTable();

	/** Evaluate the spline surface
	 * 
	 * @param[in]       x N-dimensonal coordinates at which to evaluate
	 * @param[out] result Value of spline surface at coordinates
	 * @param[in] derivatives A bitmask indicating the type of basis to use
	                          in each dimension. If the bit corresponding to
	                          a dimension is set, the basis in that
	                          dimension will consist of the derivatives of
	                          the usual B-spline basis, and result
	                          will be the gradient of the surface in that
	                          dimension.
	 * @returns 0 on success, non-zero otherwise
	 */
	int Eval(double *x, double *result, int derivatives=0) const;
	int EvalDeriv2(double *x, double *result, int derivatives=0) const;

	/** Get the number of dimensions */
	unsigned GetNDim() const { return table_.ndim; };
	/** Get the extent of full support in dimension dim */
	std::pair<double, double> GetExtents(int dim) const;
	
	/** Retrieve a value stored in the FITS header */
	double GetField(const std::string &key) const;
private:
	I3SplineTable(const I3SplineTable&);
	
	struct splinetable table_;
	double bias_;
};

#endif
