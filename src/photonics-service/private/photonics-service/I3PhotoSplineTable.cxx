#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#include <phys-services/I3RandomService.h>

#include "photonics-service/I3PhotonicsServiceCommons.h"
#include "photonics-service/I3PhotoSplineTable.h"

//Spline includes
extern "C" {
    #include "photospline/bspline.h"
    #include "photospline/splinetable.h"
    #include "photospline/splinepdf.h"
    #include "photonics-service/geo_type.h"
}

I3PhotoSplineTable::I3PhotoSplineTable() : 
    tablestruct_(), errorvalue_(-1), localbasis_(), centers_(),
    fastGradients_(true), adhoc_rng_(NULL) {}

I3PhotoSplineTable::~I3PhotoSplineTable()
{
	if (tablestruct_) {
		tablestruct_.reset();

		free(centers_);
		free(localbasis_);
	} 
	if (adhoc_rng_ != NULL)
		gsl_rng_free(adhoc_rng_);
}

static void
splinetable_destructor(splinetable *table)
{
	if (!table) return;

	splinetable_free(table);
	delete table;
}

bool
I3PhotoSplineTable::SetupTable(const std::string& path, double errorvalue)
{
	errorvalue_ = errorvalue;

	tablestruct_ = boost::shared_ptr<splinetable>(new splinetable,
	    splinetable_destructor);
	if (readsplinefitstable(path.c_str(), &*tablestruct_) == 0) {
		long geo, geotype, par, err;
		double nGroupTable;

		err = splinetable_read_key(tablestruct_.get(), SPLINETABLE_INT,
		    "GEOMETRY", &geo);
		if (err)
			geo = CYLINDRICAL;
		if (geo != SPHERICAL && geo != CYLINDRICAL)
			log_fatal("Table '%s': unknown geometry type %ld!",
			    path.c_str(), geo);
		geometry_ = (geo_type)geo;

		err = splinetable_read_key(tablestruct_.get(), SPLINETABLE_INT,
		    "GEOTYPE", &geotype);
		if (err) {
			log_warn("Table '%s': unset geometry type. Assuming "
			    "shower table", path.c_str());
			geotype = 0;
		}
		geotype_ = geotype;

		err = splinetable_read_key(tablestruct_.get(),
		    SPLINETABLE_DOUBLE, "NGROUP", &nGroupTable);
		if (err || nGroupTable == -1) {
			log_warn("No nGroup in table, using default! This is "
			    "probably not what you want");
			nGroupTable = I3PhotonicsCommons::nGroup;
		}
		nGroupTable_ = nGroupTable;

		err = splinetable_read_key(tablestruct_.get(), SPLINETABLE_INT,
		    "PARITY", &par);
		if (err) {
			// The first L1 tables were made with positive source
			// angles, but the first level2 tables should have
			// EVEN parity
			if (geotype_ == POINTSOURCE)
				par = ODD;
			else
				par = EVEN;
		}
		if (par != EVEN && par != ODD)
			log_fatal("Table '%s': unknown parity type %ld!",
			    path.c_str(), par);
		parity_ = (parity_type)par;

		return true;
	} else {
		tablestruct_.reset(); 
		return false;
	}
}

void
I3PhotoSplineTable::SetMaximumRadius(double maxRadius)
{
	double &radius = tablestruct_->extents[0][1];
	
	if (maxRadius > radius){
		log_warn("Requested maximum radius (%.1f m) larger than table extent "
		    "(%.1f m). The maximum radius will remain %.1f m",
		    maxRadius, radius, radius);
	} else {
		radius = maxRadius;
	}
}

/*
 * XXX: hard-coded for order-1 spline kernel.
 * FIXME: this should never be called on an amplitude table,
 *        lest nasal demons consume us all.
 */
void
I3PhotoSplineTable::SetTimeSmearing(double sigma)
{
	bool err = false;
	int n_knots = 3, time_axis;
	double knots[3] = {-2*sigma, 0, 2*sigma};

	switch (geotype_) {
	case POINTSOURCE:
		time_axis = 3;
		break;
	case INFINITEMUON:
		time_axis = 2;
		break;
	default:
		log_fatal("Unknown table geometry type %d", geotype_);
	}
	err = splinetable_convolve(tablestruct_.get(), time_axis, knots,
		    n_knots);

	if (err) {
		log_error("Error convolving splines along the time axis! "
		    "Genuine wierdness may be imminent.");
		return;
	}

	smearing_ = sigma;
}

bool
I3PhotoSplineTable::CheckSupport(double *coordinates)
{
	/*
	 * NB: We only check the coordinates that could actually be out of
	 * range. PhotonicsService::CalculatePhotonicsInput() can be expected
	 * to always return a table azimuth (and polar angle in spherical
	 * coordinates) in the proper range.
	 */
	#define OUTSIDE(dim)                                         \
		(coordinates[dim] < tablestruct_->extents[dim][0] || \
		 coordinates[dim] > tablestruct_->extents[dim][1])
	
	switch (geotype_) {
	case POINTSOURCE:
		/* Source depth */
		if (tablestruct_->ndim > 5 && OUTSIDE(5))
			return false;
		/* Source zenith angle */
		if (tablestruct_->ndim > 4 && OUTSIDE(4))
			return false;
		/* r/rho */
		if (OUTSIDE(0))
			return false;
		/* l (cylindrical geometry only) */	
		if (geometry_ == CYLINDRICAL && OUTSIDE(2))
			return false;
		break;
	case INFINITEMUON:
		/* Source depth */
		if (tablestruct_->ndim > 3 && OUTSIDE(3))
			return false;
		/* Source zenith angle */
		if (tablestruct_->ndim > 2 && OUTSIDE(2))
			return false;
		/* r/rho */
		if (OUTSIDE(0))
			return false;
		break;
	default:
		log_fatal("Unknown table geometry type %d", geotype_);
        }

	#undef OUTSIDE

	return true;
}

const double *const
I3PhotoSplineTable::GetTimeRange() const
{
	int time_axis;

	switch (geotype_) {
	case POINTSOURCE:
		time_axis = 3;
		break;
	case INFINITEMUON:
		time_axis = 2;
		break;
	default:
		log_fatal("Unknown table geometry type %d", geotype_);
	}

	return tablestruct_->extents[time_axis];
}

void
I3PhotoSplineTable::ProfileGradients()
{
	int i, j;
	int ndim = tablestruct_->ndim;
	int centers[ndim];
	double x[ndim], eval2[ndim + 1];
	const int nsamples = 1000;
	struct timeval tp1, tp2;
	double seq_time, combined_time;
	
	/* Evaluate at the middle of each dimension */
	for (i = 0; i < ndim; i++)
		x[i] = 0.5*(tablestruct_->extents[i][0]
		    + tablestruct_->extents[i][1]);
	
	/* Evaluate sequential method */
	gettimeofday(&tp1, NULL);	
	for (i = 0; i < nsamples; i++) {
		tablesearchcenters(&*tablestruct_, x, centers);
		ndsplineeval(&*tablestruct_, x, centers, 0);
		for (j = 0; j < ndim; j++)
			ndsplineeval(&*tablestruct_, x, centers, (1 << j));
	}
	gettimeofday(&tp2, NULL);
	
	tp2.tv_usec += (suseconds_t)((tp2.tv_sec - tp1.tv_sec)*1e6);
	seq_time = double(tp2.tv_usec - tp1.tv_usec)/nsamples;
	
	/* Evaluate combined method */
	gettimeofday(&tp1, NULL);	
	for (i = 0; i < nsamples; i++) {
		tablesearchcenters(&*tablestruct_, x, centers);
		ndsplineeval_gradient(&*tablestruct_, x, centers, eval2);
	}
	gettimeofday(&tp2, NULL);
	
	tp2.tv_usec += (suseconds_t)((tp2.tv_sec - tp1.tv_sec)*1e6);
	combined_time = double(tp2.tv_usec - tp1.tv_usec)/nsamples;

	log_debug("Sequential gradient evaluation time: %.2f "
	    "microseconds / Combined time: %.2f microseconds",
	    seq_time, combined_time);
	
	if (combined_time < seq_time) {
		fastGradients_ = true;
	} else {
		log_warn("Your processor's vector unit is retarded. Falling "
		    "back to sequential gradient evaluation (%.1f times "
		    "faster).", combined_time / seq_time);
		fastGradients_ = false;
	}
}

int
I3PhotoSplineTable::Eval(double *coordinates, double *result)
{
	// should be done only once for a table
	int centers[tablestruct_->ndim];

	// tablesearchcenters should get const double* coordinates
	if (tablesearchcenters(&*tablestruct_, coordinates, centers) == 0) {
		*result = ndsplineeval(&*tablestruct_, coordinates, centers, 0);
		return 0;
	}

	return EINVAL;
}

int
I3PhotoSplineTable::EvalGradients(double *coordinates, double *result)
{
	int dim;
	int centers[tablestruct_->ndim];

	if (tablesearchcenters(&*tablestruct_, coordinates, centers) != 0)
		return EINVAL; 

	if (fastGradients_) {
		ndsplineeval_gradient(&*tablestruct_, coordinates, centers,
		    result);
	} else {
		result[0] = ndsplineeval(&*tablestruct_, coordinates, centers,
		    0);
		for (dim = 0; dim < tablestruct_->ndim; dim++) {
			result[dim+1] = ndsplineeval(&*tablestruct_,
			    coordinates, centers, (1 << dim));
		}
	}
	
	return 0;
}

int
I3PhotoSplineTable::EvalHessian(double *coordinates, double result[6][6])
{
	unsigned i, j;
	unsigned ndim = tablestruct_->ndim;
	int centers[tablestruct_->ndim];

	if (tablesearchcenters(&*tablestruct_, coordinates, centers) != 0)
		return EINVAL; 

	for (i = 0; i < ndim; i++) {
		for (j = 0; j <= i; j++) {
			// For diagonal entries, we need the second derviative
			if (i == j)
				result[i][j] = ndsplineeval_deriv2(
				    &*tablestruct_, coordinates, centers,
				    (1 << i));
			// Mixed derivatives, otoh, separate nicely.
			else
				result[i][j] = ndsplineeval(&*tablestruct_,
				    coordinates, centers, (1 << i)|(1 << j));
		}
	}
	
	return 0;
}



int
I3PhotoSplineTable::EvalGradient(double *coordinates, double *result,
    unsigned dim)
{
	// In dimensions we don't have, everything is constant.
	if (dim+1 > (unsigned)(tablestruct_->ndim)) {
		result[0] = 0.;
		return 0;
	}

	// should be done only once for a table
	int centers[tablestruct_->ndim];

	// tablesearchcenters should get const double* coordinates
	if (tablesearchcenters(&*tablestruct_, coordinates, centers) == 0) {
		*result = ndsplineeval(&*tablestruct_, coordinates, centers,
		    (1 << dim));
		return 0;
	}

	return EINVAL;
}

/*
 * We implement sampling by using an independence chain Metropolis-Hastings
 * algorithm to sample from the spline PDF using the Pandel distribution
 * as a proposal distribution. The Pandel parameters are hard-coded using
 * nominal values for AMANDA. Note also that the efficiency of this algorithm
 * is vastly improved when retrieving a number of photon delay times at once.
 */

const static double lambda = 71.; // meter
const static double tau = 671.; // ns
const static double x0 = 154.; // meter
const static double c = 0.2998; // meter/ns
const static double n = 1.34;

// We widen the Pandel distribution by the following factor to avoid
// under-sampling the tails of the spline distribution in the
// Metropolis sampler.
const static double gamma_overdispersion = 1.1;

typedef struct {
	double distance;
	const gsl_rng *rng;
} pandel_info;

static double
pandel_sample(void *info)
{
	pandel_info *params = (pandel_info*)info;
	double gamma_scale = 1./(1./tau + c/n/x0);
	double gamma_shape = params->distance/lambda;

	gamma_scale *= gamma_overdispersion;
	gamma_shape /= gamma_overdispersion;

	double sample  = gsl_ran_gamma(params->rng, gamma_shape, gamma_scale);
	if (!std::isfinite(sample) || sample < 0)
		log_fatal("BORKED %e -> %e (%e, %e)", sample, params->distance, gamma_scale, gamma_shape);
	return sample;
}

static double
pandel_pdf(double t, double lastt, void *info)
{
	pandel_info *params = (pandel_info*)info;
	double gamma_scale = 1./(1./tau + c/n/x0);
	double gamma_shape = params->distance/lambda;

	gamma_scale *= gamma_overdispersion;
	gamma_shape /= gamma_overdispersion;

	/*
	 * The actual PDF would be obtained by
	 * gsl_ran_gamma_pdf(t, gamma_shape, gamma_scale);
	 *
	 * However, as a speed optimization, we will here neglect the
	 * demoninator, which is important for normalization but irrelevant
	 * for the formation of likelihood ratios.
	 */

	return exp(log(t)*(gamma_shape - 1.0) - t/gamma_scale);
}

void
I3PhotoSplineTable::GetTimeDelays(double *coords, double *times,
    int samples, long int seed)
{
	/* Set up our own GSL RNG to feed the sampler */
	if (adhoc_rng_ == NULL) {
		gsl_rng_env_setup();
		adhoc_rng_ = gsl_rng_alloc(gsl_rng_default);
	}

	gsl_rng_set(adhoc_rng_, seed);

	GetTimeDelays(coords, times, samples, adhoc_rng_);
}

void
I3PhotoSplineTable::GetTimeDelays(double *coords, double *times,
    int samples, I3RandomServicePtr random_service)
{
	GetTimeDelays(coords, times, samples, random_service->GSLRng());
}

void
I3PhotoSplineTable::GetTimeDelays(double *coords, double *times,
    int samples, const gsl_rng *rng)
{
	pandel_info params;
	int time_axis;

	params.rng = rng;

	switch (geotype_) {
	case POINTSOURCE:
		time_axis = 3;
		if (geometry_ == CYLINDRICAL)
			params.distance = hypot(coords[0], coords[2]);
		else if (geometry_ == SPHERICAL)
			params.distance = coords[0];
		else
			log_fatal("Unknown geometry %d\n", geometry_);
		break;
	case INFINITEMUON:
		time_axis = 2;
		params.distance = coords[0];
		break;
	default:
		log_fatal("Unknown table geometry type %d", geotype_);
	}


	/* Note: MH efficiency 63.5% */
	splinepdf_n_sample(times, samples, 3 /* burn-in */, coords,
            time_axis, &*tablestruct_, (1 << time_axis) /* derivatives */,
	    &pandel_sample, &pandel_pdf, &params, rng);
}

