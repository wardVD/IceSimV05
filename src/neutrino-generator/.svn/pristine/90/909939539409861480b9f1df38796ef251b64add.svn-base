#ifndef I3NuGVEVENT_H
#define I3NuGVEVENT_H

/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file I3NuGVEvent.h
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Kotoyo Hoshina 
 *
 *   @brief interface pure virtual class for event classes
 *   all event classes must inherit it.
 */

#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/legacy/I3NuGInjector.h"
#include "neutrino-generator/utils/Utils.h"
#include "neutrino-generator/utils/Calculator.h"
#include "dataclasses/I3Map.h"
#include "icetray/I3Frame.h"

class I3NuGVEvent {

 public:
  
  I3NuGVEvent(): eventCounter_(0) {}
  virtual ~I3NuGVEvent() {}

  virtual bool MakeNeutrinoPropagation(nugen::ParticlePtr initial, 
                                       bool skipFinalInteraction = false) = 0;
  virtual void WhoAmI() { std::cerr << "I'm I3NuGVEvent" << std::endl; }
  virtual void SetEventCount(int i) { eventCounter_ = i; }
  //virtual void SetInjector(I3NuGInjectorPtr i) { injector_ptr_ = i; }

  //
  //Accessing Functions to the members
  //

  /**
   * Get ParticleVect of particle created during propagation
   */
  virtual const nugen::ParticlePtrList&  GetInEarthPropagatingParticleList() const { return propagating_list_;};
  virtual nugen::ParticlePtrList&        GetInEarthPropagatingParticleList() { return propagating_list_;};

  const nugen::ParticlePtrList& GetFinalInteractionNuCandidates() const
         { return finalInteractionNuCandidates_; }
  nugen::ParticlePtrList& GetFinalInteractionNuCandidates()
         { return finalInteractionNuCandidates_; }

  /**
   *Get final charged leptons which will propagate in final volume
   */
  virtual const nugen::ParticlePtr GetInIceParticle() const { return inice_ptr_;}
  virtual nugen::ParticlePtr       GetInIceParticle()       { return inice_ptr_;}

  /*
   * Getter for buffer_list_
   */
   const nugen::ParticlePtrList& GetPossibleParentFinalParticleList() const { return buffer_list_;};
   nugen::ParticlePtrList& GetPossibleParentFinalParticleList() { return buffer_list_;};

  virtual const I3MapStringDoublePtr GetMCWeightDictPtr() const { return weightdict_ptr_; }
  virtual I3MapStringDoublePtr GetMCWeightDictPtr() { return weightdict_ptr_; }

  // setters
  void SetInIceParticle(nugen::ParticlePtr ptr){ptr->SetLocationType(I3Particle::InIce); inice_ptr_=ptr; }
  void SetMCWeightDictPtr(I3MapStringDoublePtr ptr){weightdict_ptr_=ptr;}

  virtual void FillMCWeights(I3FramePtr frame);

  //need to implement
  virtual void PrintEvent() = 0;
  virtual int  GetTotalNumberOfInEarthDaughters() = 0;
  virtual void CheckFilledMCWeights() = 0;

  protected:

  int eventCounter_;
  I3MapStringDoublePtr  weightdict_ptr_;

  nugen::ParticlePtrList finalInteractionNuCandidates_;

  /**
   * In NuGen "InIceParticle" represents a final neutrino,
   * but not a charged particle
   */
  boost::shared_ptr<nugen::Particle>   inice_ptr_;             
  boost::shared_ptr<nugen::Steering>   steer_ptr_;
  nugen::ParticlePtrList      propagating_list_;          
  nugen::ParticlePtrList      buffer_list_;            

};

typedef boost::shared_ptr<I3NuGVEvent> I3NuGVEventPtr;

#endif //I3NuGVEVENT_H
