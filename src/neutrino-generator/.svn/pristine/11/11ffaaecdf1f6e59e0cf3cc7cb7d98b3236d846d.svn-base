#ifndef NuGNuGInteractionManager_H
#define NuGNuGInteractionManager_H
/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *
 *   @version $Revision: $
 *   @date $Date: $
 *   @author Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *   @brief Map of interactions (per neutrino type)
 */

namespace nugen {

I3_FORWARD_DECLARATION(InteractionManagerBase);
I3_FORWARD_DECLARATION(InteractionBase);
I3_FORWARD_DECLARATION(Steering);
I3_FORWARD_DECLARATION(Particle);
I3_FORWARD_DECLARATION(TauDecay);

class NuGInteractionManager : public InteractionManagerBase
{

 public:

   NuGInteractionManager(const I3Context &c);

   NuGInteractionManager(I3RandomServicePtr rand, 
                         boost::shared_ptr<nugen::Steering> steer,
                         const std::string &model = "csms",
                         const std::string &path = "",
                         const std::string &name = "NuGInteractionManager");

   virtual ~NuGInteractionManager() {}

   /**
    * configure
    */
   void Configure();

   /**
    @brief Generate all interaction drivers and initialize them
    */
   virtual void Initialize();

   /**
    @brief returns default interaction factor
    */
   virtual const InteractionTypeDoubleMap & 
   GetDefaultInteractionFactors() { return default_intfactors_; }

   /**
    @brief calculate total cross section
    @param[in] ptype particle type
    @param[in] energy neutrino energy
    @param[in] intfactors map of scale factor of each interaction
    @param[in] globalfactos map of global scale factor of each interaction
    */
   void CalcTotalXsec(
         const I3Position &pos_CE,
         I3Particle::ParticleType ptype,
         double energy,
         const InteractionTypeDoubleMap & intfactors, 
         const InteractionTypeDoubleMap & globalfactors);

   /**
     @brief setters for cross section files
    */
   void SetCrossSectionModel(const std::string &s) { xsec_model_string_ = s; }

 private:

   /**
     Interaction Driver Buffers
    */
   typedef std::map<int, InteractionBasePtr> MatDrivers;
   typedef std::map<InteractionType, MatDrivers>  IntTypeDrivers;
   typedef std::map<I3Particle::ParticleType, IntTypeDrivers> DriversMap;
   DriversMap drivers_;
   boost::shared_ptr<TauDecay> tau_decay_;
   InteractionTypeDoubleMap default_intfactors_;

   /**
     File names of Neutrino - proton or neutron interaction 
     total cross section and final interaction(Bjorken x&y) tables
     with tag
    */
   std::string xsec_model_string_;
   std::string steeringName_;
   std::string path_;
   std::map<std::string, std::string> xsecs_;

   SET_LOGGER("I3NuG");

};

I3_POINTER_TYPEDEFS(NuGInteractionManager);

}
#endif //I3NuGINTERACTION_H


