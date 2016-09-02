#ifndef NuGINTERACTIONGR_H
#define NuGINTERACTIONGR_H

/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file InteractionGR.h
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief InteractionGR header file
 *   Standard Model cross-sections derived from sigma
 */

namespace nugen {

I3_FORWARD_DECLARATION(InteractionBase);
I3_FORWARD_DECLARATION(Steering);
I3_FORWARD_DECLARATION(Particle);

class InteractionGR : public InteractionBase {

////////////////////////////////////////////////////////////////////////
// Glashow resonance
// This is also an example of a non-table parametrization
////////////////////////////////////////////////////////////////////////

  public:   

   enum GRDecayChannel { 
  	ELECTRON_GR=1,	
        MUON_GR=2,
  	TAU_GR=3,
  	HADRON_GR=4,
	NOTSET_GR=0};


   InteractionGR(I3RandomServicePtr random,
                boost::shared_ptr<Steering> steer);
   virtual ~InteractionGR();

   /**
    * Get Crosssection (override function)
    */
   virtual double GetXsecCGS(const double energy);

   virtual void FillDaughterParticles(boost::shared_ptr<Particle> particle_ptr,
                                      double energy = -1);

   inline InteractionType GetInteractionType() {return nugen::GR;}

  private:
   GRDecayChannel ChooseChannel();
   void FillResonanceStates(double fstate[]);

   SET_LOGGER("I3NuG");
};

I3_POINTER_TYPEDEFS(InteractionGR);

}

#endif //I3NuGINTERACTIONGR_H

