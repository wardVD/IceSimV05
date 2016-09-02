#ifndef I3NuGSourceSelector_H
#define I3NuGSourceSelector_H

/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file DiffuseSource.h
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *   @brief Set Diffuse primary source
 */

#include "icetray/I3ConditionalModule.h"
#include "earthmodel-service/EarthModelService.h"
#include "phys-services/I3RandomService.h"
#include "neutrino-generator/utils/Pickup.h"
#include <vector>
#include <map>

namespace nugen {
I3_FORWARD_DECLARATION(Steering); 
I3_FORWARD_DECLARATION(Particle); 
}

class I3NuGSourceSelector : public I3ConditionalModule, 
                            public nugen::Pickup {

 public:

  /**
   *Constructor & destructor
   */
  I3NuGSourceSelector(const I3Context& ctx);
  virtual ~I3NuGSourceSelector() {}

  virtual void Configure();
  virtual void DAQ(I3FramePtr frame);
  virtual void Finish() {}

  void Initialize();

 private:

  I3RandomServicePtr random_;
  boost::shared_ptr<nugen::Steering>  steer_ptr_;

  int           number_of_events_;
  int           eventCounter_;
  std::string   steer_name_;
  double        energy_bias_power_;
  std::vector<double> particle_biases_;
  std::map<I3Particle::ParticleType, double> particle_bias_map_;
  bool          keep_dark_;
  bool          useZeVForm_;

  // kill default constructor etc.
  I3NuGSourceSelector();
  I3NuGSourceSelector(const I3NuGSourceSelector&);
  I3NuGSourceSelector& operator=(const I3NuGSourceSelector&);

  double SelectionWeighter(boost::shared_ptr<nugen::Particle> p); 

  //logging
  SET_LOGGER("I3NuG");

};

I3_POINTER_TYPEDEFS(I3NuGSourceSelector);

#endif 
