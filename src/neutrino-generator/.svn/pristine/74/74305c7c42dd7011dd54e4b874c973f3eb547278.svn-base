#ifndef I3NEUTRINOGENERATOR_BASE_H
#define I3NEUTRINOGENERATOR_BASE_H
/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 * @brief I3NeutrinoGenerator header file
 * This class fills dataclasses with the event information 
 * obtained from the neutrino-generator simulation based on ANIS
 * 
 */

#include "phys-services/I3RandomService.h"
#include "dataclasses/physics/I3MCTree.h"
#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "neutrino-generator/legacy/I3NuGInjector.h"
#include "neutrino-generator/legacy/I3NuGVEvent.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"
#include "earthmodel-service/EarthModelService.h"

namespace nugen {
I3_FORWARD_DECLARATION(Steering);
I3_FORWARD_DECLARATION(Particle);
}

class I3NeutrinoGeneratorBase
{

 public:
  I3NeutrinoGeneratorBase();
  virtual ~I3NeutrinoGeneratorBase() {}
  
  void Init();

  /**
   * Propagation functions
   */
  void PropagateAll(boost::shared_ptr<nugen::Particle> p, I3FramePtr frame);
  I3Map<I3ParticleID, double> PropagateInEarth(boost::shared_ptr<nugen::Particle> p, 
                                  I3FramePtr frame, 
                                  bool save_result = true);

  /**
   * This function is used for pybinding test.
   */
  I3Map<I3ParticleID, double> PropagateInEarthWrapper(I3ParticlePtr p, I3FramePtr frame);

  void SetRandomNumberGenerator(I3RandomServicePtr random);
  void SetPropagationMode(nugen::PropagationMode m) { prop_mode_ = m; }
  nugen::PropagationMode GetPropagationMode() const { return prop_mode_; }
  void SetSkipFinalInteraction(bool b) { skip_final_interaction_ = b; }
  bool SkipFinalInteraction() const { return skip_final_interaction_; }

 private:
  /**
   *private constructors 
   */
  I3NeutrinoGeneratorBase& operator=(const I3NeutrinoGeneratorBase&);

 protected:
  //// Following 4 members are common through out the run ////
  /**
   * number of generated events in a run
   * since icetray does not know how many events are going to be generated
   * and since this number is useful to calculate the Oneweight, we input
   * it as a parameter.
  **/
  long number_of_events_;
  /**
   *Number of events so far injected in a run
   *this is the EventID stored in I3EventHeader
   */
  long eventCounter_;

  /**
   *Decide if any memory info printed
   */
  bool doPrintMemoryInfo_;

  /**
   * Core function to propagate neutrino
   * @return I3NuGVEventPtr
   */
  std::pair<bool, I3NuGVEventPtr>
  PropagateNu(boost::shared_ptr<nugen::Particle> injected_nu,
                             I3FramePtr frame, 
                             bool skip_final_interaction=false);

  /**
   * fill MCTree and MCWeightDict
   * @return void
   */
  void  FillMC(I3NuGVEventPtr, I3FramePtr);

 /**
   * Check function
   * @return void
   * @param  I3MCTreePtr
   */
  void  CheckInIceParticle(I3MCTreePtr);

  unsigned int event_year_;
  unsigned int event_daqtime_;
  unsigned int runid_;

  I3RandomServicePtr random_;
  nugen::SteeringPtr steer_;
  earthmodel::EarthModelServicePtr earth_;
  I3NuGInjectorPtr injector_;
  I3NuGInteractionInfoPtr interaction_;
  std::string steering_name_;
  std::string injector_name_;
  std::string interaction_name_;
  std::string pointSourceDirectionName_;
  std::string inice_tree_name_;
  std::string primary_nu_name_;

  double interaction_cc_factor_; // interaction weighting factor for CC
  double interaction_nc_factor_; // interaction weighting factor for NC
  double interaction_gr_factor_; // interaction weighting factor for GR

  nugen::PropagationMode prop_mode_; // do you want to use weighted evt class?
  int    interact_pos_opt_; // control interaction weight calculation
  int    interact_weight_opt_; // control interaction weight calculation
  int    crosssection_columndepth_opt_; // control crosssection *
                                        // columndepth calculation
  int    impactparam_opt_;  // control impact param calculation
  bool   skip_final_interaction_; // do you want to skip final interaction?


};

#endif //I3NEUTRINOGENERATOR_BASE_H
