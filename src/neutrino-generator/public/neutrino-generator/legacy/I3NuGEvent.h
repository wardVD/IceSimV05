#ifndef I3NuGEVENT_H
#define I3NuGEVENT_H

/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file I3NuGEvent.h
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief I3NuGEvent header file
 *   The header file for I3NuGEvent for 
 *   the I3NeutrinoGenerator Neutrino Event generator Module 
 *   final charged leptons which will be passed to MMC for further 
 *   propagation or to PSI for photon creation 
 */

#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "neutrino-generator/legacy/I3NuGVEvent.h"
#include "dataclasses/I3Map.h"
#include "icetray/I3Frame.h"
#include "phys-services/I3RandomService.h"
#include "earthmodel-service/EarthModelService.h"

//typedef I3Vector<nugen::ParticlePtr> I3NuGParticlePtrList;
//typedef I3MapStringDoublePtr I3MCWeightDictPtr;

namespace nugen {
I3_FORWARD_DECLARATION(Particle); 
I3_FORWARD_DECLARATION(Steering); 
}

class I3NuGEvent : public I3NuGVEvent {

 public:
  /**
   *Constructor
   *@param streeringInfo
   *@param interactionInfo
   *@param const random
   */
  
  I3NuGEvent(boost::shared_ptr<nugen::Steering> steer_p,
             I3NuGInteractionInfoPtr inter_p,
	     I3RandomServicePtr random);
  /**
   *Virtual Deconstructor
   */
  virtual ~I3NuGEvent();

  /**
   * 
   */
  void WhoAmI() { std::cerr << "I'm I3NuGEvent" << std::endl; }

  bool MakeNeutrinoPropagation(nugen::ParticlePtr initial, 
                               bool skipFinalInteraction = false);
  /**
   *
   */
  void PrintEvent();
  int GetTotalNumberOfInEarthDaughters();
  void CheckFilledMCWeights();

  /**
   * Check function if this is CREATED in Detection volume
   * Defined by semi-sphears of radiusm active length before and after
   */
  bool IsStartInExpectedDetectionVolume(const nugen::Particle &p);

 private:

  /**
   * Interface pointer to crosssection file information
   * For common random number access throughout module
   */
  I3RandomServicePtr       random_;
  I3NuGInteractionInfoPtr  interaction_ptr_;
  earthmodel::EarthModelServicePtr     earthservice_ptr_;

  bool PropagateNeutrino(nugen::ParticlePtr);

  void SetFinalInteractionPosition(nugen::ParticlePtr particle_ptr);

  void MakeFinalInteraction(nugen::ParticlePtr neu_part);
  bool MakeTauDecay(nugen::ParticlePtr tau_ptr);
  nugen::ParticlePtr ChooseNeutrinoToInteractFromBuffer();
  double CalculateMaximumTotalInteractionProbability(nugen::ParticlePtr particle_ptr);  

  void CheckAndPushBack_Daughters(nugen::ParticlePtr part);

  //logging
  SET_LOGGER("I3NuG");
};
////////////////////////////////////////////////////////////////////////////
typedef boost::shared_ptr<I3NuGEvent> I3NuGEventPtr;

#endif //I3NuGEVENT_H
