/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *
 *   @version $Revision: $
 *   @date $Date: $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief Crosssection implimentation file for I3NeutrinoGeneratorNC
 *   here you find all the final state classes for processes in the
 *   Standard Model. The default data is supplied through tables.
 * 
 */
//////////////////////////////////////////////////////////////////////// 
#include "neutrino-generator/interactions/InteractionBase.h"
#include "neutrino-generator/interactions/InteractionNC.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/table-interface/CrosssectionTableReader.h"
#include "neutrino-generator/table-interface/FinalStateTableReader.h"
#include "dataclasses/I3Constants.h"

namespace nugen {

////////////////////////////////////////////////////////////////////////
InteractionNC::InteractionNC(I3RandomServicePtr random,
                             SteeringPtr steer):
  InteractionBase(random, steer)
{
   log_debug("Constructing NC interaction");
}
////////////////////////////////////////////////////////////////////////
InteractionNC::~InteractionNC()
{
}

//____________________________________________________________________
void InteractionNC::FillDaughterParticles(ParticlePtr nuin_ptr, 
                                          double)
{

  log_trace("NC::neutral current interaction has been chosen to create next particles");

  if (!nuin_ptr->GetLength()) {
    log_error("Position of interaction need to be determined before selecting daughters!");
  }

  // select a secondary flavor
  I3Particle::ParticleType leptonFlavor = nuin_ptr->GetType();
  // neutrino mass is apploximately zero...
  double lepton_mass = 0;

  // Do we simulate outgoing costheta angle of secondary neutrino?
  // By default it's true because even a small deviation result in missing target (=icecube).
  // If we do it correctly, we have to inject neutrinos from large area.
  //
  //bool ignoreOutgoingAngleForNC = true;
  bool ignoreOutgoingAngleForNC = steer_->IgnoreOutgoingAngleForNC();
  double out_lepton_E = SetSecondaryLepton(nuin_ptr, 
                                           leptonFlavor, 
                                           lepton_mass, ignoreOutgoingAngleForNC);


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
  hadron_daughter_particle->SetType(I3Particle::Hadrons);
  hadron_daughter_particle->SetEnergy(had_energy);
  hadron_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

  nuin_ptr->AddDaughter(hadron_daughter_particle);

  log_trace(" # We have %d daughter particles", nuin_ptr->GetTotalNDaughters());

}

}
