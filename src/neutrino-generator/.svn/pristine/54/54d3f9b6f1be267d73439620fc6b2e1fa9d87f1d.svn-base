#ifndef I3NuGPointSource_H
#define I3NuGPointSource_H

/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file PointSource.h
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *           modified from Aya's original 
 *
 *   @brief Set Point primary source
 */

#include "icetray/I3ConditionalModule.h"
#include "earthmodel-service/EarthModelService.h"
#include "phys-services/I3RandomService.h"
#include <vector>

namespace nugen {
I3_FORWARD_DECLARATION(Particle); 
I3_FORWARD_DECLARATION(Steering); 
}

class I3NuGPointSource : public I3ConditionalModule {

 public:

  /**
   *Constructor & destructor
   */
  I3NuGPointSource(const I3Context& ctx);
  virtual ~I3NuGPointSource() {}

  virtual void Configure();
  virtual void DAQ(I3FramePtr frame);
  virtual void Finish() {}

 private:

  bool SetDirection(boost::shared_ptr<nugen::Particle> nu);

  I3RandomServicePtr random_;

  int           number_of_events_;
  int           eventCounter_;
  std::string   steer_name_;
  std::string   outNuName_;
  double        gammaIndex_;
  double        energyMinLog_;
  double        energyMaxLog_;
  std::string   sourceName_;
  double        zenith_;
  double        azimuth_;
  double        zenithSigma_;
  double        azimuthSigma_;
  std::string   flavor_;
  std::vector<double> nunubar_ratio_vec_;
  std::map<I3Particle::ParticleType, double> type_map_;

  boost::shared_ptr<nugen::Steering> steer_ptr_;
  earthmodel::EarthModelServicePtr   earth_ptr_;

  /* initial neutrino pointer */
  boost::shared_ptr<nugen::Particle> injected_ptr_;

  // kill default constructor etc.
  I3NuGPointSource();
  I3NuGPointSource(const I3NuGPointSource&);
  I3NuGPointSource& operator=(const I3NuGPointSource&);

  //logging
  SET_LOGGER("I3NuG");

};
I3_POINTER_TYPEDEFS(I3NuGPointSource);

#endif 
