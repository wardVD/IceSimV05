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

////////
I3_MODULE(I3NeutrinoGenerator);

//______________________________________________________________
I3NeutrinoGenerator::I3NeutrinoGenerator(const I3Context& ctx) : 
  I3ConditionalModule(ctx), I3NeutrinoGeneratorBase()
{

  AddParameter("EventYear",    "", event_year_);
  AddParameter("EventDaqTime", "", event_daqtime_);
  AddParameter("RunID",        "", runid_);

  AddParameter("RandomService", "Pointer to RandomService", random_);
  AddParameter("SteeringName", "Name of Steering", steering_name_);
  AddParameter("InjectorName", "Name of NuGInjector", injector_name_);
  AddParameter("InteractionInfoName", "Name of NuGInteractionInfo", interaction_name_);

  //Do you want to simulate a specified neutrino?                        
  AddParameter("PrimaryNuName", "If specified NuGen takes "              
                "an I3Particle from a frame and propagate it",           
                primary_nu_name_ );       

  interaction_cc_factor_ = 1.0; // default
  AddParameter("InteractionCCFactor", 
               "weight parameter for CC interaction at final interaction."
               "If you want to simulate more CC events, set the factor >1. "
               "Final event weight compensate the factor so that you "
               "will get correct event weight.",
               interaction_cc_factor_);

  interaction_nc_factor_ = 1.0; // default
  AddParameter("InteractionNCFactor", 
               "weight parameter for NC interaction at final interaction."
               "If you want to simulate more NC events, set the factor >1. "
               "Final event weight compensate the factor so that you "
               "will get correct event weight.",
               interaction_nc_factor_);

  interaction_gr_factor_ = 1.0; // default
  AddParameter("InteractionGRFactor", 
               "weight parameter for GR interaction at final interaction."
               "If you want to simulate more GR events, set the factor >1. "
               "Final event weight compensate the factor so that you "
               "will get correct event weight.",
               interaction_gr_factor_);


  prop_mode_ = nugen::AUTODETECT;
  AddParameter("PropagationWeightMode", 
               "* nugen::LEGACY: use legacy I3NuGEvent. \n"
               "* nugen::NOPROPWEIGHT: no propagation weight with "
               "I3NuGWeightedEvent. "
               "Some neutrino may be absorbed by the Earth. \n"
               "* nugen::NCGRWEIGHTED: CC interaction is forbidden and "
               "always NC or GR interacion is chosen if interactions "
               "occurred during propagation inside the Earth. "
               "Propagation weight is applied. "
               "No neutrino is absorbed by the Earth.\n"
               "* nugen::AUTODETECT use NCGRWEIGHTED for NuE and NuMu, NOPROPWEIGHT for NuTau.\n"
               "CAUTION: for NuTau , always use NoWeihgt option to activate tau regeneration.",
               prop_mode_);

  interact_pos_opt_ = 1;  // must be 1 for correct simulation
  AddParameter("InteractionPositionOpt", 
               "option for interaction position calculation. "
               "Default value is 1, and do NOT touch the value "
               "unless you know well about the option."
               "See StaticParams.cxx",
               interact_pos_opt_);

  interact_weight_opt_ = 1;  // must be 1 for correct simulation
  AddParameter("InteractionWeightOpt", 
               "option for interaction weight calculation. "
               "Default value is 1, and do NOT touch the value "
               "unless you know well about the option."
               "See StaticParams.cxx",
               interact_weight_opt_);

  AddParameter("CrosssectionColumndepthOpt", 
               "option for crossection*columndepth calculation. "
               "See StaticParams.cxx",
               crosssection_columndepth_opt_);

  AddParameter("ImpactParamOpt", 
               "option for impact param calculation. "
               "See StaticParams.cxx",
               impactparam_opt_);

  inice_tree_name_ = "";
  AddParameter("MCTreeName",
               "Name of I3MCTree",
               inice_tree_name_);

  AddOutBox("OutBox");
 
}

//______________________________________________________________
void I3NeutrinoGenerator::Configure()
{ 
  /**
   * In general, configurable parmeters are 16
   */
  log_debug("Configure");

  /**
   * Instanciate I3NuGInteractionInfo class pointer
   * and fill all the info which is passed to I3NuGEvent
   */  

  // scaling cross sections
  // 

  GetParameter("InteractionCCFactor", interaction_cc_factor_);
  GetParameter("InteractionNCFactor", interaction_nc_factor_);
  GetParameter("InteractionGRFactor", interaction_gr_factor_);

  // weighting control
  // 
  GetParameter("PropagationWeightMode",  prop_mode_);
  GetParameter("InteractionPositionOpt", interact_pos_opt_);
  GetParameter("InteractionWeightOpt",   interact_weight_opt_);
  GetParameter("CrosssectionColumndepthOpt", crosssection_columndepth_opt_);
  GetParameter("ImpactParamOpt",impactparam_opt_);
  GetParameter("MCTreeName", inice_tree_name_);
  GetParameter("EventYear",     event_year_);
  GetParameter("EventDaqTime",  event_daqtime_);
  GetParameter("RunID",         runid_);
  GetParameter("PrimaryNuName", primary_nu_name_);
  GetParameter("RandomService", random_);
  GetParameter("SteeringName",  steering_name_);
  GetParameter("InjectorName",  injector_name_);
  GetParameter("InteractionInfoName", interaction_name_);

  eventCounter_ = 0;

  steer_ = context_.Get<SteeringPtr>(steering_name_);
  if (!steer_) {
    log_fatal_stream("nugen::Steering with name '" << steering_name_ 
                     << "' not found in the context, please check SteeringName");
  }
  earth_ = steer_->GetEarthModelService();
  interaction_ = context_.Get<I3NuGInteractionInfoPtr>(interaction_name_);
  if (!interaction_) {
    log_fatal_stream("nugen::InteractionInfo with name '" << interaction_name_
                     << "' not found in the context, please check InteractionInfoName");
  }

  if (inice_tree_name_ == "") {
     // use default tree name stored in steer.
     inice_tree_name_ = steer_->GetMCTreeName();
  }

  Init();

  log_trace("Finished configuration");
}

//______________________________________________________________
void I3NeutrinoGenerator::DAQ(I3FramePtr frame)
{

  // initialize my event counter
  number_of_events_ = steer_->GetNGen();

  if( eventCounter_ >= number_of_events_ ) {
     RequestSuspension();
     return;
  }

  if (! random_) {
     random_ = context_.Get<I3RandomServicePtr>();
  }

  log_debug("================");
  log_debug(" Start event %ld ", eventCounter_);
  log_debug("================");
  //if(doPrintMemoryInfo_){MemoryInfo::instance()->snapshot();MemoryInfo::instance()->print();}

  /**
   * inject primary or take it from frame
   */

  ParticlePtr injected_neu_ptr;

  // Get MCWeightDict from frame or create new weight
  // dict and fill default(1.0) for all weight values
  string weightname = steer_->GetWeightDictName();
  I3MapStringDoublePtr wmap = 
         Utils::PutWeightDict(frame, weightname);

  if (frame->Has(primary_nu_name_)) {

     // Get I3Particle from frame and convert it to nugen::Particle. 
     // Also, all weight info will be restored to Particle.
     injected_neu_ptr = Utils::GetParticle(frame, primary_nu_name_, steer_, wmap);

     // set length of the particle
     I3Position posCE(earth_->GetDetCoordPosFromEarthCoordPos(injected_neu_ptr->GetPos())); 
     I3Direction dirCE(earth_->GetDetCoordDirFromEarthCoordDir(injected_neu_ptr->GetDir())); 

     double t_CE;
     I3Position impact_pos_CE;
     EarthModelCalculator::GetImpactParameter(
                      posCE, dirCE, t_CE, impact_pos_CE);

     I3Position start_CE, end_CE;
     int success = 
        EarthModelCalculator::GetIntersectionsWithSphere(
                impact_pos_CE, dirCE,
                steer_->GetWorldRadius(),
                start_CE, end_CE);
     if (!success) {
        // this time neutrino didn't pass through the Earth
        log_error(" neutrino didn't pass through the earth");
        
        ++eventCounter_;
        return;
     }
     double length = (end_CE - posCE).Magnitude();
     injected_neu_ptr->SetLength(length);

     // calc interaction geom
     injected_neu_ptr->GetInteractionGeoRef();

  } else {

     // no primary exist, use injector.
     injector_ = GetService<I3NuGInjectorPtr>(injector_name_);

     /**
      * Create a primary neutrino, store primary info to weight map
      */
     log_debug(" about to inject a neutrino");
     
     bool success = injector_->InjectInitialNeutrino(frame, wmap);

     if (!success) {
        // this time neutrino didn't pass through the Earth
        log_error(" failed to inject a neutrino");
        
        ++eventCounter_;
        return;
     }

     log_debug(" obtain injected neutrino");
     injected_neu_ptr = injector_->GetIncidentNeutrino();
  }

  // save primary particle to frame and copy weight info etc. from 
  // injected_neu_ptr to wmap.
  
  Utils::PutParticle(frame,injected_neu_ptr, primary_nu_name_, wmap); 

  // let's propagate the primary
  SimMode simmode = steer_->GetSimMode();
  if (simmode == INEARTH) {
     PropagateInEarth(injected_neu_ptr, frame, true);
  } else {
     PropagateAll(injected_neu_ptr, frame);
  }

  PushFrame(frame,"OutBox");  
  return;

}//DAQ()

//______________________________________________________________
void I3NeutrinoGenerator::Finish()
{
  if ( eventCounter_ != number_of_events_ ) {
     log_fatal("Number of processed events (%ld) doesn't match with your NEvents parameter (%ld). Did you limit number of events (streams) like tray.Execute(%ld)?  Use tray.Execute() instead, then NuGen handles it.", eventCounter_, number_of_events_, number_of_events_);

  }
}

