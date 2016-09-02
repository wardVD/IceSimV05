#ifndef NuGSourceUtils_H
#define NuGSourceUtils_H
/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 * @brief SourceUtils header file
 * 
 */

#include "icetray/I3Frame.h"
#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/I3Map.h"
#include "dataclasses/I3Position.h"
#include "phys-services/I3RandomService.h"
#include "earthmodel-service/EarthModelService.h"
#include <string>
#include <map>

namespace nugen {

I3_FORWARD_DECLARATION(Steering); 
I3_FORWARD_DECLARATION(Particle); 

namespace SourceUtils{

  boost::shared_ptr<Particle>
  GeneratePrimaryNeutrino(const std::map<I3Particle::ParticleType, double> &pmap,
                          I3RandomServicePtr rand, 
                          boost::shared_ptr<Steering> steer) ;


  bool SetPowerLowEnergy(boost::shared_ptr<Particle> nu, 
                         double gamma, 
                         double eminLog, double emaxLog,
                         I3RandomServicePtr random);

  bool SetNuPositions(boost::shared_ptr<Particle> nu_ptr,
                      earthmodel::EarthModelServicePtr earth,
                      I3RandomServicePtr  random,
                      boost::shared_ptr<Steering> steer) ;

  I3Position  
  SelectRandomPos(boost::shared_ptr<Particle> nu_ptr,
                  I3RandomServicePtr random,
                  boost::shared_ptr<Steering> steer) ;


  void FillPrimaryInfo(boost::shared_ptr<Particle> nu,
                       I3MapStringDoublePtr map);

  void FillDiffusePrimaryGenInfo(double gannmaIndex,
                                     double energyMinLog,
                                     double energyMaxLog,
                                     double zenithMin,
                                     double zenithMax,
                                     double azimuthMin,
                                     double azimuthMax,
                                     I3MapStringDoublePtr wmap);


  void FillPointPrimaryGenInfo(double gammaIndex,
                               double energyMinLog,
                               double energyMaxLog,
                               double zenith,
                               double zenithSigma,
                               double azimuth,
                               double azimuthSigma,
                               I3MapStringDoublePtr wmap);


}
}

#endif 
