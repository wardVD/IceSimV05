/** 
 *  $Id: $ *  @file I3NuGSourceSelector.cxx *  @version $Revision:  $
 *  @date    $Date:      $
 *
 *  Copyright (C) 2005
 *  the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  Aya Ishihara  <aya.ishihara@icecube.wisc.edu>
 *  modified by Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 */

#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/utils/Utils.h"
#include "neutrino-generator/utils/Calculator.h"
#include "neutrino-generator/utils/Defaults.h"
#include "neutrino-generator/sources/SourceUtils.h"
#include "neutrino-generator/sources/I3NuGSourceSelector.h"

using namespace std;
using namespace nugen;
using namespace earthmodel;

I3_MODULE(I3NuGSourceSelector);

//______________________________________________________________
I3NuGSourceSelector::I3NuGSourceSelector(const I3Context& ctx) : 
  I3ConditionalModule(ctx), Pickup(),
  number_of_events_(-1),
  eventCounter_(0),
  energy_bias_power_(0),
  particle_biases_(6, 1.0),
  keep_dark_(false),
  useZeVForm_(false)
{
  AddParameter("RandomService", "pointer to random service", "I3RandomService");
  AddParameter("SteeringName", "name of steering service", Defaults::steerName);
  AddParameter("EnergyBiasPowerIndex", 
               "Neutrino is selected with pow(energy [GeV], index)",
                energy_bias_power_);

  AddParameter("ParticleBiases", 
               "Bias ratio for NuE:NuEBar:NuMu:NuMuBar:NuTau:NuTauBar. "
               "must be 6-dim vector<double>",
                particle_biases_);

  AddParameter("KeepDarkNeutrinos", 
               "Do you want to keep dark neutrinos? ",
                keep_dark_);

  AddParameter("UseZeVForm", 
               "If your neutrino bundle contains ~ZeV neutrinos "
               "set it true.",
                useZeVForm_);

  AddOutBox("OutBox");
}

//______________________________________________________________
void I3NuGSourceSelector::Configure()
{ 
  log_debug("called Configure");
  GetParameter("RandomService",    random_);

  GetParameter("SteeringName", steer_name_);
  steer_ptr_ = GetService<SteeringPtr>(steer_name_);
  if (!steer_ptr_) {
     log_fatal("You have to add I3NuGSteerService in advance!");
  }

  GetParameter("EnergyBiasPowerIndex", energy_bias_power_);
  GetParameter("ParticleBiases", particle_biases_);
  GetParameter("KeepDarkNeutrinos", keep_dark_);
  GetParameter("UseZeVForm", useZeVForm_); 

  Initialize();

  log_debug("Configured!");
}

//______________________________________________________________
void I3NuGSourceSelector::Initialize()
{ 
  log_debug("called Initialize");
  particle_bias_map_.clear();
  particle_bias_map_[I3Particle::NuE] = particle_biases_[0];
  particle_bias_map_[I3Particle::NuEBar] = particle_biases_[1];
  particle_bias_map_[I3Particle::NuMu] = particle_biases_[2];
  particle_bias_map_[I3Particle::NuMuBar] = particle_biases_[3];
  particle_bias_map_[I3Particle::NuTau] = particle_biases_[4];
  particle_bias_map_[I3Particle::NuTauBar] = particle_biases_[5];

  // initialize event counter etc.
  number_of_events_ = steer_ptr_->GetNGen();
  eventCounter_ = 0;
}

//___________________________________________________________________
void I3NuGSourceSelector::DAQ(I3FramePtr frame)
{
  if (number_of_events_>0 && eventCounter_ >= number_of_events_ ) {
      RequestSuspension();
      return;
  }

  if (! random_) {
     random_ = GetService<I3RandomServicePtr>();
  }

  I3MCTreeConstPtr tree = frame->Get<I3MCTreeConstPtr>(steer_ptr_->GetMCTreeName());
  if (!tree) {
     log_error("MCTree %s doesn't exist!", steer_ptr_->GetMCTreeName().c_str());
  }

  I3MCTree::const_iterator iter;

  std::vector<ParticlePtr> nuvec;
  
  I3Surfaces::SurfaceConstPtr detectionVolume = steer_ptr_->GetDetectionSurface();

  for (iter=tree->begin(); iter!=tree->end(); ++iter) {
     if (iter->IsNeutrino()) {
        // Omit neutrinos that don't ever intersect the sampling surface
        if (detectionVolume &&
           !std::isfinite(detectionVolume->GetIntersection(
           iter->GetPos(), iter->GetDir()).first)) {
              log_trace_stream(iter->GetEnergy() << " GeV "
                 << iter->GetTypeString() << " outside volume!");
              continue;
        }

        ParticlePtr nu(new Particle(*iter, steer_ptr_));
        nuvec.push_back(nu);
     }
  }

  ParticlePtr nup;
  
  if (nuvec.size() > 0) {
     // Pickup a neutrino!
     log_trace("Try to pickup a neutrino");
     std::pair<ParticlePtr, double> apair = 
             PickupANeutrino(random_, nuvec, useZeVForm_);

     nup = apair.first;
     double selection_weight = apair.second;
     InteractionInfo& intinfo = nup->GetInteractionInfoRef();
     intinfo.SetPrimarySelectionWeight(selection_weight);

     log_debug("Selected Neutrino : Type %s, log10E %e, Weight %e",
                nup->GetTypeString().c_str(), log10(nup->GetEnergy()),  
                selection_weight);

     // put the particle to frame
     I3MapStringDoublePtr wmap = 
            Utils::PutWeightDict(frame, steer_ptr_->GetWeightDictName());
     log_trace("Put weight map");

     Utils::PutParticle(frame, nup, Defaults::primaryNuName, wmap);
     log_trace("Put selected particle");
  }

  // let's clean up input MCTree
  Utils::PutCleanedMCTree(frame, nup, steer_ptr_->GetMCTreeName(), keep_dark_);
  log_trace("Added cleaned tree");

  PushFrame(frame,"OutBox");  

  ++eventCounter_;
  return;

}

//___________________________________________________________________
double I3NuGSourceSelector::SelectionWeighter(ParticlePtr p) {
   double eweight = std::pow(p->GetEnergy(), energy_bias_power_);
   double ptypeweight = 1.0;
   if (particle_bias_map_.count(p->GetType())) {
      ptypeweight = particle_bias_map_[p->GetType()];
   }
   return eweight * ptypeweight;
}

