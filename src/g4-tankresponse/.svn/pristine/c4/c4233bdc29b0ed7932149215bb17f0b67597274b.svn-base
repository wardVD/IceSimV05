/*
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de>, Peter Nießen (tanktop)
 * Last changed by: $LastChangedBy$
 */


#include <globals.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4NeutrinoE.hh>
#include <G4AntiNeutrinoE.hh>
#include <G4ProcessManager.hh>

#include "G4IceTopEMPhysics.h"


G4IceTopEMPhysics::G4IceTopEMPhysics()
  : G4VPhysicsConstructor("standard")
{}


G4IceTopEMPhysics::~G4IceTopEMPhysics()
{}


void G4IceTopEMPhysics::ConstructParticle()
{
  // gamma
  G4Gamma::GammaDefinition();
 
  // electron
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
}


void G4IceTopEMPhysics::ConstructProcess()
{
  G4ProcessManager *pManager = 0;
  
  // Gamma Physics
  pManager = G4Gamma::Gamma()->GetProcessManager();
  pManager->AddDiscreteProcess(&photoEffect);
  pManager->AddDiscreteProcess(&comptonEffect);
  pManager->AddDiscreteProcess(&pairProduction);

  // Electron Physics
  pManager = G4Electron::Electron()->GetProcessManager();
  pManager->AddProcess(&electronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(&electronIonisation,         -1, 2, 2);
  pManager->AddProcess(&electronBremsStrahlung,     -1, 3, 3);  

  //Positron Physics
  pManager = G4Positron::Positron()->GetProcessManager();
  pManager->AddProcess(&positronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(&positronIonisation,         -1, 2, 2);
  pManager->AddProcess(&positronBremsStrahlung,     -1, 3, 3);  
  pManager->AddProcess(&annihilation,                0,-1, 4);  
}
