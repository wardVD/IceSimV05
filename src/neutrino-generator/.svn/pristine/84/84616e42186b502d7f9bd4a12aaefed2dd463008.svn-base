#ifndef I3NuGINJECTOR_H
#define I3NuGINJECTOR_H

/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file I3NuGInjector.h
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *   K.Hoshina changed the class to a service
 *
 *   @brief I3NuGInjector header file
 *   The header file for I3NuGInjector for 
 *   the I3NeutrinoGenerator Neutrino Event generator Module 
 */

#include "dataclasses/I3Position.h"
#include "dataclasses/I3Direction.h"
#include "dataclasses/I3Map.h"
#include "phys-services/I3RandomService.h"
#include "icetray/I3ServiceBase.h"
#include "icetray/I3Frame.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"

namespace nugen {
I3_FORWARD_DECLARATION(Particle); 
I3_FORWARD_DECLARATION(Steering); 
}

class I3NuGInjector : public I3ServiceBase {

 public:
  /**
   *Constructor
   *@param streeringInfo
   *@param const random
   */
  
  I3NuGInjector(const I3Context &c);
  I3NuGInjector(I3RandomServicePtr random,
                boost::shared_ptr<nugen::Steering> steer,
                const std::string &name = "I3NuGInjector");

  /**
   *Virtual Deconstructor
   */
  virtual ~I3NuGInjector();

  /**
   *Virtual Deconstructor
   */
  virtual void Configure();

  /**
   * Generate initial injection neutrino.
   * It may fail to generate a neutrino
   * if a chosen neutrino doesn't path through 
   * the Earth (with Ice).
   * @return bool flag
   */
  bool InjectInitialNeutrino(I3FramePtr frame,
                             I3MapStringDoublePtr wmap);

  /**
   *Get injected neutrino
   */
  boost::shared_ptr<nugen::Particle> GetIncidentNeutrino(){ return injected_neutrino_ptr_;};  

  // setters for injected particle
  void SetInitialNuFlavor(const std::string &s) {nu_flavor_ = s;}
  void SetPowerLawIndex(double g)     {gamma_ = g;}
  void SetFluxEnergyMinLog(double e)  {energy_min_log_ = e;}
  void SetFluxEnergyMaxLog(double e)  {energy_max_log_ = e;}
  void SetFluxZenithMin(double z)     {zenith_min_ = z;}
  void SetFluxZenithMax(double z)     {zenith_max_ = z;}
  void SetFluxAzimuthMin(double a)    {azimuth_min_ = a;}
  void SetFluxAzimuthMax(double a)    {azimuth_max_ = a;}
  void SetSimulatePointSource(bool b) {simulate_pointsource_ = b; }
  void SetPointSourceDirName(const std::string &b) {pointsource_name_ = b; }
  void SetPointSourceZenith(double z) {point_zenith_ = z; }
  void SetPointSourceAzimuth(double z) {point_azimuth_ = z; }

  // getters for injected particle
  const std::string GetInitialNuFlavor()   const {return nu_flavor_;}
  const double GetPowerLawIndex()     const {return gamma_;}
  const double GetFluxEnergyMinLog()  const {return energy_min_log_;}
  const double GetFluxEnergyMaxLog()  const {return energy_max_log_;}
  const double GetFluxZenithMin()     const {return zenith_min_;}
  const double GetFluxZenithMax()     const {return zenith_max_;}
  const double GetFluxAzimuthMin()    const {return azimuth_min_;}
  const double GetFluxAzimuthMax()    const {return azimuth_max_;}

  /**
   *Getters for local Point source simulation:
   **/
  const bool   GetSimulatePointSource()     const {return simulate_pointsource_;}
  const double GetPointSourceZenith()       const {return point_zenith_;}
  const double GetPointSourceAzimuth()      const {return point_azimuth_;}
  const double GetPointSourceZenithSigma()  const {return point_zenith_sigma_;}
  const double GetPointSourceAzimuthSigma() const {return point_azimuth_sigma_;}

  private:

  std::string nu_flavor_;
  double gamma_;
  double energy_min_log_;
  double energy_max_log_;
  double zenith_min_;
  double zenith_max_;
  double azimuth_min_;
  double azimuth_max_;
  bool   simulate_pointsource_;
  std::string pointsource_name_;
  double point_zenith_;
  double point_zenith_sigma_;
  double point_azimuth_;
  double point_azimuth_sigma_;
  double zenith_weight_param_;  // control zenith weight
  std::string zenith_sampling_string_;
  nugen::AngleSamplingMode zenith_sampling_;

  /**
   * Interface pointer to steering information
   * For common random number access throughout module
   */
  I3RandomServicePtr        random_;
  std::string                  steeringName_;
  boost::shared_ptr<nugen::Steering>  steer_;

  /* initial neutrino pointer */
  boost::shared_ptr<nugen::Particle> injected_neutrino_ptr_;

  /**
   * Decide functions for injected neutrino using steering information
   * and fill it
   */
  bool DecideFlavor(boost::shared_ptr<nugen::Particle> p);
  bool DecideEnergy(boost::shared_ptr<nugen::Particle> p);
  bool DecideDirection(boost::shared_ptr<nugen::Particle> p);

  /**
   * This function decides intersection position to the detection volume
   * calculate its entrance and exit position to/from the Earth
   * need to be called after DecideDirection()
   */
  bool DecidePositions(boost::shared_ptr<nugen::Particle> p);

  void GetStartPosDCCandidate(boost::shared_ptr<nugen::Particle> neu_ptr,
                              I3Position &intersectPosDC, 
                              I3Direction &nuDir);

  //logging
  SET_LOGGER("I3NuG");
};
////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<I3NuGInjector> I3NuGInjectorPtr;



#endif //I3NuGINJECTOR_H
