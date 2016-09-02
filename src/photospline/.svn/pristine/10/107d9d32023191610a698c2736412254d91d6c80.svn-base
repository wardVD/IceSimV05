#include <cerrno>
#include <stdexcept>
#include <sstream>
#include <photospline/I3SplineTable.h>
#include <photospline/bspline.h>

I3SplineTable::I3SplineTable(const std::string &path)
{
	if (readsplinefitstable(path.c_str(), &table_) != 0)
		throw std::runtime_error("Couldn't read spline table " + path);
	if (splinetable_read_key(&table_, SPLINETABLE_DOUBLE, "BIAS", &bias_))
		bias_ = 0;
}

I3SplineTable::~I3SplineTable()
{
	splinetable_free(&table_);
}

int
I3SplineTable::Eval(double *coordinates, double *result, int derivatives) const
{
	int centers[table_.ndim];
	
	if (tablesearchcenters(&table_, coordinates, centers) == 0)
		*result = ndsplineeval(&table_, coordinates, centers, derivatives);
	else
		return EINVAL;
	
	*result -= bias_;
	
	return 0;
}

int
I3SplineTable::EvalDeriv2(double *coordinates, double *result, int derivatives) const
{
	int centers[table_.ndim];
	
	if (tablesearchcenters(&table_, coordinates, centers) == 0)
		*result = ndsplineeval_deriv2(&table_, coordinates, centers, derivatives);
	else
		return EINVAL;
	
	*result -= bias_;
	
	return 0;
}

std::pair<double, double>
I3SplineTable::GetExtents(int dim) const
{
	if (dim < 0 || dim >= table_.ndim)
		throw std::out_of_range("Dimension index out of range");
	return std::make_pair(table_.extents[dim][0], table_.extents[dim][1]);
}

double
I3SplineTable::GetField(const std::string &key) const
{
	double v = 0;

	if (splinetable_read_key(&table_, SPLINETABLE_DOUBLE, key.c_str(), &v)) {
		std::ostringstream msg;
		msg << "Invalid header key " << key;
		throw std::invalid_argument(msg.str());
	}
		
	return v;
}

