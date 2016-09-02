/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief TauDecay implimentation file
 *
 */
//////////////////////////////////////////////////////////////////////
#include "neutrino-generator/interactions/InteractionBase.h"
#include "neutrino-generator/interactions/TauDecay.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/table-interface/DecayStateTableReader.h"
#include "neutrino-generator/utils/Constants.h"
#include "dataclasses/I3Constants.h"

using namespace std;

namespace nugen {
using namespace Constants;

/////////////////////////////////////////////////////////////////////////////
TauDecay::TauDecay(I3RandomServicePtr random,
                   SteeringPtr steer)
   : InteractionBase(random, steer)
{
 log_debug("Constructing Tau decay");
 tau_final_ptr = boost::shared_ptr<DecayStateTableReader>(new DecayStateTableReader());

}
/////////////////////////////////////////////////////////////////////////////
TauDecay::~TauDecay()
{ 
 log_trace("TauDecay deconstructed");
}
/////////////////////////////////////////////////////////////////////////////
void TauDecay::InitializeFinalStateTable(const string& finalfile){
  decayname_ = finalfile;
  tau_final_ptr->ReadDecayStateTableFillArray(finalfile);
}

//___________________________________________________________________________
void TauDecay::FillDaughterParticles(ParticlePtr tau_ptr, double decay_energy){

  if(tau_ptr->GetInteractionInfoRef().GetInteractionType()
      != DECAY)
  {
     log_error("This is supposed be DECAY, CHECK!!");
  }

  log_trace("FillDecayDaughterParticles");
  int tau_charge = 0;
  if(tau_ptr->GetType()==I3Particle::TauPlus)tau_charge=1;
  else if(tau_ptr->GetType()==I3Particle::TauMinus)tau_charge=-1;
  else log_fatal("tau particle type not defined");
  int n_daughter=0;

  //Possible final states: nu_tau, nu_mu, nu_e, hadron, muon, electron
  double finalstate[6]={0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double rand = random_->Uniform(0.0, 1.0);

  tau_final_ptr->SampleDecayState(rand, finalstate);
  double secondary_energy = 0;

  if (!finalstate[0]) {
     log_error("There is no nu_tau in TAU DECAY! check!");
  } else{
     log_trace("fill secondary NuTau");
     secondary_energy = finalstate[0] * decay_energy;

     //using input array finalstate[6]
     //nu_tau is always there
     //type and energy are filled for each particle
     //Particle first_daughter_particle;
     ParticlePtr first_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
     ++n_daughter;
     first_daughter_particle->SetShape(I3Particle::Null);//4
     first_daughter_particle->SetFitStatus(I3Particle::NotSet);//5
     first_daughter_particle->SetPos(tau_ptr->GetEndPosition());//6
     first_daughter_particle->SetTime(tau_ptr->GetTime()+tau_ptr->GetLength()/tau_ptr->GetSpeed());//7
     first_daughter_particle->SetDir(tau_ptr->GetDir());//8
     first_daughter_particle->SetLength(NAN);//9
     first_daughter_particle->SetSpeed(I3Constants::c);//10
     if(tau_charge>0)first_daughter_particle->SetType(I3Particle::NuTauBar);//11
     else if(tau_charge<0)first_daughter_particle->SetType(I3Particle::NuTau);
     first_daughter_particle->SetEnergy(secondary_energy);//12
     first_daughter_particle->CopyExtraInfoFrom(*tau_ptr);

     tau_ptr->AddDaughter(first_daughter_particle);
  }

  if (finalstate[1]!=0.0) {     
     log_trace("fill secondary NuMu");
     secondary_energy = finalstate[1] * decay_energy;

     ++n_daughter;//if non-zero nu_mu energy  anti-nu_mu
     ParticlePtr numu_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
    
     numu_daughter_particle->SetShape(I3Particle::Null);//4
     numu_daughter_particle->SetFitStatus(I3Particle::NotSet);//5
     numu_daughter_particle->SetPos(tau_ptr->GetEndPosition());//6
     numu_daughter_particle->SetTime(tau_ptr->GetTime()+tau_ptr->GetLength()/tau_ptr->GetSpeed());//7
     numu_daughter_particle->SetDir(tau_ptr->GetDir());  //8
     numu_daughter_particle->SetLength(NAN);//9
     numu_daughter_particle->SetSpeed(I3Constants::c);//10
     if(tau_charge>0)numu_daughter_particle->SetType(I3Particle::NuMu);//tauplus
     else if(tau_charge<0)numu_daughter_particle->SetType(I3Particle::NuMuBar);//tau
     numu_daughter_particle->SetEnergy(secondary_energy);
     numu_daughter_particle->CopyExtraInfoFrom(*tau_ptr);

     tau_ptr->AddDaughter(numu_daughter_particle);
  }

  if (finalstate[2]!=0.0) {     ++n_daughter;//if non-zero nu_e energy  anti-nu_e
     log_trace("fill secondary NuE");
     secondary_energy = finalstate[2] * decay_energy;

     ParticlePtr nue_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
     nue_daughter_particle->SetShape(I3Particle::Null);//4
     nue_daughter_particle->SetFitStatus(I3Particle::NotSet);//5
     nue_daughter_particle->SetPos(tau_ptr->GetEndPosition());//6
     nue_daughter_particle->SetTime(tau_ptr->GetTime()+tau_ptr->GetLength()/tau_ptr->GetSpeed());//7
     nue_daughter_particle->SetDir(tau_ptr->GetDir());  //8
     nue_daughter_particle->SetLength(NAN);//9
     nue_daughter_particle->SetSpeed(I3Constants::c);//10
     if(tau_charge>0)nue_daughter_particle->SetType(I3Particle::NuE);//tauplus
     else if(tau_charge<0)nue_daughter_particle->SetType(I3Particle::NuEBar);//tau
     nue_daughter_particle->SetEnergy(secondary_energy);
     nue_daughter_particle->CopyExtraInfoFrom(*tau_ptr);

     tau_ptr->AddDaughter(nue_daughter_particle);
  }

  if(finalstate[3]!=0.0){     ++n_daughter;//hadron cascade (pions, kaons)
     log_trace("fill secondary Hadron");
     secondary_energy = finalstate[3] * decay_energy;

     ParticlePtr hadron_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
     hadron_daughter_particle->SetShape(I3Particle::Cascade);//4
     hadron_daughter_particle->SetFitStatus(I3Particle::NotSet);//5
     hadron_daughter_particle->SetPos(tau_ptr->GetEndPosition());//6
     hadron_daughter_particle->SetTime(tau_ptr->GetTime()+tau_ptr->GetLength()/tau_ptr->GetSpeed());//7
     hadron_daughter_particle->SetDir(tau_ptr->GetDir());  //8
     hadron_daughter_particle->SetLength(NAN);//9
     hadron_daughter_particle->SetSpeed(I3Constants::c);//10
     if(tau_charge>0)hadron_daughter_particle->SetType(I3Particle::Hadrons);//tau plus
     else if(tau_charge<0)hadron_daughter_particle->SetType(I3Particle::Hadrons);//tau
     hadron_daughter_particle->SetEnergy(secondary_energy);
     hadron_daughter_particle->CopyExtraInfoFrom(*tau_ptr);

     tau_ptr->AddDaughter(hadron_daughter_particle);
  }

  if (finalstate[4]!=0.0) {     ++n_daughter;//em cascade  (electrons, photons)
     log_trace("fill secondary Electrons");
     secondary_energy = finalstate[4] * decay_energy;

     ParticlePtr electron_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
     electron_daughter_particle->SetShape(I3Particle::Cascade);//4
     electron_daughter_particle->SetFitStatus(I3Particle::NotSet);//5
     electron_daughter_particle->SetPos(tau_ptr->GetEndPosition());//6
     electron_daughter_particle->SetTime(tau_ptr->GetTime()+tau_ptr->GetLength()/tau_ptr->GetSpeed());//7
     electron_daughter_particle->SetDir(tau_ptr->GetDir());  //8
     electron_daughter_particle->SetLength(NAN);//9
     electron_daughter_particle->SetSpeed(I3Constants::c);//10
     if(tau_charge>0)electron_daughter_particle->SetType(I3Particle::EPlus);//positron tau plus
     else if(tau_charge<0)electron_daughter_particle->SetType(I3Particle::EMinus);//electron tau
     electron_daughter_particle->SetEnergy(secondary_energy);
     electron_daughter_particle->CopyExtraInfoFrom(*tau_ptr);
     if (secondary_energy < M_e) {
        // this shouldn't be visible.
        electron_daughter_particle->SetShape(I3Particle::Dark);
     }

     tau_ptr->AddDaughter(electron_daughter_particle);
  }

  if (finalstate[5]!=0.0) {     ++n_daughter;//muon
     log_trace("fill secondary Muons");
     secondary_energy = finalstate[5] * decay_energy;

     ParticlePtr muon_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
     muon_daughter_particle->SetShape(I3Particle::Null);//4
     muon_daughter_particle->SetFitStatus(I3Particle::NotSet);//5
     muon_daughter_particle->SetPos(tau_ptr->GetEndPosition());//6
     muon_daughter_particle->SetTime(tau_ptr->GetTime()+tau_ptr->GetLength()/tau_ptr->GetSpeed());//7
     muon_daughter_particle->SetDir(tau_ptr->GetDir());  //8
     muon_daughter_particle->SetLength(NAN);//9
     muon_daughter_particle->SetSpeed(I3Constants::c);//10
     if(tau_charge>0)muon_daughter_particle->SetType(I3Particle::MuPlus);//anti-muon tau plus
     else if(tau_charge<0)muon_daughter_particle->SetType(I3Particle::MuMinus);//muon tau
     muon_daughter_particle->SetEnergy(secondary_energy);
     muon_daughter_particle->CopyExtraInfoFrom(*tau_ptr);
     if (secondary_energy < M_MU) {
        // this shouldn't be visible.
        muon_daughter_particle->SetShape(I3Particle::Dark);
     }

     tau_ptr->AddDaughter(muon_daughter_particle);
  }
  log_trace("Tau Energy %f, Sum Decay Daughter Energy %f", decay_energy, tau_ptr->GetTotalDaughterEnergies());

}
/////////////////////////////////////////////////////////////////////////////

}

