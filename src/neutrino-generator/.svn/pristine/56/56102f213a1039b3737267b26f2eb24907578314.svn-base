#ifndef NuGInteractionManagerBase_H
#define NuGInteractionManagerBase_H
/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *
 *   @version $Revision: $
 *   @date $Date: $
 *   @author Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *   @brief base class of interaction manager
 *    This class inherits boost:::enable_shared_from_this
 *    in order to obtain shared pointer to point the
 *    object itself. 
 *    For details, search "enable_shared_from_this.html"
 */

#include "dataclasses/physics/I3Particle.h"
#include "phys-services/I3RandomService.h"
#include "icetray/I3ServiceBase.h"
#include "boost/enable_shared_from_this.hpp"
#include "neutrino-generator/utils/EnumTypeDefs.h"

namespace nugen {

I3_FORWARD_DECLARATION(InteractionBase);
I3_FORWARD_DECLARATION(Steering);

class InteractionManagerBase : 
      public I3ServiceBase, 
      public boost::enable_shared_from_this<InteractionManagerBase>
{
  protected:

   I3RandomServicePtr random_;
   boost::shared_ptr<Steering> steer_;

   /**
    * buffers
    */
   I3Particle::ParticleType cur_ptype_;
   double cur_energy_;
   InteractionTypeDoubleMap cur_int_factors_;
   InteractionTypeDoubleMap cur_global_factors_;
   earthmodel::EarthModelService::MediumType cur_medium_type_;

   class InteractionBuf 
   {
     public :
        boost::shared_ptr<InteractionBase> interaction_;
        double xsec_;
        double xsec_scaled_;
        double xsec_per_gram_;
        double xsec_per_gram_scaled_;
        double sum_xsec_;
        double sum_xsec_scaled_;
        double sum_xsec_per_gram_;
        double sum_xsec_per_gram_scaled_;
        double scale_weight_;
   };
   typedef std::map<double, InteractionBuf> InteractionBufMap;

   /**
    * Map of double and InteractionBasePtr. 
    * The key double must be accumulated cross section.
    * See example in NuGInteractionManager::CalcTotalXsec.
    */
   InteractionBufMap intbufmap_;
   InteractionBufMap intbufmap_scaled_;

   /**
    * Check if we need to update Xsec calculation
    */
   bool UpdateXsec(earthmodel::EarthModelService::MediumType medtype,
                   I3Particle::ParticleType ptype, double ene,
                   const InteractionTypeDoubleMap & intfactors,
                   const InteractionTypeDoubleMap & globalfactors)
   {
      if (medtype != cur_medium_type_) return true;
      if (ptype != cur_ptype_ || ene != cur_energy_) return true;
      if (intfactors != cur_int_factors_) return true;
      if (globalfactors != cur_global_factors_) return true;
      return false;     
   }

   /**
    @brief calculate total cross section
    @param[in] pos_CE position where you want to calculate xsec, in
               EarthCentered coordinate
    @param[in] ptype particle type
    @param[in] energy neutrino energy
    @param[in] intfactors map of scale factor of each interaction
    @param[in] globalfactos map of global scale factor of each interaction

    In this function, protected maps and buffers listed below must be 
    updated.
    * intbufmap_
    * intbufmap_scaled_
    * cur_ptype_;
    * cur_energy_;
    * cur_int_factors_;
    * cur_global_factors_;
    * cur_medium_type_;
    See example in NuGInteractionManager::CalcTotalXsec.
    */
   virtual void CalcTotalXsec(
         const I3Position &pos_CE,
         I3Particle::ParticleType ptype,
         double energy,
         const InteractionTypeDoubleMap & intfactors, 
         const InteractionTypeDoubleMap & globalfactors) = 0;

 public:

   InteractionManagerBase(const I3Context &c); 

   InteractionManagerBase(I3RandomServicePtr rand, 
                          boost::shared_ptr<nugen::Steering> steer,
                          const std::string &name);

   virtual ~InteractionManagerBase() {}

   /**
    @brief Generate all interaction drivers and initialize them
    */
   virtual void Initialize() = 0;

   /**
    @brief obtain shared pointer to this object
    */
   boost::shared_ptr<InteractionManagerBase> GetSharedPtr();

   /**
    @brief This function must return non-weighted interaction factors.
    * example NuGen : 
    * NuGen has 4 interaction types (CC, NC, GR, DECAY). 
    * Default interaction  factor should be
    * [CC] = 1.0
    * [NC] = 1.0
    * [GR] = 1.0
    * [DECAY] = 1.0
    * see the end of NuGInteractionManager::Initialize().
    * since the function returns reference, the map must be a data member!
    */
   virtual const InteractionTypeDoubleMap & 
                 GetDefaultInteractionFactors() = 0;

   /**
    @brief get total cross section 
     Note that the returned values are not weighted with 
     interaction factors.
    @param[in] pos_CE position where you want to calculate xsec, in
               EarthCentered coordinate
    @param[in] ptype particle type
    @param[in] energy neutrino energy
    @param[in] materialweights map of weight ratio of materials
    @param[in] globalfactos map of global scale factor of each interaction
    @return : pair<totalXsec, totalXsec_per_gram>
   */
   DoubleDoublePair GetTotalXsec(
         const I3Position &pos_CE,
         I3Particle::ParticleType ptype,
         double energy,
         const InteractionTypeDoubleMap & globfactors);

   /**
    @brief select an interaction for given neutrino and position.
    @param[in] pos_CE position of the vertex, in
               EarthCentered coordinate
    @param[in] ptype particle type
    @param[in] energy neutrino energy
    @param[in] materialweights map of weight ratio of materials
    @param[in] intfactors map of scale factor of each interaction
    @param[in] globalfactos map of global scale factor of each interaction
    @param[in] isweighted select a driver with weight or not
    @return : pair<double weight, interaction driver>

    This function takes into account of interaction factors if 
    isweighted parameter is true.
    The returned interaction must be weighted with the returned 
    weight value to get correct frequency of the interaction.
    Global factors are used to scale cross section, and this effect
    is NOT compensated with the returned weight.
    */

   std::pair<double, boost::shared_ptr<InteractionBase> > 
      SelectAnInteraction(
         const I3Position &pos_CE,
         I3Particle::ParticleType ptype,
         double energy,
         const InteractionTypeDoubleMap & intfactors, 
         const InteractionTypeDoubleMap & globalfactors,
         bool isweighted);


   SET_LOGGER("I3NuG");

};

I3_POINTER_TYPEDEFS(InteractionManagerBase);

}
#endif //I3NuGINTERACTION_H


