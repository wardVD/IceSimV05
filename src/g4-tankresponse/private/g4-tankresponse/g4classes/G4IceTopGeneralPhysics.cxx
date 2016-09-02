/*
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: G4IceTopGeneralPhysics.cxx 86420 2012-03-20 16:00:37Z kislat $
 *
 * @version $Revision: 86420 $
 * @date $LastChangedDate: 2012-03-20 11:00:37 -0500 (Tue, 20 Mar 2012) $
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy: kislat $
 */

#include "G4IceTopGeneralPhysics.h"

#include <iomanip>   

#include <globals.hh>
#include <G4ios.hh>
#include <G4ParticleDefinition.hh>
#include <G4ProcessManager.hh>
// Bosons
#include <G4ChargedGeantino.hh>
#include <G4Geantino.hh>


G4IceTopGeneralPhysics::G4IceTopGeneralPhysics()
  :  G4VPhysicsConstructor("general")
{}


G4IceTopGeneralPhysics::~G4IceTopGeneralPhysics()
{}


void G4IceTopGeneralPhysics::ConstructParticle()
{
  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();  
}


void G4IceTopGeneralPhysics::ConstructProcess()
{
  //AddTransportation();

  // Add Decay Process
  theParticleIterator->reset();
  while ((*theParticleIterator)()) {
    G4ParticleDefinition *particle = theParticleIterator->value();
    G4ProcessManager *pmanager = particle->GetProcessManager();
    if (decay.IsApplicable(*particle)) { 
      pmanager->AddProcess(&decay);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager->SetProcessOrdering(&decay, idxPostStep);
      pmanager->SetProcessOrdering(&decay, idxAtRest);
    }
  }
}

