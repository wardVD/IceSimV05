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
#include <G4MuonPlus.hh>
#include <G4MuonMinus.hh>
#include <G4TauMinus.hh>
#include <G4TauPlus.hh>
#include <G4NeutrinoTau.hh>
#include <G4AntiNeutrinoTau.hh>
#include <G4NeutrinoMu.hh>
#include <G4AntiNeutrinoMu.hh>
#include <G4ProcessManager.hh>

#include "G4IceTopMuonPhysics.h"



G4IceTopMuonPhysics::G4IceTopMuonPhysics()
  :  G4VPhysicsConstructor("muon")
{}

/********************************************************************/

G4IceTopMuonPhysics::~G4IceTopMuonPhysics()
{}

/********************************************************************/

void G4IceTopMuonPhysics::ConstructParticle()
{
  // Mu
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();

  // Tau
  G4TauMinus::TauMinusDefinition();
  G4TauPlus::TauPlusDefinition();
  G4NeutrinoTau::NeutrinoTauDefinition();
  G4AntiNeutrinoTau::AntiNeutrinoTauDefinition();
}

/********************************************************************/


void G4IceTopMuonPhysics::ConstructProcess()
{

  G4ProcessManager *pManager = 0;

  // Muon Plus Physics
  pManager = G4MuonPlus::MuonPlus()->GetProcessManager();
   
  pManager->AddProcess(&muPlusMultipleScattering_, -1, 1, 1);
  pManager->AddProcess(&muPlusIonisation_,         -1, 2, 2);
  pManager->AddProcess(&muPlusBremsstrahlung_,     -1, 3, 3);
  pManager->AddProcess(&muPlusPairProduction_,     -1, 4, 4);

  // Muon Minus Physics
  pManager = G4MuonMinus::MuonMinus()->GetProcessManager();
   
  pManager->AddProcess(&muMinusMultipleScattering_, -1,  1, 1);
  pManager->AddProcess(&muMinusIonisation_,         -1,  2, 2);
  pManager->AddProcess(&muMinusBremsstrahlung_,     -1,  3, 3);
  pManager->AddProcess(&muMinusPairProduction_,     -1,  4, 4);

  pManager->AddRestProcess(&muMinusCaptureAtRest_);

  // Tau Plus Physics
  pManager = G4TauPlus::TauPlus()->GetProcessManager();
   
  pManager->AddProcess(&tauPlusMultipleScattering_, -1, 1, 1);
  pManager->AddProcess(&tauPlusIonisation_,         -1, 2, 2);

  // Tau Minus Physics
  pManager = G4TauMinus::TauMinus()->GetProcessManager();

  pManager->AddProcess(&tauMinusMultipleScattering_, -1, 1, 1);
  pManager->AddProcess(&tauMinusIonisation_,         -1, 2, 2);
}
