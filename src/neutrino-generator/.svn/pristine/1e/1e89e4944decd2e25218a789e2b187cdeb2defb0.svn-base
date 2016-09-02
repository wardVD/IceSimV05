/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief Interaction implimentation file
 *
 */
//////////////////////////////////////////////////////////////////////

#include "neutrino-generator/Particle.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/interactions/InteractionManagerBase.h"
#include "neutrino-generator/interactions/InteractionBase.h"
#include "neutrino-generator/table-interface/CrosssectionTableReader.h"
#include "neutrino-generator/table-interface/FinalStateTableReader.h"

using namespace std;
using namespace earthmodel;

namespace nugen {

//_________________________________________________________________
InteractionManagerBase::InteractionManagerBase(const I3Context &c) 
    :I3ServiceBase(c) 
{
}

//_________________________________________________________________
InteractionManagerBase::InteractionManagerBase(
                          I3RandomServicePtr rand, 
                          SteeringPtr steer,
                          const std::string &name)
    :I3ServiceBase(name), random_(rand), steer_(steer)
{
}

//_________________________________________________________________
InteractionManagerBasePtr InteractionManagerBase::GetSharedPtr()
{
   return shared_from_this();
}

//_________________________________________________________________
DoubleDoublePair InteractionManagerBase::GetTotalXsec(
         const I3Position &pos_CE,
         I3Particle::ParticleType ptype,
         double energy,
         const InteractionTypeDoubleMap & globfactors)
{
   CalcTotalXsec(pos_CE, ptype, energy, 
                 GetDefaultInteractionFactors(), 
                 globfactors);

   // buffers are updated. take last key of the buffer
   if (intbufmap_.size()<1) {
      log_error("Buffer is not filled!");
   }
   InteractionBufMap::iterator end = intbufmap_.end();
   InteractionBuf intbuf = (--end)->second; 
   
   DoubleDoublePair apair(intbuf.sum_xsec_, 
                          intbuf.sum_xsec_per_gram_);
   return apair;
}

//_________________________________________________________________
DoubleInteractionPair 
InteractionManagerBase::SelectAnInteraction(
              const I3Position &pos_CE,
              I3Particle::ParticleType ptype,
              double ene,
              const InteractionTypeDoubleMap & intf, 
              const InteractionTypeDoubleMap & globf,
              bool isweighted)
{
   DoubleInteractionPair intpair;

   // calculate total xsec etc.
   CalcTotalXsec(pos_CE, ptype, ene, 
                 intf, globf);

   // get unweighted total cross section 
   if (!isweighted) {

      if (intbufmap_.size()<1) {
         log_error("Buffer is not filled!");
      }
      InteractionBufMap::iterator end = intbufmap_.end();
      InteractionBuf lastintbuf = (--end)->second; 
    
      double totXsec_per_gram = lastintbuf.sum_xsec_per_gram_;
      double xsec_per_gram = random_->Uniform() * totXsec_per_gram;
      InteractionBufMap::iterator i = 
                     intbufmap_.lower_bound(xsec_per_gram);
      InteractionBuf buf = i->second;
      intpair.first = 1.0; // unweighted
      intpair.second = buf.interaction_; // pointer to interaction
      return intpair;

   }
      
   // else, we must take into account interaction scale.

   if (intbufmap_scaled_.size()<1) {
      log_error("Buffer is not filled!");
   }
   InteractionBufMap::iterator end = intbufmap_scaled_.end();
   InteractionBuf lastintbuf = (--end)->second; 

   double totXsec_per_gram_scaled = lastintbuf.sum_xsec_per_gram_scaled_;

   double xsec_per_gram_scaled = random_->Uniform() 
                                 * totXsec_per_gram_scaled;     
   InteractionBufMap::iterator i = 
        intbufmap_scaled_.lower_bound(xsec_per_gram_scaled);
   InteractionBuf buf = i->second;
   intpair.first = buf.scale_weight_; // weight 
   intpair.second = buf.interaction_; // pointer to interaction
   return intpair;
}

}
