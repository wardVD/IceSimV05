/**
 *@file 
 *@brief Implementation of class I3DummyPhotonicsService
 *
 *@author Klas Hultqvist
 *@version $Id$
 *@date $Date$
 *(c) the IceCube Collaboration
 */
//C/C++ includes
#include <iostream>
#include <ostream>
#include <cmath>
//Local includes
#include "photonics-service/I3DummyPhotonicsService.h"
#include "photonics-service/I3PhotonicsServiceCommons.h"

#ifdef PHOTONICS_SERVICE_ROOT
    ///ROOT class definition macro
    ClassImp(I3PhotonicsService); 
#endif

using namespace std;
using namespace I3DummyPhotonics;

I3DummyPhotonicsService::I3DummyPhotonicsService() : 
  I3PhotonicsService()
{
  cout << "************** I3DummyPhotonicsService *******************" << endl;
  cout << "* The DUMMY version of I3PhotonicsService is being used. *" << endl;
  cout << "* NO MEANINGFUL COMPARISONS with real data or photonics- *" << endl;
  cout << "* based simulation are possible                          *" << endl;
  cout << "**********************************************************" << endl;
}

void I3DummyPhotonicsService::SelectSource(
				  double &meanPEs,
				  double &emissionPointDistance,
				  double &geoTime,
				  PhotonicsSource const &source)
{
  //Calculate geometrical quantities and retrieve them (converting to doubles)
  if( !CalculatePhotonicsInput(true,xOM_,yOM_,zOM_,source) ){
    meanPEs = 0;
    emissionPointDistance = NAN;
    geoTime = NAN;
    return;
  }

  // making these local makes it clear that GetPhotonicsInput sets all of these
  // and you don't have to look at the methods signature to figure out that these
  // are all passed by reference ( as opposed to const reference or by-value)
  double zenith(NAN);
  double rho(NAN);
  double azimuth(NAN);
  double l(NAN);
  double z(NAN);
  double eOffset(NAN);
  GetPhotonicsInput(zenith,rho,azimuth,
                    l,z,eOffset,emissionPointDistance);

  if(std::isnan(zenith) || std::isnan(rho) || std::isnan(azimuth)
     || std::isnan(l)|| std::isnan(z) || std::isnan(eOffset) 
     || std::isnan(emissionPointDistance)){
    log_error("Something went horribly wrong when setting the following values:");
    log_error("  zenith = %.2f",zenith);
    log_error("  rho = %.2f",rho);
    log_error("  azimuth = %.2f",azimuth);
    log_error("  l = %.2f",l);
    log_error("  z = %.2f",z);
    log_error("  eOffset = %.2f",eOffset);
    log_error("  emissionPointDistance = %.2f",emissionPointDistance);
  }

  //"Geometric time", i.e. arrival time of direct photons relative to source time
  if(source.type == 0) 
    // Muon: Particle time to emission point + direct photon propagation
    geoTime = (eOffset + emissionPointDistance*nGroup_)/cVacuum_; 
  else 
    // Shower: Photon propagation from source position
  geoTime = sqrt( (xOM_-source.x)*(xOM_-source.x) + 
		  (yOM_-source.y)*(yOM_-source.y) + 
		  (zOM_-source.z)*(zOM_-source.z) )*nGroup_/cVacuum_;
  /**
   * Determine relevant distances and time offsets
   * dSpace:    distance determining overall light yield
   * dTime:     distance determining residual time distribution
   * tOffset_:  time offset when track does not reach nominal point
   *            of cherenkov emission
   */
#define BACKWARD 2 //Suppression factor if OM behind muon starting point (not optimised...) 
  double dSpace;
  double dTime;
  tOffset_ = 0;
  if(source.type == 0) {
    // muon:
    if(l >= source.length) {
      dSpace = sqrt(lStopPxC_*lStopPxC_ + rho*rho);
      if(eOffset > l) {
	dTime = dSpace;
	tOffset_ = (dSpace - emissionPointDistance)*nGroup_/cVacuum_;
      } else {
	dTime = emissionPointDistance;
      }
    } else if(l < 0) {
      dSpace = BACKWARD*sqrt(l*l + rho*rho);
      dTime = BACKWARD*dSpace;
    }else {
      dSpace = rho;
      dTime = emissionPointDistance;
    }
    meanPEs = A;
  } else {
    // cascade:
    dSpace =  sqrt( (xOM_-source.x)*(xOM_-source.x) + 
		    (yOM_-source.y)*(yOM_-source.y) + 
		    (zOM_-source.z)*(zOM_-source.z) );
    dTime = dSpace;
    meanPEs = C*source.E;
  }
  t0_ = T0SLOPE*dTime;
  k_ = 1/(t0_*t0_/2 + TAU*t0_);
  expNorm_ = k_*t0_*exp(t0_/TAU); 
  meanPEs *=  exp(-B*dSpace);  
}

void I3DummyPhotonicsService::GetTimeDelay(double const random,double &timeDelay)
{
  if(random < k_ * t0_*t0_ / 2) {
    timeDelay  = sqrt(2 * random / k_);
  }{
    timeDelay = - TAU * log ( exp(-t0_/TAU) - ( random -  k_ * t0_*t0_ / 2 ) / (TAU*expNorm_) );
  }
  timeDelay += tOffset_;
}

void I3DummyPhotonicsService::GetProbabilityDensity(double &density,double const timeDelay)
{
  double t = timeDelay - tOffset_;
  if(t > 0) {
    if(timeDelay < t0_) density = k_*t;
    else density = expNorm_ * exp(- t / TAU);
  } else {
    density = 0;
  }
}

double I3DummyPhotonicsService::GetIntegratedProbability(double const timeDelay)
{
  double t = timeDelay - tOffset_;
  double integral = 0;
  if (t > 0 && timeDelay < t0_)
      integral += k_*(timeDelay*timeDelay/2. - timeDelay*tOffset_);
  else if (t > 0)
      integral += k_*(t0_*t0_/2. - t0_*tOffset_);
  
  if (timeDelay >= t0_)
      integral += expNorm_*TAU*(exp(-(t0_ - tOffset_)/TAU) - exp(-t/TAU));
  return integral;
}

bool I3DummyPhotonicsService::GetProbabilityQuantiles(double *time_edges, double t_0, double *amplitudes, size_t n_bins)
{
  for (unsigned i = 0; i <= n_bins; i++)
    amplitudes[i] = GetIntegratedProbability(time_edges[i+1]) - GetIntegratedProbability(time_edges[i]);

  return true;
}

// Things we don't actually implement, all log_fatal

bool I3DummyPhotonicsService::GetMeanAmplitudeGradient(double *) { log_fatal("I3DummyPhotonicsService::%s not implemented", __func__); }
bool I3DummyPhotonicsService::GetMeanAmplitudeHessian(double *, double (*)[6]) { log_fatal("I3DummyPhotonicsService::%s not implemented", __func__); }
bool I3DummyPhotonicsService::GetProbabilityQuantileGradients(double *, double, double (*)[6], size_t) { log_fatal("I3DummyPhotonicsService::%s not implemented", __func__); }
bool I3DummyPhotonicsService::GetProbabilityQuantileHessians(double *, double, double *, double (*)[6],     double (*)[6][6], size_t) { log_fatal("I3DummyPhotonicsService::%s not implemented", __func__); }


