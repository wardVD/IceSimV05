#ifndef I3DUMMYPHOTONICSSERVICE_H
#define I3DUMMYPHOTONICSSERVICE_H
/**
 *@file
 *@brief I3DummyPhotonicsService
 *       A class which re-implements methods from I3PhotonicsService
 *       with dummy ones which do not use photonics tables
 *@author Klas Hultqvist
 *@version $Id$
 *@date $Date$
 *(c) the IceCube Collaboration *
 */

/**
 *@brief A few parameters to describe response. Only very roughly
 *tuned to give results which are not entirely unreasonable.
 */
namespace I3DummyPhotonics {
const double B = 1./22.;  // Light attenuation
const double A = 2.1;     // Scaling factor for photon yield of muons
const double C = 0.016;   // Scaling factor for photon yield of cascades
const double TAU = 400;   // tail time constant of residual times
const double T0SLOPE = 8; // Residual time peak increase with distance
}

#ifdef PHOTONICS_SERVICE_ROOT
#include "TObject.h" 
#endif

#include "photonics-service/I3PhotonicsService.h"


/** 
 *@brief Dummy version of I3PhotonicsService (does not use photonics) 
 *
 *Uses some simple eyeball parametrisations based on looking at a few event
 *displays. Light yield falls off exponentially with some suitable distance measure
 *with some additional suppression behind the source. The time distribution rises
 *linearly and then decays exponentially in a way which depends on distance. 
 *This is not meant for any serious use; please read the code if you want to know the
 *details anyway.
 *
 */ 
class I3DummyPhotonicsService : public I3PhotonicsService 
{
protected:
  #ifndef PHOTONICS_SERVICE_ROOT
  SET_LOGGER("I3DummyPhotonicsService");
  #endif
public:
/** 
 *@brief Constructor
 */
  I3DummyPhotonicsService();
/** 
 *@brief Destructor
 */
  ~I3DummyPhotonicsService(){}

  /**
   *@brief Select source
   *Select source, return light yield, and set time distribution parameters for
   *later calls to timing methods. 
   *@param meanPEs                    Mean number of PEs
   *@param emissionPointDistance      Distance from nominal point of cherenkov point
   *                                  to OM
   *@param geoTime                    "Geometric time"
   *@param source                     Source parameters
   */
  void SelectSource( 
		    double &meanPEs,
		    double &emissionPointDistance,
		    double &geoTime,
		    PhotonicsSource const &source); 

  /**
   *@brief Get residual time for hit (w.r.t direct propagation from nominal 
   *emission point)
   *@param[in]  random      Uniform random number 
   *@param[out] timeDelay   Residual time
   */
  void GetTimeDelay(double random, double &timeDelay);

  /**
   *@brief Get probability density for previously selected OM/source combination
   *@param[out] density       Probability density (ns^-1)for given residual time 
   *@param[in] timeDelay      Residual time
   */

  void GetProbabilityDensity(double &density, double timeDelay);
  double GetIntegratedProbability(double timeDelay);

  bool GetProbabilityQuantiles(double *, double, double *, size_t);

  // Things we don't even try to implement
  bool GetMeanAmplitudeGradient(double *);
  bool GetMeanAmplitudeHessian(double *, double (*)[6]);
  bool GetProbabilityQuantileGradients(double *, double, double (*)[6], size_t);
  bool GetProbabilityQuantileHessians(double *, double, double *, double (*)[6],
     double (*)[6][6], size_t);

private:
  double t0_;       /* Time of maximum probability density (linear rise below, 
                       exponential decrease above) */
  double k_;        /* Slope of linear rise */
  double expNorm_;  /* Constant factor in exponential part */
  double tOffset_;  /* Offset added to residual time for muon tracks which do not
		       reach nominal point of cherenkov emission */

#ifdef PHOTONICS_SERVICE_ROOT
    ///ROOT class definition macro
    ClassDef(I3DummyPhotonicsService,2); ///I3PhotonicsService - access photonics tables
#endif

};

#ifndef PHOTONICS_SERVICE_NO_I3
///Definitions of shared pointers
I3_POINTER_TYPEDEFS(I3DummyPhotonicsService);
#endif

#endif
