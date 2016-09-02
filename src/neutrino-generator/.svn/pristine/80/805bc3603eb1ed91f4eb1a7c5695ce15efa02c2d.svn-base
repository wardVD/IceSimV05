#ifndef NuGINTERACTIONCC_H
#define NuGINTERACTIONCC_H

/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file InteractionCC.h
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief InteractionCC header file
 *   Standard Model cross-sections derived from sigma
 */


namespace nugen {

I3_FORWARD_DECLARATION(InteractionBase);
I3_FORWARD_DECLARATION(Steering);
I3_FORWARD_DECLARATION(Particle);

class InteractionCC : public InteractionBase {

 public:
  InteractionCC(I3RandomServicePtr random, 
                boost::shared_ptr<Steering> steer);
  virtual ~InteractionCC();

  //overload abstract function
  virtual void FillDaughterParticles(boost::shared_ptr<Particle> particle_ptr,
                                     double energy = -1);

  inline InteractionType GetInteractionType() { return nugen::CC; }

 private:
  InteractionCC() : InteractionBase() {}
  SET_LOGGER("I3NuG");

};
I3_POINTER_TYPEDEFS(InteractionCC);

}
#endif //I3NuGINTERACTIONCC_H


