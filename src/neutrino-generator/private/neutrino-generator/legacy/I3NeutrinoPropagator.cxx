/** 
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *           modified by Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *  @brief   I3NeutrinoGenerator IMPLEMENTATION FILE
 */

#include "neutrino-generator/legacy/I3NeutrinoPropagator.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "neutrino-generator/legacy/I3NuGEvent.h"
#include "neutrino-generator/legacy/I3NuGWeightedEvent.h"
#include "neutrino-generator/Particle.h"

// #include "neutrino-generator/legacy/I3NuGInteractionGeo.h"
#include <boost/foreach.hpp>
 
using namespace nugen;
using namespace earthmodel;
using namespace std;

//______________________________________________________________
I3NeutrinoPropagator::I3NeutrinoPropagator(
                       I3RandomServicePtr random, 
                       SteeringPtr steer,
                       I3NuGInteractionInfoPtr interaction)
{
  random_ = random;
  steer_ = steer;
  earth_ = steer_->GetEarthModelService();
  interaction_ = interaction;
}

//______________________________________________________________
void
I3NeutrinoPropagator::SetRandomNumberGenerator(I3RandomServicePtr r)
{
  random_ = r;
  interaction_->SetRandomService(r);
}

//______________________________________________________________
std::vector<I3Particle>
I3NeutrinoPropagator::Propagate(I3Particle& incident_nu, 
                               DiagnosticMapPtr frame)
{
  log_info("Propagate is called for event %ld, type %s, energy 1e%g", 
             eventCounter_, incident_nu.GetTypeString().c_str(), 
             log10(incident_nu.GetEnergy()));
  
  // it's dirty fix, but set propmode to static params
  Init();

  ParticlePtr neutrino(new Particle(incident_nu, steer_));
  
  //
  // copy interaction info from MCWeightDict.
  // first, get copy of MCWeightDict
  //

  string weightname = steer_->GetWeightDictName();
  I3MapStringDoublePtr wmap = 
         Utils::PutWeightDict(I3FramePtr(new I3Frame), weightname);
 
  //
  // then, copy info in MCWeightDict to primary particle
  // these information will be updated in the end of the function.
  InteractionInfo &intinfo = neutrino->GetInteractionInfoRef();
  intinfo.RestoreInteractionInfo(wmap);
  InteractionGeo &intgeo = neutrino->GetInteractionGeoRef();
  intgeo.RestoreInteractionGeo(wmap);

  //
  // Propagate Neutrino.
  // skip_final_interaction should be false
  // NB: since Propagate() doesn't have direct access to the underlying frame,
  // we pass a null pointer here.
  //
  std::pair<bool, I3NuGVEventPtr> result
                      = PropagateNu(neutrino, I3FramePtr(), false);
  bool survived = result.first;
  I3NuGVEventPtr event_ptr = result.second;
 
  // prepare return vector.
  std::vector<I3Particle> daughters;
  if (!survived) {
     ++eventCounter_;
     return daughters;
  }
	
  ParticlePtr inice = event_ptr->GetInIceParticle();
  int count = 0;
  BOOST_FOREACH(I3ParticlePtr p, inice->GetDaughters()) {
     p->SetLocationType(I3Particle::InIce);
     daughters.push_back(*p);

     log_info("+++ Secondary particle %d :type %s, energy 1e%g, location %s  is added", 
             count, (p->GetTypeString()).c_str(), 
             log10(p->GetEnergy()), (p->GetLocationTypeString()).c_str());
     ++count;
  }

  // copy weight info to wmap
  const InteractionInfo &intinfo_inice = inice->GetInteractionInfoRef();
  intinfo_inice.FillInteractionInfo(wmap);
  const InteractionGeo &intgeo_inice = inice->GetInteractionGeoRef();
  intgeo_inice.FillInteractionGeo(wmap);

  ++eventCounter_;
  return daughters;
}

