#ifndef NuGINTERACTIONBase_H
#define NuGINTERACTIONBase_H
/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *
 *   @version $Revision: $
 *   @date $Date: $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief Interaction header file for I3NeutrinoGenerator
 */
//////////////////////////////////////////////////////////////////////

#include "dataclasses/I3Direction.h"
#include "dataclasses/physics/I3Particle.h"
#include "phys-services/I3RandomService.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"

#include <map>

namespace nugen {
 
I3_FORWARD_DECLARATION(Steering);
I3_FORWARD_DECLARATION(Particle);
I3_FORWARD_DECLARATION(CrosssectionTableReader);
I3_FORWARD_DECLARATION(FinalStateTableReader);

class InteractionBase {

 protected:
  I3RandomServicePtr  random_;
  boost::shared_ptr<Steering> steer_;
  boost::shared_ptr<CrosssectionTableReader> cross_file_ptr_;
  boost::shared_ptr<FinalStateTableReader>   final_file_ptr_;
  std::string sigmaname_;
  std::string finalname_;
  unsigned int flavormask_;
  unsigned int materialmask_;

  // default constructor
  InteractionBase() {}


 public:
   InteractionBase(I3RandomServicePtr random, 
                   boost::shared_ptr<Steering> steer);
   virtual ~InteractionBase();

   /**	
    * pure virtual function	
    */
   virtual void FillDaughterParticles(
                  boost::shared_ptr<Particle> particle_ptr, 
                  double energy = 0) = 0;
   virtual InteractionType GetInteractionType() = 0;


   virtual void InitializeCrosssectionTable(const std::string& sigmafile);
   virtual void InitializeFinalStateTable(const std::string& finalfile);

   /**
    @brief returns cross section of this interaction in [cm^2]
    */
   virtual double GetXsecCGS (const double energy);

   std::string&  GetCrosssectionFileName() {return sigmaname_;};
   std::string&  GetFinalStateFileName()   {return finalname_;};
   unsigned int  GetActiveFlavorMask()     {return flavormask_;};
   unsigned int  GetActiveMaterialMask()   {return materialmask_;};

   void SetActiveFlavorMask(unsigned int mask)   {flavormask_ = mask;};
   void SetActiveMaterialMask(unsigned int mask) {materialmask_ = mask;};
        

   I3Direction GetLeptonDirection(const I3Direction& direction, 
                                  const double lepton_energy, 
                                  const double cos_theta);

   std::vector<double> SelectXY(double log_e);

   /**
    * @return energy of outgoing lepton
    */
   double SetSecondaryLepton(boost::shared_ptr<Particle> nuin_ptr,
                        I3Particle::ParticleType leptonFlavor,
                        double lepton_mass, 
                        bool skipCalcCosTheta = false);

   double CalcOutgoingCosThetaSimple(double ene,
                                     double x, double y) ;

   double CalcOutgoingCosTheta(double ene, double lepton_mass,
                               double x, double y) ;

   SET_LOGGER("I3NuG");
};

I3_POINTER_TYPEDEFS(InteractionBase);
typedef std::pair<double, InteractionBasePtr> DoubleInteractionPair;
typedef std::map<double, InteractionBasePtr> DoubleInteractionMap;

}
#endif //I3NuGINTERACTION_H


