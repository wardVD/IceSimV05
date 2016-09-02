////////////////////////////////////////////////////////////////////////
#ifndef NuGINTERACTIONNC_H
#define NuGINTERACTIONNC_H

/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file InteractionNC.h
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief InteractionNC header file
 *   Standard Model cross-sections derived from sigma
 */


namespace nugen {

I3_FORWARD_DECLARATION(InteractionBase);
I3_FORWARD_DECLARATION(Steering);
I3_FORWARD_DECLARATION(Particle);

class InteractionNC : public InteractionBase {

 public:
  InteractionNC(I3RandomServicePtr random,
                boost::shared_ptr<Steering> steer);
  virtual ~InteractionNC(); 

  //overload abstract function
  virtual void FillDaughterParticles(boost::shared_ptr<Particle> ptr, 
                                     double energy = -1);

  inline InteractionType GetInteractionType() {return nugen::NC;}

 private:
  SET_LOGGER("I3NuG");
};
I3_POINTER_TYPEDEFS(InteractionNC);

}
#endif //I3NuGINTERACTIONNC_H
