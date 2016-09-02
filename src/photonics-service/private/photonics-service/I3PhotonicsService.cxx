/**
 *@file
 *@brief Implementation of class I3PhotonicsService
 *
 *@author Klas Hultqvist
 *@version $Id: I3PhotonicsService.cxx 142443 2016-02-24 19:04:35Z snowicki $
 *@date $Date: 2016-02-24 13:04:35 -0600 (Wed, 24 Feb 2016) $
 *(c) the IceCube Collaboration
 */

#include <icetray/I3Units.h>
#include <photonics-service/I3PhotonicsService.h>
#include <dataclasses/geometry/I3OMGeo.h>

#include "photonics-service/I3PhotonicsServiceCommons.h"
#include "I3MPConstants.h"

PhotonicsSource::PhotonicsSource(const I3Particle &src)
{
	int photo_type;
	double speed = src.GetSpeed()/I3Constants::c;

	if (src.IsCascade()) {
		photo_type = 1;
	} else if (src.GetType() == I3Particle::Monopole) {
		if (speed <= I3MPConst::BETA_TYPE13)
			photo_type = 13;
		else if (speed <= I3MPConst::BETA_TYPE12)
			photo_type = 12;
		else if (speed <= I3MPConst::BETA_TYPE11)
			photo_type = 11;
		else if (speed <= I3MPConst::BETA_TYPE10)
			photo_type = 10;
		else 
			photo_type = 9;
	} else {
		photo_type = 0;
	}

	*this = PhotonicsSource(src.GetX()/I3Units::m, src.GetY()/I3Units::m,
	    src.GetZ()/I3Units::m, src.GetZenith()/I3Units::degree,
	    src.GetAzimuth()/I3Units::degree, speed, src.GetLength()/I3Units::m,
	    src.GetEnergy()/I3Units::GeV, photo_type);
}

PhotonicsSource::PhotonicsSource(double const x_, double const y_,
    double const z_, double const zenith_, double const azimuth_,
    double const speed_, double const length_, double const energy_,
    int const type_)
{
	// Position of reference (start) point
	x = x_;
	y = y_;
	z = z_;

	// theta and phi of source in radians
	double thetaR = (180-zenith_)*M_PI/180;
	double phiR = (azimuth_-180)*M_PI/180;
	// Calculate source direction in cartesian coordinates
	costheta = cos(thetaR);
	sintheta = sin(thetaR);
	cosphi   = cos(phiR);
	sinphi   = sin(phiR);
	dirx = sintheta*cosphi;
	diry = sintheta*sinphi;
	dirz = costheta;

	// Direction perpendicular to source direction and pointing toward
	// positive z, i.e.upwards (dir and perp lie in a vertical plane) 
	this->perpz = hypot(this->dirx, this->diry);
	if ( perpz > 0 ){
		perpx = - dirx * dirz/perpz;
		perpy = - diry * dirz/perpz;
	} else {
		// for a vertical source direction, choose a random horizontal
		// direction (should be irrelevant which one)
		perpx = 1.;
		perpy = 0.;
	}

	// Zenith angle of point on celestial sphere source is pointing away
	// from (standard IceCube zenith convention) 
	zenith = zenith_;

	// Energy
	E = energy_;

	// Length (zero for cascades)
	length = std::isnan(length_) ? 0 : length_;

	// Speed
	speed = speed_;

	// Photonics track type
	type = type_;
}

PhotonicsSource::PhotonicsSource() {}

bool
I3PhotonicsService::CalculatePhotonicsInput(bool CheckLmax, double xOM,
    double yOM, double zOM, const PhotonicsSource &source)
{
	//Geometry variables
	double rhox_,rhoy_,rhoz_;   // rho vector (perpendicular to axis) 

	// Find projection of OM position on principal axis. Calculate this
	// first to make it possible to skip as much as possible if outside
	// table range.
	l_ = (xOM - source.x)*source.dirx + (yOM - source.y)*source.diry + 
	    (zOM - source.z)*source.dirz;

	if (source.type == 0) {
		// For muons (should have length>0) calculate distance along
		// track direction from stopping point next. Skip if outside
		// range
		if( source.length > 0 ) {
			lStopPxC_ = (xOM - source.x - source.length*source.dirx)
			      *source.dirx + 
			    (yOM - source.y - source.length*source.diry)
			      *source.diry + 
			    (zOM - source.z - source.length*source.dirz)
			      *source.dirz;
			if (CheckLmax && lStopPxC_ > GetLmaxLevel2())
				return false;
			zStopPxC_ = source.z + source.length*source.dirz;
		}
	} else if ((geometry_ != SPHERICAL) && CheckLmax &&
	    l_ > GetLmaxLevel1()) {
		return false;
	}

	// Calculate rho vector (perpendicular to principal axis and connecting
	// it to OM) and its length
	rhox_ = xOM - (source.x + l_*source.dirx);
	rhoy_ = yOM - (source.y + l_*source.diry);
	rhoz_ = zOM - (source.z + l_*source.dirz);
	rho_ = sqrt(rhox_*rhox_ + rhoy_*rhoy_ + rhoz_*rhoz_);

	// Calculate spherical r (used to calculate geoTime for both geometries)
	r_ = hypot(rho_, l_);

	// Calculate Azimuthal angle of OM around principal axis (zero when OM
	// vertically above axis)
	if( rho_ != 0 )
		cosAzi_ = (rhox_*source.perpx + rhoy_*source.perpy +
		    rhoz_*source.perpz)/rho_;
	else
		cosAzi_ = 0.0;
	if (cosAzi_ > 1.0)
		cosAzi_=1;
	else if (cosAzi_ < -1.0)
		cosAzi_=-1;
  
	// If photomc was given _positive_ zenith angles, then the azimuthal
	// angle is measured with respect to a vector perpendicular to the
	// source direction, but pointing towards _negative_ IceCube z.
	if (parity_ == ODD) cosAzi_ *= -1;

	// Find distance from source to nominal cherenkov emission point
	emissionPointOffset_ = l_ - rho_ / tanCh_;

	// Find distance from nominal emission point to OM
	if (source.type == 0)
		emissionPointDistance_ = rho_ / sinCh_;
	else
		emissionPointDistance_ = sqrt(rho_*rho_ + l_*l_);

	// Set variables for Photonics lookup

	// Azimuthal angle with respect to a vector perpendicular to the track
	// direction and pointing towards +z (-z if photomc was given a
	// positive zenith angle)
	coordPxC_[1] = acos(cosAzi_);
  
	// Direction from with the track came, in degrees
	if (parity_ == ODD)
		coordPxC_[3] = 180.0 - source.zenith;
	else
		coordPxC_[3] = source.zenith;

	// Depth of track, in meters
	coordPxC_[4] = source.z;
  
	switch (geometry_) {
	case SPHERICAL:
		cosZenith_ = l_/r_;
		coordPxC_[0] = r_;
		coordPxC_[2] = cosZenith_;
		break;
	default: // i.e. CYLINDRICAL
		coordPxC_[0] = rho_;
		coordPxC_[2] = l_;
		break;
	}

	return true;
}

I3PhotonicsService::I3PhotonicsService() :
  geometry_(CYLINDRICAL),
  parity_(EVEN)
{ 
	b_ = 3.3E-4; 
	nPhase_ = I3PhotonicsCommons::nPhase;
	nGroup_ = I3PhotonicsCommons::nGroup;
	cVacuum_ = I3PhotonicsCommons::cVacuum;
	tanCh_ = sqrt(nPhase_*nPhase_ - 1.0);
	sinCh_ = tanCh_ / nPhase_;
}

I3PhotonicsService::~I3PhotonicsService() {}

PhotonicsSource
I3PhotonicsService::FillPhotonicsSource(double const x, double const y,
    double const z, double const zenith, double const azimuth,
    double const length, double const energy, int const type)
{
	// OLD API: THIS FUNCTION IS DEPRECATED
	return PhotonicsSource(x, y, z, zenith, azimuth, 1 /* assume c */,
	    length, energy, type);
}

void
I3PhotonicsService::SelectModuleCoordinates(double const xOM, double const yOM,
    double const zOM)
{
	xOM_ = xOM;
	yOM_ = yOM;
	zOM_ = zOM;
}

void
I3PhotonicsService::SelectModule(const I3OMGeo &geo)
{
	SelectModuleCoordinates(
	    geo.position.GetX(),
	    geo.position.GetY(),
	    geo.position.GetZ());
}

void
I3PhotonicsService::GetTimeDelays(I3RandomServicePtr random,
    double *timeDelays, int n)
{
	for (int i = 0; i < n; i++)
		GetTimeDelay(random->Uniform(), timeDelays[i]);
}

void
I3PhotonicsService::GetPhotonicsInput(double& zenith, double& rho,
    double& phiOM, double& l, double& z, double& eOffset, double& eDistance)
{
	zenith = coordPxC_[3];
	rho = coordPxC_[0];
	phiOM = coordPxC_[1];
	l = coordPxC_[2];
	z = coordPxC_[4];
	eOffset = emissionPointOffset_;
	eDistance = emissionPointDistance_;
}

void
I3PhotonicsService::SelectSource( double &meanPEs, double gradients[6],
    double &emissionPointDistance, double &geoTime,
    PhotonicsSource const &source, bool getAmp)
{
	SelectSource(meanPEs, emissionPointDistance, geoTime, source);
	if (gradients != NULL)
		GetMeanAmplitudeGradient(gradients);
}

void
I3PhotonicsService::SelectSource(double &meanPEs, double &emissionPointDistance,
    double &geoTime, double x, double y, double z, double zenith,
    double azimuth, double length, double Energy, int type)
{
	PhotonicsSource s(x, y, z, zenith, azimuth, 1 /* assume c */,
	    length, Energy, type);
	SelectSource(meanPEs, emissionPointDistance, geoTime, s);
}

bool
I3PhotonicsService::GetProbabilityQuantiles(double *time_edges, double t_0,
    double *amplitudes, double gradients[][6], size_t n_bins)
{
	bool ret;
	ret = GetProbabilityQuantiles(time_edges, t_0, amplitudes, n_bins);
	if (!ret || gradients == NULL)
		return ret;
	return GetProbabilityQuantileGradients(time_edges, t_0, gradients,
	    n_bins);
}

bool
I3PhotonicsService::GetPhotorecInfo(double &yield, double &probDensity,
    double delay, PhotonicsSource const &source)
{
	double junk;
	SelectSource(yield, junk, junk, source);
	GetProbabilityDensity(probDensity, delay);

	return true;
}

bool
I3PhotonicsService::GetPhotorecDensities( double &yield,
    std::vector<double> &probDensities, std::vector<double> delays,
    PhotonicsSource const &source)
{
	bool ret = true;

	for (unsigned i = 0; i < delays.size(); i++)
		ret = GetPhotorecInfo(yield, probDensities[i], delays[i],
		    source);

	return ret;
}

bool
I3PhotonicsService::GetPhotorecInfo(double &yield, double &probdensity,
    double delay, double x, double y, double z, double zenith,
    double azimuth, double length, double Energy, int type)
{
	PhotonicsSource s(x, y, z, zenith, azimuth, 1 /* assume c */,
	    length, Energy, type);
	return GetPhotorecInfo(yield, probdensity, delay, s);
}

