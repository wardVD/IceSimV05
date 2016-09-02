#ifndef I3PHOTOSPLINETABLE_H
#define I3PHOTOSPLINETABLE_H
/**
 *@file
 *@brief I3PhotoSplineTable
 *       A class that represents a fitted photonics tables.
 *
 *@author Eike Middell
 *(c) the IceCube Collaboration
 */


#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"
#include <string>

/* Geometry type definitions */
extern "C" {
#include "photonics-service/geo_type.h"
}

struct splinetable;
#ifndef __GSL_RNG_H__
struct gsl_rng;
#endif
I3_FORWARD_DECLARATION(I3RandomService);

enum {  // Table geometry types
	POINTSOURCE = 0,
	INFINITEMUON = 1,
};

class I3PhotoSplineTable {
public:
	I3PhotoSplineTable();
	virtual ~I3PhotoSplineTable();

	bool SetupTable(const std::string& path, double errorvalue);
	int Eval(double *x, double *result);
	int EvalGradient(double *x, double *result, unsigned derivdim);
	int EvalHessian(double *coordinates, double result[6][6]);
	int EvalGradients(double *x, double *results);

	bool CheckSupport(double *x);
	const double *const GetTimeRange() const;

	void GetTimeDelays(double *coords, double *times, int samples, 
	    I3RandomServicePtr random_service);
	void GetTimeDelays(double *coords, double *times, int samples, 
	    long int seed);
	    
	void SetMaximumRadius(double extents);
	void SetTimeSmearing(double sigma);
	double GetTimeSmearing(void) { return smearing_; };

	geo_type GetGeometry() { return geometry_; };
	int GetGeometryType() { return geotype_; };
	double GetNGroupTable() { return nGroupTable_; };
	parity_type GetParity() { return parity_; };

private:
	boost::shared_ptr<splinetable> tablestruct_;
	// the value return by ndsplineeval in case of errors
	double errorvalue_;
	double **localbasis_;
	int *centers_;
	geo_type geometry_;
	int geotype_;
	double nGroupTable_;
	parity_type parity_;
	bool fastGradients_;
	void ProfileGradients();
	double smearing_;

	gsl_rng *adhoc_rng_;

	void GetTimeDelays(double *coords, double *times, int samples, 
	    const gsl_rng *rng);

	SET_LOGGER("I3PhotoSplineTable");
};

I3_POINTER_TYPEDEFS(I3PhotoSplineTable);

#endif

