/**
 *@file
 *@brief Implementation of class I3PhotonicsTableService
 *
 *@author Klas Hultqvist
 *@version $Id$
 *@date $Date$
 *(c) the IceCube Collaboration
 */

// C/C++ includes
#include <sys/param.h>
#include <iostream>
#include <ostream>
#include <cmath>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

// Local includes
#include "icetray/I3Units.h"
#include "photonics-service/I3PhotonicsTableService.h"
#include "photonics-service/I3PhotonicsServiceCommons.h"
#include "boost/filesystem.hpp"

using namespace std;
using boost::filesystem::exists;
using boost::filesystem::path;
using boost::filesystem::current_path;
using boost::filesystem::remove;
using boost::filesystem::filesystem_error;

#ifdef PHOTONICS_SERVICE_ROOT
// ROOT class definition macro
ClassImp(I3PhotonicsTableService);
#endif

using std::ostream;

path SwitchDir(const path newDir);

I3PhotonicsTableService::I3PhotonicsTableService() :
    I3PhotonicsService(), interpolationMode_(7),
    level1TablesLoaded_(false), level2TablesLoaded_(false),
    level1DriverFileName_(""), level2DriverFileName_(""),
    photonicsTopLevelDirectory_(""),
    isPhotorecTable_(false)
{ 
  l1_photonics_ = new I3PhotonicsL1Reader();
  l2_photonics_ = new I3PhotonicsL2Reader();
}

I3PhotonicsTableService::I3PhotonicsTableService(
    const std::string& photonicsTopLevelDirectory,
    const std::string& driverFileDirectory, const std::string& level1DriverFile,
    const std::string& level2DriverFile, int interpolMode, int verbosity,
    double angularSelectLow, double angularSelectHigh, double zSelectLow,
    double zSelectHigh, int photonicsTableSelection, bool isPhotorecTable) :

    I3PhotonicsService(), interpolationMode_(interpolMode),
    level1TablesLoaded_(false), level2TablesLoaded_(false),
    level1DriverFileName_(""), level2DriverFileName_(""),
    photonicsTopLevelDirectory_(""),isPhotorecTable_(isPhotorecTable)
{ 
	l1_photonics_ = new I3PhotonicsL1Reader();
	l2_photonics_ = new I3PhotonicsL2Reader();

	if (verbosity)
		log_warn("I3PhotonicsTableService verbosity deprecated: use "
		     "IceTray logging to change logging levels");
      
	if (photonicsTableSelection == 0 || photonicsTableSelection == 1) {
		if (!exists(driverFileDirectory+"/"+level1DriverFile))
			log_fatal("Level 1 driver file %s does not exist",
			    (driverFileDirectory+"/"+level1DriverFile).c_str());
		SetAngularSelection(1, angularSelectLow/I3Units::degree,
		    angularSelectHigh/I3Units::degree);
		SetDepthSelection(1, zSelectLow/I3Units::m,
		    zSelectHigh/I3Units::m);
		if (!LoadTables(1, driverFileDirectory + "/" + level1DriverFile,
		    photonicsTopLevelDirectory))
			log_fatal("Failed to load level 1 photonics tables");
	}

	if (photonicsTableSelection == 0 || photonicsTableSelection == 2) {
		if (!exists(driverFileDirectory+"/"+level2DriverFile))
			log_fatal("Level 2 driver file %s does not exist",
			    (driverFileDirectory+"/"+level2DriverFile).c_str());
		SetAngularSelection(2, angularSelectLow/I3Units::degree,
		    angularSelectHigh/I3Units::degree);
		SetDepthSelection(2, zSelectLow/I3Units::m,
		    zSelectHigh/I3Units::m);
		if (!LoadTables(2, driverFileDirectory + "/" + level2DriverFile,
		    photonicsTopLevelDirectory))
			log_fatal("Failed to load level 2 photonics tables");
	}
}

I3PhotonicsTableService::~I3PhotonicsTableService()
{
	log_info("Destructor called for I3PhotonicsTableService \n");
	if (level1TablesLoaded_) {
		l1_photonics_->free_tablesets_level1(); 
		level1TablesLoaded_ = false;
	}
	if (level2TablesLoaded_) {
		l2_photonics_->free_tablesets_level2(); 
		level2TablesLoaded_ = false;
	} 
	delete l1_photonics_;
	delete l2_photonics_;
}

void
I3PhotonicsTableService::SetInterpolationMode(int const mode)
{
	interpolationMode_ = mode;
}

bool
I3PhotonicsTableService::SetPhotonicsVerbosity(int const level,
    int const verbosity)
{
	log_warn("I3PhotonicsTableService verbosity deprecated: use "
	    "IceTray logging to change logging levels");
	return false;
}

bool
I3PhotonicsTableService::SetAngularSelection(int const level,
    float const low, float const high)
{
	log_debug("Setting level %d angular selection to %f<angle<%f",
	    level,low,high);

	// store angular selection so it can be queried by modules using service
	angularSelectLow_ = low;
	angularSelectHigh_ = high;

	if (level == 1) {
		l1_photonics_->set_level1_angular_interval(low,high);
		if (level1TablesLoaded_) {
			// Re-load tables if already loaded
			l1_photonics_->free_tablesets_level1();
			return LoadLevel1Tables();
		}
	} else if (level == 2) {
		l2_photonics_->set_level2_angular_interval(low,high);
		if (level2TablesLoaded_) {
			// Re-load tables if already loaded
			l2_photonics_->free_tablesets_level2(); 
			return LoadLevel2Tables();
		}
	} else {
		log_error("SetAngularSelection failed! Invalid photonics "
		    "format specified: level %d. Should be 1 or 2", level);
		return false;
	}
	
	return true;
}

bool
I3PhotonicsTableService::SetDepthSelection(int const level,
    float const low, float const high)
{
	log_debug("Setting level %d depth selection to %f<depth<%f",
	    level,low,high);

	// store depth selection so it can be queried by modules using service
	depthSelectLow_ = low;
	depthSelectHigh_ = high;

	if (level == 1) {
		l1_photonics_->set_level1_depth_interval(low,high);
		if (level1TablesLoaded_) {
			// Re-load tables if already loaded
			l1_photonics_->free_tablesets_level1();
			return LoadLevel1Tables();
		}
	} else if (level == 2) {
		l2_photonics_->set_level2_depth_interval(low,high);
		if (level2TablesLoaded_) {
			// Re-load tables if already loaded
			l2_photonics_->free_tablesets_level2(); 
			return LoadLevel2Tables();
		}
	} else {
		log_error("SetDepthSelection failed! Invalid photonics format "
		    "specified: level %d. Should be 1 or 2", level);
		return false;
	}
	
	return true;
}

bool
I3PhotonicsTableService::LoadTables(int const level, std::string driverFile,
    std::string topLevel)
{
	if (level == 1) {
		if (level1TablesLoaded_) {
			log_error("LoadTables failed: Level 1 tables "
			    "already loaded!");
		} else {
			level1DriverFileName_ = driverFile;
			photonicsTopLevelDirectory_ = topLevel;
			return LoadLevel1Tables();
		}
	} else if (level == 2) {
		if (level2TablesLoaded_) {
			log_error("LoadTables failed: Level 2 tables "
			    "already loaded!");
		} else {
			level2DriverFileName_ = driverFile;
			photonicsTopLevelDirectory_ = topLevel;
			return LoadLevel2Tables();
		}
	} else {
		log_error("LoadTable failed! Invalid photonics format "
		    "specified : level %d. Should be 1 or 2", level);
	}
	
	return false;
}

void
I3PhotonicsTableService::ClearTables(int const level)
{
	// Free table space for loaded tables
	if (level==1 && level1TablesLoaded_) {
		l1_photonics_->free_tablesets_level1(); 
		level1TablesLoaded_ = false;
	}
	if (level==2 && level2TablesLoaded_) {
		l2_photonics_->free_tablesets_level2(); 
		level2TablesLoaded_ = false;
	} 
}

bool
I3PhotonicsTableService::LoadLevel1Tables()
{
	path cwd = SwitchDir(photonicsTopLevelDirectory_.c_str());
	int pxsReturn = l1_photonics_->load_tables(level1DriverFileName_.c_str());
	cwd = SwitchDir(cwd);
	if (pxsReturn == 0) {
		log_error("LoadTables failed for level 1 tables for driver "
		    "file \"%s\" ", level1DriverFileName_.c_str());
		level1TablesLoaded_ = false;
		return false;
	}
	level1TablesLoaded_ = true;

	lmaxLevel1_ = l1_photonics_->get_level1_lmax();
	geometry_ = l1_photonics_->get_level1_geometry();
	return true;
}

bool
I3PhotonicsTableService::LoadLevel2Tables()
{
	path cwd = SwitchDir(photonicsTopLevelDirectory_.c_str());
	int pxsReturn =
	  l2_photonics_->load_level2_tables(level2DriverFileName_.c_str());
	cwd = SwitchDir(cwd);
	if(pxsReturn == 0 ) {
		log_error("LoadTables failed for level 2 tables for driver "
		    "file \"%s\" ", level2DriverFileName_.c_str());
		level2TablesLoaded_ = false;
		return false;
	}
	level2TablesLoaded_ = true;

	lmaxLevel2_ = l2_photonics_->get_level2_lmax();

	if (geometry_ != CYLINDRICAL)
		log_error("Attempted to load Level2 tables, but the "
		    "already-loaded Level1 tables have non-cylindrical "
		    "geometry (type %d). Level2 table lookups "
		    "will be wildly wrong!", static_cast<int>(geometry_));
	return true;
}

void
I3PhotonicsTableService::SelectModuleCoordinates(double const xOM, 
    double const yOM, double const zOM)
{
	xOM_ = xOM;
	yOM_ = yOM;
	zOM_ = zOM;
}

void
I3PhotonicsTableService::SelectSource(double &meanPEs,
    double &emissionPointDistance, double &geoTime,
    PhotonicsSource const &source)
{
	if (isPhotorecTable_) { //translate to a call to GetPhotorecInfo
		double probDensity; //this will be computed, but we will ignore it
		bool success=GetPhotorecInfo(meanPEs,probDensity,0.0,source);
		if (!success)
			meanPEs = 0;
		return;
	}
	
	// Calculate geometrical quantities	 
	if (!CalculatePhotonicsInput(true,xOM_,yOM_,zOM_,source)) {
		meanPEs = 0;
		return;
	}
	emissionPointDistance = emissionPointDistance_;
	
	// No table accessed yet:
	level_=-1;
	
	//
	// Find photonics "amplitude". For muons this is the light yield at OM 
	// for one emitted photon per metre of track length. For cascades it is 
	// for one emitted photon.
	//
	if(source.type == 0) {
		// Muon -- use level 2 tables
		if (source.length > 0) {
			if (level2TablesLoaded_) {
				float raw =
				    l2_photonics_->
				    get_level2_general_photon_density(
				    coordPxC_[3], coordPxC_[0], coordPxC_[1], 
				    coordPxC_[2], lStopPxC_, coordPxC_[4],
				    zStopPxC_, interpolationMode_, 
				    &tableSetId_, &tableId_, &stopTableId_,
				    &lBin_, &lBinStop_, &rhoBin_, &rhoBinStop_,
				    &phiBin_, &phiBinStop_ );
				rawYield_ = (double) raw; 
				level_ = 2;
			} else {
				log_info("Level 2 photonics tables not loaded "
				    "- no light for muon");
				rawYield_ = 0;
			}
		} else {
			// log_warn("Zero length muon. This is not a problem,
			//     but it means we had one before! \n");
			rawYield_ = 0;
		}
		//
		// Calculate "geometric time" (time for muon to propagate to
		// emission point + time for light to reach OM from there):
		//
		geoTime = (emissionPointOffset_ +
		    emissionPointDistance*nGroup_)/cVacuum_;
	} else {
		//Not muon -- use level1 tables
		if (level1TablesLoaded_) {
			// Use EM shower tables (type_=1) unless other
			// known type
			if (source.type != 13) {
				// All monopoles except the slowest 
				if (source.type < 9)
					type_ = 1;
				else if (source.type <= 12)
					type_ = source.type;
				else
					type_ = 1;
				
				float raw = l1_photonics_->get_photon_density(
				    coordPxC_[4],coordPxC_[3],type_,
				    coordPxC_[2],coordPxC_[0],
				    coordPxC_[1]/I3Units::degree,
				    interpolationMode_, &tableId_, &lBin_,
				    &rhoBin_,&phiBin_);
				
				rawYield_ = double(raw);
			} else {
				// if type 13 (slow monopoles) then set
				// rawYield_ to 0
				log_info("Level 1 photonics tables loaded, but"
				    " particle is a slow monopole, so raw "
				    "yield set to 0");
				rawYield_ = 0; 
			}
			level_ = 1;
		} else if (level2TablesLoaded_) {
			// This is probably a PhotoRec cascade table.
			rawYield_ =
			    l2_photonics_->get_level2_photon_density(
			    coordPxC_[4], coordPxC_[3], coordPxC_[2],
			    coordPxC_[0], coordPxC_[1],
			    interpolationMode_, &tableId_,
			    &lBin_, &rhoBin_, &phiBin_);
			level_ = 42;
		} else {
			log_info("Level 1 photonics tables not loaded - no " 
			    "light for source type %d ",source.type);
			rawYield_ = 0;
		}
		// 
		// "geometric time" for cascades corresponds to light
		// propagation directly from vertex
		// 
		geoTime = emissionPointDistance_ * nGroup_/cVacuum_;
	}
	//
	// Check photon density OK
	//
	if (!std::isnormal(rawYield_)) {
		log_debug("Photonics returned NAN or INF "
		    "yield, setting yield to 0");
		rawYield_ = 0;
	} else if (rawYield_ < 0) {
		log_debug("Photonics returned negative yield %f, "
		    "(probably because reading outside table limits) "
		    "setting yield to 0 ", rawYield_);
		rawYield_ = 0;
	} else if (rawYield_ == 0) {
		log_debug("Photonics yield was 0, "
		    "there will be no timing information availible");
	} else {
		//
		// Photonics yield OK. Must be multiplied by the number of 
		// photons per metre and,for cascades, scaled with energy to
		// account for the total length of tracks in cluster. For muons
		// there is also an energy-dependent correction for track 
		// length in low energy secondaries. For other sources, do not 
		// modify raw yield.
		// 
		if (source.type < 3 && source.E > 0) {
			float lightEnergy = source.E;
			// Calculate energy at nominal emission point for
			// stopping muons
			if (source.length > 0)
				lightEnergy =
				    EmissionEnergy(source.E,source.length);
			// For energy close to zero, light calculation gives
			// unreasonable results
			if (lightEnergy < 0.01) { 
				log_trace("Energy %f<0.01 GeV, using 0.01 "
				    "GeV for light output calculation", 
				    lightEnergy);
				lightEnergy = 0.01;
			}
			//Convert yield
			meanPEs = I3PhotonicsCommons::scaleLightYield(source,
			    rawYield_, lightEnergy);
			return;
		} else {
			log_trace("Will not scale yield with light factor."
			    "track type = %d, track energy = %f",
			    source.type, source.E);
		}
	}
	meanPEs = rawYield_;
}

void
I3PhotonicsTableService::GetTimeDelay(double const random,double &timeDelay)
{
	switch (level_) {
		case 1:
			timeDelay = l1_photonics_->get_photon_delay(random,
			    tableId_, lBin_, rhoBin_, phiBin_);
			break;
		case 2:
			timeDelay = l2_photonics_->get_level2_general_delay(
			    random, tableSetId_, tableId_, stopTableId_, lBin_,
			    lBinStop_, rhoBin_, rhoBinStop_, phiBin_,
			    phiBinStop_);
			break;
		default:
			log_fatal("GetTimeDelay failed -- invalid photonics "
			    "level: %d",level_);
			break;
	}
}

void
I3PhotonicsTableService::GetTimeDelays(I3RandomServicePtr random,
    double *timeDelays, int n)
{
	if (n == 1) {
		GetTimeDelay(random->Uniform(), timeDelays[0]);
		return;
	}

	typedef double (*uniform_t)(const void *r, double a, double b);
	typedef void (*multinomial_t)(const void *r, size_t k, unsigned int N,
	    const double p[], unsigned int n[]);

	switch (level_) {
	case 1:
		l1_photonics_->get_photon_delays(uniform_t(&gsl_ran_flat),
		    multinomial_t(&gsl_ran_multinomial), random->GSLRng(),
		    n, timeDelays, tableId_,lBin_,rhoBin_,phiBin_);
		break;
	case 2:
		for (int i = 0; i < n; i++)
			GetTimeDelay(random->Uniform(), timeDelays[i]);
		break;
	default:
		log_fatal("GetTimeDelays failed -- invalid photonics level: %d",level_);
		break;
	}
}

void
I3PhotonicsTableService::GetProbabilityDensity(double &density,
    double const timeDelay)
{
	if ( level_ == 1 ) {
		density = l1_photonics_->get_delay_prob(timeDelay,tableId_,
		    lBin_,rhoBin_, phiBin_); 
	} else if (level_ == 2 ) {
		density = l2_photonics_->get_level2_general_delay_prob(
		    timeDelay,tableSetId_,tableId_,stopTableId_,lBin_,
		    lBinStop_,rhoBin_,rhoBinStop_,phiBin_,phiBinStop_ );
	} else if (level_ == 42) {
		double amp;
		bool retval = l2_photonics_->get_level2_photorec(
		    coordPxC_[3],coordPxC_[0],coordPxC_[1],coordPxC_[2],
		    coordPxC_[4],timeDelay,&amp,&density,interpolationMode_,
		    0);
		if (!retval)
			density = 0;
	} else {
		log_fatal("GetProbabilityDensity failed -- invalid photonics "
		    "level: %d",level_);
	}
	
	if (density < 0)
		density = 0;
}

bool
I3PhotonicsTableService::GetProbabilityQuantiles(double *time_edges, double t_0,
    double *amplitudes, size_t n_bins) 
{
	for (size_t i = 0; i < n_bins; i++) {
		double bin_width = time_edges[i+1] - time_edges[i];
		double bin_center = (time_edges[i+1] + time_edges[i])/2. - t_0;
		double bin_prob;

		GetProbabilityDensity(bin_prob, bin_center);
		bin_prob = bin_prob * bin_width;
		amplitudes[i] = bin_prob;
	}

	return true;
}

bool
I3PhotonicsTableService::GetMeanAmplitudeGradient(double gradient[6])
{
	log_error("I3PhotonicsTableService doesn't know about gradients.");
	return false;
}

bool
I3PhotonicsTableService::GetMeanAmplitudeHessian(double gradient[6],
    double hessian[6][6])
{
	log_error("I3PhotonicsTableService certainly doesn't know about "
	    "Hessian matrices.");
	return false;
}

bool
I3PhotonicsTableService::GetProbabilityQuantileGradients(
    double *time_edges,double t_0, double gradients[][6], size_t n_bins)
{
	log_error("I3PhotonicsTableService doesn't know about gradients.");
	return false;
}

bool
I3PhotonicsTableService::GetProbabilityQuantileHessians(double *time_edges,
    double t_0, double values[], double gradients[][6],
    double hessians[][6][6], size_t n_bins)
{
	log_error("I3PhotonicsTableService certainly doesn't know about "
	    "second derivatives.");
	return false;
}

float
I3PhotonicsTableService::EmissionEnergy(double const E,double const length)
{
	if(emissionPointOffset_ > length)
		return 0;
	double a=(b_*E)/(exp(length*b_)-1);
	return (E+a/b_)*exp(-b_*emissionPointOffset_) - a/b_;
}	 

bool
I3PhotonicsTableService::GetPhotorecInfo(double &yield, double &probDensity,
    double delay, PhotonicsSource const &source)
{
	// Calculate geometrical quantities
	if(!CalculatePhotonicsInput(false,xOM_,yOM_,zOM_,source))
		return false;
	
	bool retval;
	//Calculate coordinates relative to end point for finite muon tracks
	if (( source.type== 0 ) && ( source.length > 0 )) {
		// Finite muon
		retval = l2_photonics_->get_level2_photorec_finite(
			    coordPxC_[3],coordPxC_[0],coordPxC_[1],
			    coordPxC_[2],lStopPxC_,coordPxC_[4], zStopPxC_,
			    delay,&yield,&probDensity,interpolationMode_,0);
	} else {
		retval = l2_photonics_->get_level2_photorec(
		    coordPxC_[3],coordPxC_[0],coordPxC_[1],coordPxC_[2],
		    coordPxC_[4],delay,&yield,&probDensity,interpolationMode_,
		    0);
	}

	if (!retval)
		return retval;

	// Don't scale for zero energy
	if (source.E == 0)
		return retval;
	float lightEnergy = source.E;

	// Never scale with energies below 0.01 GeV
	if (lightEnergy < 0.01)
		lightEnergy = 0.01; 

	// Scale amplitude with light factor
	yield = I3PhotonicsCommons::scaleLightYield(source, yield, lightEnergy);

	return retval;
}

bool
I3PhotonicsTableService::GetPhotorecDensities(double &yield,
    vector<double> &probDensities, vector<double> const delays,
    PhotonicsSource const &source)
{
	// Calculate geometrical quantities
	if (!CalculatePhotonicsInput(false,xOM_,yOM_,zOM_,source))
		return false;
	
	bool retval=true;
	// Calculate coordinates relative to end point for finite muon tracks
	int nPts=delays.size();
	if ((source.type== 0) && (source.length > 0)) {
		// Finite muon
		for (int k=0; k < nPts; k++) {
			retval = retval &&
			    l2_photonics_->get_level2_photorec_finite(
			    coordPxC_[3],coordPxC_[0],coordPxC_[1],
			    coordPxC_[2],lStopPxC_,coordPxC_[4],
			    zStopPxC_,delays[k], &yield, &probDensities[k],
			    interpolationMode_,0);
		}
	} else {
		for (int k=0; k<nPts; k++) {
			retval = retval && l2_photonics_->get_level2_photorec(
			    coordPxC_[3],coordPxC_[0],coordPxC_[1],
			    coordPxC_[2],coordPxC_[4],delays[k],
			    &yield,&probDensities[k],interpolationMode_,0);
		}
	}

	// Don't scale for zero energy
	if (source.E == 0)
		return retval;
	float lightEnergy = source.E;

	// Never scale with energies below 0.01 GeV
	if (lightEnergy < 0.01)
		lightEnergy = 0.01;

	// Scale amplitude with	light factor
	yield = I3PhotonicsCommons::scaleLightYield(source, yield, lightEnergy);

	return retval;
}

double
I3PhotonicsTableService::GetLmaxLevel1()
{
	return level1TablesLoaded_ ? lmaxLevel1_ : 0;
}

double
I3PhotonicsTableService::GetLmaxLevel2()
{
	return level2TablesLoaded_ ? lmaxLevel2_ : 0;
}

// Change directories and return the previous cwd, so we can do an
// easy swap. log_fatal()'ing is cheating here. We should fail
// gracefully and kick it up the chain

path
SwitchDir(const path newDir)
{
	path startWD = current_path();
	try {
		current_path(newDir);
	} catch (filesystem_error &e) {
		log_fatal("Error Changing Directory to %s: %s",
			  newDir.string().c_str(), e.what());
	}

	return (startWD);
}

