/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief Interaction header file
 *
 */
///////////////////////////////////////////////////////////////////////////// 
#ifndef NuGTAUDECAY_H
#define NuGTAUDECAY_H

#include "phys-services/I3RandomService.h"

namespace nugen {

I3_FORWARD_DECLARATION(InteractionBase);
I3_FORWARD_DECLARATION(DecayStateTableReader);
I3_FORWARD_DECLARATION(Particle);

class TauDecay : public InteractionBase {

 public:
  TauDecay(I3RandomServicePtr random,
           boost::shared_ptr<Steering> steer);
  virtual ~TauDecay();

  /**
   * read decay table (override function)
   */
  virtual void InitializeFinalStateTable(const std::string& finalfile);

  /**
   * TauDecay doesn't have cross section file. return -1
   * (override function)
   */
  virtual double GetXsecCGS (const double energy) { return -1; }

  virtual void FillDaughterParticles(boost::shared_ptr<Particle> tau, double decay_energy);

  virtual InteractionType GetInteractionType() { return nugen::DECAY; }
  std::string& GetTauDecayFileName(){return decayname_;};
           
 private:
  boost::shared_ptr<DecayStateTableReader> tau_final_ptr;  
  std::string decayname_;

  SET_LOGGER("I3NuG");

 };

I3_POINTER_TYPEDEFS(TauDecay);

}
#endif //TAUDECAY_H




