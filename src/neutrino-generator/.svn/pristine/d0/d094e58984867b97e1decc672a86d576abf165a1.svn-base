/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *
 *   @version $Revision: $
 *   @date $Date: $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief Crosssection implimentation file for I3NeutrinoGeneratorCC
 *   here you find all the final state classes for processes in the
 *   Standard Model. The default data is supplied through tables.
 * 
 */

#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/interactions/InteractionBase.h"
#include "neutrino-generator/interactions/InteractionCC.h"
#include "neutrino-generator/utils/Constants.h"
#include "neutrino-generator/table-interface/CrosssectionTableReader.h"
#include "neutrino-generator/table-interface/FinalStateTableReader.h"
#include "dataclasses/I3Constants.h"

namespace nugen {

using namespace Constants;

//___________________________________________________________________
InteractionCC::InteractionCC(I3RandomServicePtr random,
                             SteeringPtr steer):
  InteractionBase(random, steer)
{
  log_debug("Constructing CC interaction");
}

//___________________________________________________________________
InteractionCC::~InteractionCC()
{
  log_trace("deconstruct SigmaCC");
}

//___________________________________________________________________
void InteractionCC::FillDaughterParticles(ParticlePtr nuin_ptr, 
                                          double)
{
  log_trace("CC::charged current interaction has been chosen to create next particles");

  if (!nuin_ptr->GetLength()) {
    log_error("Position of interaction need to be determined before selecting daughters!");
  }

  // select a secondary flavor
  I3Particle::ParticleType leptonFlavor = I3Particle::unknown;
  double lepton_mass = 0;

  switch (nuin_ptr->GetType()) {
     case I3Particle::NuE : 
        leptonFlavor = I3Particle::EMinus;   
        lepton_mass = M_e;
        break;
     case I3Particle::NuEBar :
        leptonFlavor = I3Particle::EPlus; 
        lepton_mass = M_e;
        break;
     case I3Particle::NuMu :
        leptonFlavor = I3Particle::MuMinus;
        lepton_mass = M_MU;
        break;
     case I3Particle::NuMuBar : 
        leptonFlavor = I3Particle::MuPlus;
        lepton_mass = M_MU;
        break;
     case I3Particle::NuTau : 
        leptonFlavor = I3Particle::TauMinus;
        lepton_mass = M_TAU;
        break;
     case I3Particle::NuTauBar :
        leptonFlavor = I3Particle::TauPlus; 
        lepton_mass = M_TAU;
        break;
     default: 
        log_error("invalid neutrino type"); 
        break;
  }

  double out_lepton_E = 0;
  if (lepton_mass == M_TAU && nuin_ptr->GetLocationType() != I3Particle::InIce) {
     // this is NuTau CC interaction inside the Earth.
     // ignore outgoing angle calculation.
     out_lepton_E  = SetSecondaryLepton(nuin_ptr, 
                           leptonFlavor, lepton_mass, true);
  } else {
     out_lepton_E  = SetSecondaryLepton(nuin_ptr, 
                           leptonFlavor, lepton_mass);
  }

  //create hadron  
  double had_energy = nuin_ptr->GetEnergy() - out_lepton_E;

  ParticlePtr hadron_daughter_particle(new Particle(I3Particle::Null, 
                                           I3Particle::unknown, steer_));
  hadron_daughter_particle->SetShape(I3Particle::Cascade);//4
  hadron_daughter_particle->SetFitStatus(I3Particle::NotSet);//5
  hadron_daughter_particle->SetPos(nuin_ptr->GetEndPosition());//6
  hadron_daughter_particle->SetTime(nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed());//7
  hadron_daughter_particle->SetDir(nuin_ptr->GetDir());//8
  hadron_daughter_particle->SetLength(NAN);//9
  hadron_daughter_particle->SetSpeed(I3Constants::c);//10
  hadron_daughter_particle->SetType(I3Particle::Hadrons);//11
  hadron_daughter_particle->SetEnergy(had_energy);//12
  hadron_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

  nuin_ptr->AddDaughter(hadron_daughter_particle);
  log_trace(" # We have %d daughter particles", nuin_ptr->GetTotalNDaughters());

}

}

