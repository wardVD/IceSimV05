#ifndef I3NuGWEIGHTEDEVENT_H
#define I3NuGWEIGHTEDEVENT_H

/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file I3NuGWeightedEvent.h
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *   @brief I3NuGWeightedEvent header file
 *   This class is modified version of I3NuGEvent which improves : 
 *   - reduce duplicated code
 *   - precise calculation for muon range
 *   - precise calculation for interaction weight
 *   - allow propagation weight for numu simulation
 *   - follow coding convention
 */

#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "neutrino-generator/legacy/I3NuGVEvent.h"
#include "neutrino-generator/utils/Pickup.h"
#include "dataclasses/I3Map.h"
#include "icetray/I3Frame.h"
#include "phys-services/I3RandomService.h"
#include "earthmodel-service/EarthModelService.h"

namespace nugen {
I3_FORWARD_DECLARATION(Particle); 
I3_FORWARD_DECLARATION(Steering); 
}

class I3NuGWeightedEvent : public I3NuGVEvent, public nugen::Pickup {

 public:

  /**
   *Constructor
   *@param streeringInfo
   *@param interactionInfo
   *@param const random
   */
  I3NuGWeightedEvent(boost::shared_ptr<nugen::Steering> steer_p,
                     I3NuGInteractionInfoPtr inter_p,
                     I3RandomServicePtr random);
  /**
   *Virtual Deconstructor
   */
  virtual ~I3NuGWeightedEvent();

  virtual void WhoAmI() { std::cerr << "I'm I3NuGWeightedEvent" << std::endl; }

  /**
   * Propagate a neutrino from EarthEntrance point to the 
   * end of a visible particle.
   * This is the main function and all private functions are
   * invoked inside this function.
   */
  virtual bool MakeNeutrinoPropagation(nugen::ParticlePtr initial,
                                       bool skipFinalInteraction = false);

  virtual void PrintEvent();

  virtual int  GetTotalNumberOfInEarthDaughters();

  virtual void CheckFilledMCWeights();

  virtual double SelectionWeighter(boost::shared_ptr<nugen::Particle> p);

  /**
   * artifical enhancement factor of each interaction at 
   * final interaction.
   * total interaction weight compensate the effect so that
   * final weight should be correct.
   */
  inline void SetInteractionCCFactor(double f) { interaction_cc_factor_ = f; }
  inline void SetInteractionNCFactor(double f) { interaction_nc_factor_ = f; }
  inline void SetInteractionGRFactor(double f) { interaction_gr_factor_ = f; }
  inline double GetInteractionCCFactor() { return interaction_cc_factor_; }
  inline double GetInteractionNCFactor() { return interaction_nc_factor_; }
  inline double GetInteractionGRFactor() { return interaction_gr_factor_; }

 protected:

  /**
   * Interface pointer to crosssection file information
   * For common random number access throughout module
   */
  I3RandomServicePtr        random_;
  I3NuGInteractionInfoPtr   interaction_ptr_;

  earthmodel::EarthModelServicePtr     earthservice_ptr_;

  /**
   * artificial weight factor for
   * interactions 
   */
  double interaction_cc_factor_;
  double interaction_nc_factor_;
  double interaction_gr_factor_;

  /**
   * Propagate a neutrino from a particle position to the front surface
   * of the detection volume.
   * 
   */
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
typedef boost::shared_ptr<I3NuGWeightedEvent> I3NuGWeightedEventPtr;
#endif //I3NuGEVENT_H
