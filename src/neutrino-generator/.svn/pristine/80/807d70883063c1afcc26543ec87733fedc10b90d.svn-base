/** 
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *           modified by Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *  @brief   I3NeutrinoGenerator IMPLEMENTATION FILE
 */

#include "icetray/I3Tray.h"
#include "icetray/I3TrayHeaders.h"

#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "dataclasses/I3Map.h"
#include "neutrino-generator/legacy/MemoryInfo.h"
#include "neutrino-generator/legacy/StaticParams.h"
#include "neutrino-generator/legacy/StaticParamsSetter.h"
//I3NeutrinoGenerator classes 5 basic classes and one utility class
#include "neutrino-generator/legacy/I3NeutrinoGenerator.h"
#include "neutrino-generator/legacy/I3NuGEvent.h"
#include "neutrino-generator/legacy/I3NuGWeightedEvent.h"
//#include "neutrino-generator/legacy/I3NuGBenchmarkEvent.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/utils/Utils.h"
#include "neutrino-generator/utils/Defaults.h"

#include "neutrino-generator/utils/TreeUtils.h"

#include <boost/foreach.hpp>
 
using namespace nugen;
using namespace earthmodel;
using namespace std;

//______________________________________________________________
I3NeutrinoGeneratorBase::I3NeutrinoGeneratorBase() :
  //counter
  number_of_events_(0),
  eventCounter_(0),
  doPrintMemoryInfo_(false),

  event_year_(1000), 
  event_daqtime_(1000), 
  runid_(10),

  // parameters below except for prop_mode_
  // are controlled by steering service
  // use set method to change prop_mode
  steering_name_("NuGSteering"),
  injector_name_("I3NuGInjector"),
  interaction_name_("I3NuGInteractionInfo"),
  inice_tree_name_(""),
  primary_nu_name_(Defaults::primaryNuName),
 
  //cross section scale factors
  interaction_cc_factor_(1.0),
  interaction_nc_factor_(1.0),
  interaction_gr_factor_(1.0),

  //propagation and weight modes
  prop_mode_(nugen::AUTODETECT),
  interact_pos_opt_(1),
  interact_weight_opt_(1),
  crosssection_columndepth_opt_(0),
  impactparam_opt_(0),
  skip_final_interaction_(false)
{}

//______________________________________________________________
void I3NeutrinoGeneratorBase::Init()
{ 
  // set static params. will be cleaned up in future...
  StaticParams::gPROPAGATION_MODE    = prop_mode_;  
  StaticParams::gINTPOS_SAMPLE_OPT   = interact_pos_opt_;  
  StaticParams::gINTERACT_WEIGHT_OPT = interact_weight_opt_;  
  StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT = crosssection_columndepth_opt_;  
  StaticParams::gIMPACTPARAM_OPT     = impactparam_opt_;  
}

//______________________________________________________________
I3Map<I3ParticleID, double>
I3NeutrinoGeneratorBase::PropagateInEarthWrapper(I3ParticlePtr injected_nu, 
                                      I3FramePtr frame) 
{
  // it's dirty fix, but set propmode to static params
  Init();

  ParticlePtr neutrino(new Particle(*injected_nu, steer_));
  
  //
  // copy interaction info from MCWeightDict.
  // first, get copy of MCWeightDict
  //

  string weightname = steer_->GetWeightDictName();
  I3MapStringDoublePtr wmap = 
         Utils::PutWeightDict(frame, weightname);
 
  //
  // then, copy info in MCWeightDict to primary particle
  // these information will be updated in the end of the function.
  //

  InteractionInfo &intinfo = neutrino->GetInteractionInfoRef();
  intinfo.RestoreInteractionInfo(wmap);
  InteractionGeo &intgeo = neutrino->GetInteractionGeoRef();
  intgeo.RestoreInteractionGeo(wmap);

  return PropagateInEarth(neutrino, frame, false);

}

//______________________________________________________________
std::pair<bool, I3NuGVEventPtr>
I3NeutrinoGeneratorBase::PropagateNu(ParticlePtr injected_nu, 
                                   I3FramePtr frame,
                                   bool skip_final_interaction)
{
  log_info("Propagate is called for event %ld, type %s, energy 1e%g", 
             eventCounter_, injected_nu->GetTypeString().c_str(), 
             log10(injected_nu->GetEnergy()));

  log_debug(" check injected neutrino");
  injected_nu->CheckParticle();

  I3NuGVEventPtr event_ptr;

  if (prop_mode_ == nugen::LEGACY) {
     log_debug(" propagation mode Legacy is selected. Generate I3NuGEvent");
     event_ptr = I3NuGEventPtr(new I3NuGEvent(steer_,
                                              interaction_, 
                                              random_)); 
     log_debug(" I3NuGEvent successfully generated");

  } else {

     log_debug(" propagation mode NoWeight etc. is selected. Generate I3NuGWeightedEvent");
     I3NuGWeightedEventPtr e_ptr = 
          I3NuGWeightedEventPtr(new I3NuGWeightedEvent(
                                    steer_,
                                    interaction_, 
                                    random_));
     e_ptr->SetInteractionCCFactor(interaction_cc_factor_);
     e_ptr->SetInteractionNCFactor(interaction_nc_factor_);
     e_ptr->SetInteractionGRFactor(interaction_gr_factor_);

     log_debug(" I3NuGWeightedEvent successfully generated");
     event_ptr = e_ptr;
  }

  log_debug(" about to call MakeNeutrinoPropagation");
  event_ptr->SetEventCount(eventCounter_);


  // make neutrino propagation!
  bool got_inice = event_ptr->MakeNeutrinoPropagation(injected_nu, skip_final_interaction);

  if(got_inice)log_trace("In-Ice particles are obtained");
  else log_trace("In-Ice particles NOT obtained");
  log_debug(" finished MakeNeutrinoPropagation");

  std::pair<bool, I3NuGVEventPtr> result(got_inice, event_ptr);
  return result;

}

//______________________________________________________________
void
I3NeutrinoGeneratorBase::FillMC(I3NuGVEventPtr event_ptr,
                                I3FramePtr frame)
{
  ParticlePtrList& inEarth_NuGList = event_ptr->GetInEarthPropagatingParticleList();

  // fill MCTree
  Utils::PutMCTree(frame, inEarth_NuGList, inice_tree_name_);
  
  // fill MCWeightDict
  event_ptr->FillMCWeights(frame);
  event_ptr->CheckFilledMCWeights();
}

//______________________________________________________________
I3Map<I3ParticleID, double>
I3NeutrinoGeneratorBase::PropagateInEarth(
                                   ParticlePtr injected_nu, 
                                   I3FramePtr frame, 
                                   bool save_result) 
{
  // skip_final_interaction should be true
  std::pair<bool, I3NuGVEventPtr> result
                      = PropagateNu(injected_nu, frame, true);
  bool survived = result.first;
  I3NuGVEventPtr event_ptr = result.second;

  // fill MC info
  FillMC(event_ptr, frame);
 
  // prepare return weight map
  boost::shared_ptr<I3Map<I3ParticleID, double> > daughters(new I3Map<I3ParticleID, double>());

  if (!survived) {
     ++eventCounter_;
     log_info(" no particle survived");
     return *daughters;
  }
	
  ParticlePtrList cand = event_ptr->GetFinalInteractionNuCandidates();
  log_debug("%d candidates found", int(cand.size()));

  int count = 0;
  for (unsigned int i=0; i<cand.size(); ++i) {
     ParticlePtr p = cand[i];
     InteractionInfo &intinfo = p->GetInteractionInfoRef();
     double weight = intinfo.GetPropagationWeight();
     I3ParticleID id = p->GetID();

     (*daughters)[id] = weight;

     log_trace("+++ Secondary particle %d :type %s, energy 1e%g, location %s, weight %g is added", 
             count, (p->GetTypeString()).c_str(), 
             log10(p->GetEnergy()), (p->GetLocationTypeString()).c_str(), weight);
     ++count;
  }

  if (save_result) {
     frame->Put("NuGInIceNeutrinos", daughters);
  }

  ++eventCounter_;

  return *daughters;
}

//______________________________________________________________
void I3NeutrinoGeneratorBase::PropagateAll(ParticlePtr injected_nu,
                                       I3FramePtr frame)
{

  // skip_final_interaction should be false
  std::pair<bool, I3NuGVEventPtr> result
                      = PropagateNu(injected_nu, frame, false);

  I3NuGVEventPtr event_ptr = result.second;

  event_ptr->PrintEvent();

  FillMC(event_ptr, frame);

  ++eventCounter_;
  return;
}

