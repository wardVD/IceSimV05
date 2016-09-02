#include <gsl/gsl_randist.h>
#include <icetray/I3Units.h>
#include <icetray/I3SingleServiceFactory.h>
#include <photonics-service/I3PhotoSplineService.h>
#include "photonics-service/I3PhotonicsServiceCommons.h"
#include "photonics-service/I3PhotoSplineTable.h"
#include <cmath>   

I3PhotoSplineService::I3PhotoSplineService() :
    I3PhotonicsService(), I3ServiceBase("I3PhotoSplineService"),
    amplitudeSplineTable_(), timingSplineTable_(),
    meanPEs_(-1.), sourceType_(-1), sourceLength_(-2), lastSource_(),
    rawYield_(0.)
{ }

I3PhotoSplineService::I3PhotoSplineService(
    I3Context const &context) : I3PhotonicsService(), I3ServiceBase(context),
    amplitudeSplineTable_(), timingSplineTable_(),
    meanPEs_(-1.), sourceType_(-1), sourceLength_(-2), lastSource_(),
    rawYield_(0.)
{
	AddParameter("AmplitudeTable", "Path to the amplitude spline table.", "");
	AddParameter("TimingTable", "Path to the timing spline table.", "");
	AddParameter("TimingSigma", "Smear the arrival time distribution with "
	    "a B-spline of order 1 and this standard deviation.", 0.0);
	AddParameter("MaxRadius","Maximum expected distance for detected light from emitter",
	    std::numeric_limits<double>::infinity());
}

void I3PhotoSplineService::Configure()
{
	std::string amplitudeTable, timingTable;
	double timingSigma;
	GetParameter("AmplitudeTable", amplitudeTable);
	GetParameter("TimingTable", timingTable);
	GetParameter("TimingSigma", timingSigma);
	GetParameter("MaxRadius", maxRadius_);
	if (!LoadSplineTables(amplitudeTable, timingTable)) {
		log_error("Error while loading spline tables! Do the files "
		    "'%s' and '%s' exist?", amplitudeTable.c_str(),
		    timingTable.c_str());
		return;
	}

	if (timingSigma > 0)
		ConvoluteTime(timingSigma);
	
	if (std::isfinite(maxRadius_)) {
		amplitudeSplineTable_->SetMaximumRadius(maxRadius_);
		timingSplineTable_->SetMaximumRadius(maxRadius_);
	}
}

I3PhotoSplineService::I3PhotoSplineService(const std::string& amplitudeTable,
    const std::string& timingTable, double timingSigma, double maxRadius) :
    I3PhotonicsService(), I3ServiceBase("I3PhotoSplineService"),
    amplitudeSplineTable_(), timingSplineTable_(), maxRadius_(maxRadius),
    meanPEs_(-1.), sourceType_(-1), sourceLength_(-2), lastSource_(),
    rawYield_(0.)
{

	if (!LoadSplineTables(amplitudeTable, timingTable)) {
		log_error("Error while loading spline tables! Do the files "
		    "'%s' and '%s' exist?", amplitudeTable.c_str(),
		    timingTable.c_str());
		return;
	}

	if (timingSigma > 0)
		ConvoluteTime(timingSigma);

	maxRadius_ = maxRadius;
	if (std::isfinite(maxRadius_)) {
		amplitudeSplineTable_->SetMaximumRadius(maxRadius_);
		timingSplineTable_->SetMaximumRadius(maxRadius_);
	}
}
 
I3PhotoSplineService::~I3PhotoSplineService()
{ }
        
bool
I3PhotoSplineService::LoadSplineTables(std::string ampFileName,
    std::string timeFileName)
{
	amplitudeSplineTable_ = I3PhotoSplineTablePtr(new I3PhotoSplineTable());
	if (!amplitudeSplineTable_->SetupTable(ampFileName, -1)) {
		amplitudeSplineTable_.reset();
		return false;
	}

	timingSplineTable_ = I3PhotoSplineTablePtr(new I3PhotoSplineTable());
	if (!timingSplineTable_->SetupTable(timeFileName, -1)) {
		timingSplineTable_.reset();
		amplitudeSplineTable_.reset();
		return false;
	}

	assert( timingSplineTable_->GetGeometry() == 
	    amplitudeSplineTable_->GetGeometry() );

	geometry_ = amplitudeSplineTable_->GetGeometry();
	
        assert( timingSplineTable_->GetGeometryType() ==
            amplitudeSplineTable_->GetGeometryType() );

	geotype_ = amplitudeSplineTable_->GetGeometryType();

        assert( timingSplineTable_->GetNGroupTable() ==
            amplitudeSplineTable_->GetNGroupTable() );

	nGroup_ = timingSplineTable_->GetNGroupTable();

	assert( timingSplineTable_->GetParity() == 
	    amplitudeSplineTable_->GetParity() );
	
	parity_ = amplitudeSplineTable_->GetParity();

	return true;
}


void
I3PhotoSplineService::ConvoluteTime(double sigma)
{
	if (!timingSplineTable_)
		log_error("Timing spline table hasn't been loaded yet!");
	else
		timingSplineTable_->SetTimeSmearing(sigma);
}

void
I3PhotoSplineService::SelectModuleCoordinates(double OMx, double OMy,
    double OMz)
{
	xOM_ = OMx; 
	yOM_ = OMy; 
	zOM_ = OMz;
	meanPEs_ = -1; // invalidate cache
}

void 
I3PhotoSplineService::FillTableCoordinates(double tablecoordinates[6],
    bool timing)
{
	int timegap = timing ? 1 : 0;
	
	switch (geotype_) {
	case POINTSOURCE:
		tablecoordinates[0] = coordPxC_[0];
		tablecoordinates[1] =
		    coordPxC_[1]*I3Units::radian/I3Units::degree;

		// Round-off errors can push azimuthal angles out of the
		// valid region.
		if (tablecoordinates[1] > 180.0) tablecoordinates[1] = 180.0;
		else if (tablecoordinates[1] < 0.0) tablecoordinates[1] = 0.0;

		tablecoordinates[2] = coordPxC_[2];
		tablecoordinates[3] = -1; // Time, if necessary
		tablecoordinates[3+timegap] = coordPxC_[3];
		tablecoordinates[4+timegap] = coordPxC_[4];
		break;

	case INFINITEMUON:
		tablecoordinates[0] = coordPxC_[0];
		tablecoordinates[1] = coordPxC_[1]; // phi is in radians here!

		tablecoordinates[2] = -1; // Time, if necessary

		// Translate z to let l = 0 for degenerate tables
		// This transformation is from
		// photonics/level2/level2_table_io.c, line 2341 
		tablecoordinates[2+timegap] = coordPxC_[3];
		tablecoordinates[3+timegap] = coordPxC_[4] -
		    coordPxC_[2]*cos(coordPxC_[3]*M_PI/180);

		// Round-off errors can push azimuthal angles out of the
		// valid region.
		if (tablecoordinates[1] > M_PI) tablecoordinates[1] = M_PI;
		else if (tablecoordinates[1] < 0.0) tablecoordinates[1] = 0.0;
		break;
	default:
		log_fatal("Unknown spline table geometry %d", geotype_);
	}
}

void
I3PhotoSplineService::SelectSource(double &meanPEs,
    double &emissionPointDistance, double &geoTime,
    PhotonicsSource const &source)
{
	SelectSource(meanPEs, NULL, emissionPointDistance, geoTime, source,
	    true);
}

void
I3PhotoSplineService::SelectSource(double &meanPEs, double *gradient,
    double &emissionPointDistance, double &geoTime,
    PhotonicsSource const &source, bool getAmp)
{
	// check extends already here
    double ePDist = sqrt( pow( (xOM_ - source.x),2 ) + pow( (yOM_ - source.y),2) + pow((zOM_ - source.z),2) );
    if (  ePDist > maxRadius_  ) {   
        log_trace_stream("source distance " << ePDist << " > " << maxRadius_ << ", 0 PE");
        meanPEs = 0.0;
        emissionPointDistance = ePDist;
        geoTime = emissionPointDistance*nGroup_/cVacuum_;
        return;
    }
	// return cached values for same DOM/source config
	if (meanPEs_ >= 0 && lastSource_ == source && gradient == NULL) {
		meanPEs = meanPEs_;
		geoTime = geoTime_;
		emissionPointDistance = emissionPointDistance_;
		log_trace("Same source, returning PE=%4.2e geoTime=%4.2e "
		    "dist=%4.2e",meanPEs,geoTime,emissionPointDistance);
		return;
	}
	
	log_trace("Calculating geotime, distance, and maybe amp at position "
	    "%5.2f %5.2f %5.2f", xOM_,yOM_,zOM_);
	CalculatePhotonicsInput(false, xOM_, yOM_, zOM_, source);

	emissionPointDistance = emissionPointDistance_;
	sourceType_ = source.type;
	sourceLength_ = source.length;

	double tablecoordinates[6], buffer[7];
	FillTableCoordinates(tablecoordinates, false);

	/* Are the requested coordinates within bounds? */
	bool supported = amplitudeSplineTable_->CheckSupport(
	    tablecoordinates);

	switch (geotype_) {
	case POINTSOURCE:
		// showers and other point-like light sources
		
		geoTime = r_*nGroup_/I3PhotonicsCommons::cVacuum;
		break;
	case INFINITEMUON:
		/**
		 * Calculate "geometric time", which has two options for
		 * infinite muons:
		 *  - time for muon to propagate to emission point + time for
		 *  light to reach OM from there for muons (length >= 0)
		 *  - cascade-like direct time for lightsaber (length = -1)
		 */

		if (source.length == -1)
			geoTime = r_*nGroup_/
			    I3PhotonicsCommons::cVacuum;
		else
			geoTime = (emissionPointOffset_ + 
			    emissionPointDistance_*nGroup_)/cVacuum_;

		break;
	default:
		log_fatal("Unknown table geometry type %d", geotype_);
	}

	if (!supported) {
		rawYield_ = -1;
	} else if (!getAmp) {
		rawYield_ = 0;
	} else if (gradient == NULL) {
		if (amplitudeSplineTable_->Eval(tablecoordinates,
		    &rawYield_) == 0)
			rawYield_ = exp(rawYield_);
		else
			rawYield_ = -1;
	}

	if (gradient != NULL) {
		if (!supported || !getAmp) {
			memset(gradient, 0, 6*sizeof(gradient[0]));
		} else if (amplitudeSplineTable_->EvalGradients(
		    tablecoordinates, buffer) == 0) {
			rawYield_ = exp(buffer[0]);
			ConvertMeanAmplitudeGradient(
			    &buffer[1], gradient, source);
		} else {
			rawYield_ = -1;
			memset(gradient, 0, 6*sizeof(gradient[0]));
		}
	}

	/*
	* Check photon density 
	*/
	if ( !std::isfinite(rawYield_) ) {
		log_debug("Photonics returned NAN or INF yield, "
		    "setting yield to 0");
		rawYield_ = 0;
		meanPEs = -1;
	} else if ( rawYield_ <= 0 ) {
		log_debug("Photonics yield was 0, there will be no "
		    "timing information availible");
		meanPEs = -1;
	} else {
		// photon density is ok. scale to amplitude
		meanPEs = I3PhotonicsCommons::scaleLightYield(source,
		    rawYield_);
		log_trace("meanPEs = %.3e",meanPEs);
		
		if (gradient != NULL)
			ScaleMeanAmplitudeGradient(gradient,
			    rawYield_, meanPEs, source);
	}

	if (meanPEs >= 0) {
		lastSource_ = source;
		meanPEs_ = meanPEs;
		geoTime_ = geoTime;
	}
}

void
I3PhotoSplineService::GetTimeDelay(double random, double &timeDelay)
{
	double tablecoordinates[6];
	FillTableCoordinates(tablecoordinates, true);
    
	/* Note that this is MUCH more efficient if we send
	 * multiple samples upstream at once. */
	timingSplineTable_->GetTimeDelays(tablecoordinates,&timeDelay,1,
	   (long int)(random*LONG_MAX)) ;
}

void
I3PhotoSplineService::GetTimeDelays(I3RandomServicePtr random,
    double *timeDelays, int n)
{
	const double *time_range = timingSplineTable_->GetTimeRange();
	if (n > (time_range[1] - time_range[0])) {
		// Multinomial samples if we have more time delays than ns
		int nbins = 5*(time_range[1] - time_range[0]); // 200 ps
		double *bin_edges = new double[nbins+1];
		double *quantiles = new double[nbins];
		unsigned int *bin_counts = new unsigned int[nbins];

		for (int i = 0; i < nbins+1; i++)
			bin_edges[i] = time_range[0] +
			    (time_range[1] - time_range[0])/(nbins + 1) * i;
		GetProbabilityQuantiles(bin_edges, 0, quantiles, nbins);
		gsl_ran_multinomial(random->GSLRng(), nbins, n, quantiles,
		    bin_counts);
		delete [] quantiles;

		for (int i = 0, j = 0; i < nbins; i++)
			for (unsigned int k = 0; k < bin_counts[i]; k++, j++)
				timeDelays[j] = bin_edges[i];

		delete [] bin_edges;
		delete [] bin_counts;
		return;
	}

	double tablecoordinates[6];
	FillTableCoordinates(tablecoordinates, true);

	timingSplineTable_->GetTimeDelays(tablecoordinates, timeDelays, n,
	    random);
}

bool
I3PhotoSplineService::GetProbabilityQuantiles(double *time_edges,
    double t_0, double *amplitudes, size_t n_bins)
{
	return GetProbabilityQuantiles(time_edges, t_0, amplitudes,
	    NULL, n_bins);
}

void
I3PhotoSplineService::GetProbabilityDensity(double &density, double timeDelay)
{
	// need to make sure that SelectSource was called before
	if (rawYield_ < 0) {
		log_debug("no timing information available: rawYield_ < 0");
		density = 0.;
		return;
	}

	double tablecoordinates[6];
	int time_axis;
	FillTableCoordinates(tablecoordinates, true);

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
	tablecoordinates[time_axis] = timeDelay;

	density = 0;
	timingSplineTable_->EvalGradient(tablecoordinates, &density, time_axis);

	if (density < 0)
		density = 0.;
}

typedef I3SingleServiceFactory<I3PhotoSplineService, I3PhotonicsService>
    I3PhotoSplineServiceFactory;
I3_SERVICE_FACTORY(I3PhotoSplineServiceFactory);
