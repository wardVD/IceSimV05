/*
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: G4IceTopHadronPhysics.cxx 86420 2012-03-20 16:00:37Z kislat $
 *
 * @version $Revision: 86420 $
 * @date $LastChangedDate: 2012-03-20 11:00:37 -0500 (Tue, 20 Mar 2012) $
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy: kislat $
 *
 * Copied from tanktop
 */

#include <iomanip>   

#include <globals.hh>
#include <G4ios.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4MesonConstructor.hh>
#include <G4BaryonConstructor.hh>
#include <G4ShortLivedConstructor.hh>
#include <G4ProcessManager.hh>

#include "G4IceTopHadronPhysics.h"

G4IceTopHadronPhysics::G4IceTopHadronPhysics()
  :  G4VPhysicsConstructor("hadron") {
}

/********************************************************************/

G4IceTopHadronPhysics::~G4IceTopHadronPhysics()
{
  delete stringDecay_;
}

/********************************************************************/

void G4IceTopHadronPhysics::ConstructParticle()
{

  //  Construct all mesons
  G4MesonConstructor mesonConstructor;
  mesonConstructor.ConstructParticle();

  //  Construct all barions
  G4BaryonConstructor baryonConstructor;
  baryonConstructor.ConstructParticle();

  //  Construct resonaces and quarks
  G4ShortLivedConstructor shortLivedConstructor;
  shortLivedConstructor.ConstructParticle();  

}

/********************************************************************/

void G4IceTopHadronPhysics::ConstructProcess()
{
  G4ProcessManager *pManager = 0;

  // G4cout << "" << G4endl;
  // G4cout << "You are using the G4IceTopHadronPhysics" << G4endl;
  // G4cout << " - Note that this hadronic physics list is not optimized for any particular usage" << G4endl;
  // G4cout << " - If you wish to have a starting point tailored for a particular area of work," << G4endl;
  // G4cout << "   please use one of the available physics lists by use-case." << G4endl;
  // G4cout << "   More information can also be found from the Geant4 HyperNews." << G4endl;
  // G4cout << "" << G4endl;

  // Elastic Process
  elasticModel_ = new G4LElastic();
  elasticProcess_.RegisterMe(elasticModel_);

  // pi+ and pi-
  preEquilib_ = new G4PreCompoundModel(&excitationHandler_);
  cascade_.SetDeExcitation(preEquilib_);  
  theoModel_.SetTransport(&cascade_);
  theoModel_.SetHighEnergyGenerator(&stringModel_);
  stringDecay_ = new G4ExcitedStringDecay(&fragmentation_);
  stringModel_.SetFragmentationModel(stringDecay_);
  theoModel_.SetMinEnergy(15*GeV);
  theoModel_.SetMaxEnergy(100*TeV);

  // PionPlus
  pManager = G4PionPlus::PionPlus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  lePionPlusModel_ = new G4LEPionPlusInelastic();
  pionPlusInelastic_.RegisterMe(lePionPlusModel_);
  pionPlusInelastic_.RegisterMe(&theoModel_);
  pManager->AddDiscreteProcess(&pionPlusInelastic_);

  pManager->AddProcess(&pionPlusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&pionPlusMult_);
  pManager->SetProcessOrdering(&pionPlusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&pionPlusMult_, idxPostStep, 1);

  // PionMinus
  pManager = G4PionMinus::PionMinus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  lePionMinusModel_ = new G4LEPionMinusInelastic();
  pionMinusInelastic_.RegisterMe(lePionMinusModel_);
  pionMinusInelastic_.RegisterMe(&theoModel_);
  pManager->AddDiscreteProcess(&pionMinusInelastic_);

  pManager->AddProcess(&pionMinusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&pionMinusMult_);
  pManager->SetProcessOrdering(&pionMinusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&pionMinusMult_, idxPostStep, 1);

  pManager->AddRestProcess(&pionMinusAbsorption_, ordDefault);

  // KaonPlus
  pManager = G4KaonPlus::KaonPlus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leKaonPlusModel_ = new G4LEKaonPlusInelastic();
  heKaonPlusModel_ = new G4HEKaonPlusInelastic();
  kaonPlusInelastic_.RegisterMe(leKaonPlusModel_);
  kaonPlusInelastic_.RegisterMe(&theoModel_);
  pManager->AddDiscreteProcess(&kaonPlusInelastic_);

  pManager->AddProcess(&kaonPlusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&kaonPlusMult_);
  pManager->SetProcessOrdering(&kaonPlusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&kaonPlusMult_, idxPostStep, 1);

  // KaonMinus
  pManager = G4KaonMinus::KaonMinus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leKaonMinusModel_ = new G4LEKaonMinusInelastic();
  heKaonMinusModel_ = new G4HEKaonMinusInelastic();
  kaonMinusInelastic_.RegisterMe(leKaonMinusModel_);
  kaonMinusInelastic_.RegisterMe(heKaonMinusModel_);
  pManager->AddDiscreteProcess(&kaonMinusInelastic_);

  pManager->AddProcess(&kaonMinusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&kaonMinusMult_);
  pManager->SetProcessOrdering(&kaonMinusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&kaonMinusMult_, idxPostStep, 1);

  pManager->AddRestProcess(&kaonMinusAbsorption_, ordDefault);

  // KaonZeroL
  pManager = G4KaonZeroLong::KaonZeroLong()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leKaonZeroLModel_ = new G4LEKaonZeroLInelastic();
  heKaonZeroLModel_ = new G4HEKaonZeroInelastic();
  kaonZeroLInelastic_.RegisterMe(leKaonZeroLModel_);
  kaonZeroLInelastic_.RegisterMe(heKaonZeroLModel_);
  pManager->AddDiscreteProcess(&kaonZeroLInelastic_);
 
  // KaonZeroS
  pManager = G4KaonZeroShort::KaonZeroShort()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leKaonZeroSModel_ = new G4LEKaonZeroSInelastic();
  heKaonZeroSModel_ = new G4HEKaonZeroInelastic();
  kaonZeroSInelastic_.RegisterMe(leKaonZeroSModel_);
  kaonZeroSInelastic_.RegisterMe(heKaonZeroSModel_);
  pManager->AddDiscreteProcess(&kaonZeroSInelastic_);

  // Proton
  pManager = G4Proton::Proton()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leProtonModel_ = new G4LEProtonInelastic();
  heProtonModel_ = new G4HEProtonInelastic();
  protonInelastic_.RegisterMe(leProtonModel_);
  protonInelastic_.RegisterMe(&theoModel_);
  pManager->AddDiscreteProcess(&protonInelastic_);

  pManager->AddProcess(&protonIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&protonMult_);
  pManager->SetProcessOrdering(&protonMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&protonMult_, idxPostStep, 1);

  // anti-Proton
  pManager = G4AntiProton::AntiProton()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leAntiProtonModel_ = new G4LEAntiProtonInelastic();
  heAntiProtonModel_ = new G4HEAntiProtonInelastic();
  antiProtonInelastic_.RegisterMe(leAntiProtonModel_);
  antiProtonInelastic_.RegisterMe(heAntiProtonModel_);
  pManager->AddDiscreteProcess(&antiProtonInelastic_);

  pManager->AddProcess(&antiProtonIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&antiProtonMult_);
  pManager->SetProcessOrdering(&antiProtonMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&antiProtonMult_, idxPostStep, 1);

  pManager->AddRestProcess(&antiProtonAnnihilation_);

  // Neutron
  pManager = G4Neutron::Neutron()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leNeutronModel_ = new G4LENeutronInelastic();
  heNeutronModel_ = new G4HENeutronInelastic();
  neutronInelastic_.RegisterMe(leNeutronModel_);
  neutronInelastic_.RegisterMe(&theoModel_);
  pManager->AddDiscreteProcess(&neutronInelastic_);
  
  neutronFissionModel_ = new G4LFission();
  neutronFission_.RegisterMe(neutronFissionModel_);
  pManager->AddDiscreteProcess(&neutronFission_);

  neutronCaptureModel_ = new G4LCapture();
  neutronCapture_.RegisterMe(neutronCaptureModel_);
  pManager->AddDiscreteProcess(&neutronCapture_);

  // AntiNeutron
  pManager = G4AntiNeutron::AntiNeutron()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leAntiNeutronModel_ = new G4LEAntiNeutronInelastic();
  heAntiNeutronModel_ = new G4HEAntiNeutronInelastic();
  antiNeutronInelastic_.RegisterMe(leAntiNeutronModel_);
  antiNeutronInelastic_.RegisterMe(heAntiNeutronModel_);
  pManager->AddDiscreteProcess(&antiNeutronInelastic_);
    
  pManager->AddRestProcess(&antiNeutronAnnihilation_);

  // Lambda
  pManager = G4Lambda::Lambda()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leLambdaModel_ = new G4LELambdaInelastic();
  heLambdaModel_ = new G4HELambdaInelastic();
  lambdaInelastic_.RegisterMe(leLambdaModel_);
  lambdaInelastic_.RegisterMe(heLambdaModel_);
  pManager->AddDiscreteProcess(&lambdaInelastic_);
  
  // AntiLambda
  pManager = G4AntiLambda::AntiLambda()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leAntiLambdaModel_ = new G4LEAntiLambdaInelastic();
  heAntiLambdaModel_ = new G4HEAntiLambdaInelastic();
  antiLambdaInelastic_.RegisterMe(leAntiLambdaModel_);
  antiLambdaInelastic_.RegisterMe(heAntiLambdaModel_);
  pManager->AddDiscreteProcess(&antiLambdaInelastic_);
    
  // SigmaMinus
  pManager = G4SigmaMinus::SigmaMinus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leSigmaMinusModel_ = new G4LESigmaMinusInelastic();
  heSigmaMinusModel_ = new G4HESigmaMinusInelastic();
  sigmaMinusInelastic_.RegisterMe(leSigmaMinusModel_);
  sigmaMinusInelastic_.RegisterMe(heSigmaMinusModel_);
  pManager->AddDiscreteProcess(&sigmaMinusInelastic_);

  pManager->AddProcess(&sigmaMinusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&sigmaMinusMult_);
  pManager->SetProcessOrdering(&sigmaMinusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&sigmaMinusMult_, idxPostStep, 1);

  // anti-SigmaMinus
  pManager = G4AntiSigmaMinus::AntiSigmaMinus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leAntiSigmaMinusModel_ = new G4LEAntiSigmaMinusInelastic();
  heAntiSigmaMinusModel_ = new G4HEAntiSigmaMinusInelastic();
  antiSigmaMinusInelastic_.RegisterMe(leAntiSigmaMinusModel_);
  antiSigmaMinusInelastic_.RegisterMe(heAntiSigmaMinusModel_);
  pManager->AddDiscreteProcess(&antiSigmaMinusInelastic_);

  pManager->AddProcess(&antiSigmaMinusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&antiSigmaMinusMult_);
  pManager->SetProcessOrdering(&antiSigmaMinusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&antiSigmaMinusMult_, idxPostStep, 1);

  // SigmaPlus
  pManager = G4SigmaPlus::SigmaPlus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leSigmaPlusModel_ = new G4LESigmaPlusInelastic();
  heSigmaPlusModel_ = new G4HESigmaPlusInelastic();
  sigmaPlusInelastic_.RegisterMe(leSigmaPlusModel_);
  sigmaPlusInelastic_.RegisterMe(heSigmaPlusModel_);
  pManager->AddDiscreteProcess(&sigmaPlusInelastic_);

  pManager->AddProcess(&sigmaPlusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&sigmaPlusMult_);
  pManager->SetProcessOrdering(&sigmaPlusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&sigmaPlusMult_, idxPostStep, 1);

  // anti-SigmaPlus
  pManager = G4AntiSigmaPlus::AntiSigmaPlus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leAntiSigmaPlusModel_ = new G4LEAntiSigmaPlusInelastic();
  heAntiSigmaPlusModel_ = new G4HEAntiSigmaPlusInelastic();
  antiSigmaPlusInelastic_.RegisterMe(leAntiSigmaPlusModel_);
  antiSigmaPlusInelastic_.RegisterMe(heAntiSigmaPlusModel_);
  pManager->AddDiscreteProcess(&antiSigmaPlusInelastic_);

  pManager->AddProcess(&antiSigmaPlusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&antiSigmaPlusMult_);
  pManager->SetProcessOrdering(&antiSigmaPlusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&antiSigmaPlusMult_, idxPostStep, 1);

  // XiMinus
  pManager = G4XiMinus::XiMinus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leXiMinusModel_ = new G4LEXiMinusInelastic();
  heXiMinusModel_ = new G4HEXiMinusInelastic();
  xiMinusInelastic_.RegisterMe(leXiMinusModel_);
  xiMinusInelastic_.RegisterMe(heXiMinusModel_);
  pManager->AddDiscreteProcess(&xiMinusInelastic_);

  pManager->AddProcess(&xiMinusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&xiMinusMult_);
  pManager->SetProcessOrdering(&xiMinusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&xiMinusMult_, idxPostStep, 1);

  // anti-XiMinus
  pManager = G4AntiXiMinus::AntiXiMinus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leAntiXiMinusModel_ = new G4LEAntiXiMinusInelastic();
  heAntiXiMinusModel_ = new G4HEAntiXiMinusInelastic();
  antiXiMinusInelastic_.RegisterMe(leAntiXiMinusModel_);
  antiXiMinusInelastic_.RegisterMe(heAntiXiMinusModel_);
  pManager->AddDiscreteProcess(&antiXiMinusInelastic_);

  pManager->AddProcess(&antiXiMinusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&antiXiMinusMult_);
  pManager->SetProcessOrdering(&antiXiMinusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&antiXiMinusMult_, idxPostStep, 1);

  // XiZero
  pManager = G4XiZero::XiZero()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leXiZeroModel_ = new G4LEXiZeroInelastic();
  heXiZeroModel_ = new G4HEXiZeroInelastic();
  xiZeroInelastic_.RegisterMe(leXiZeroModel_);
  xiZeroInelastic_.RegisterMe(heXiZeroModel_);
  pManager->AddDiscreteProcess(&xiZeroInelastic_);

  // anti-XiZero
  pManager = G4AntiXiZero::AntiXiZero()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leAntiXiZeroModel_ = new G4LEAntiXiZeroInelastic();
  heAntiXiZeroModel_ = new G4HEAntiXiZeroInelastic();
  antiXiZeroInelastic_.RegisterMe(leAntiXiZeroModel_);
  antiXiZeroInelastic_.RegisterMe(heAntiXiZeroModel_);
  pManager->AddDiscreteProcess(&antiXiZeroInelastic_);

  // OmegaMinus
  pManager = G4OmegaMinus::OmegaMinus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leOmegaMinusModel_ = new G4LEOmegaMinusInelastic();
  heOmegaMinusModel_ = new G4HEOmegaMinusInelastic();
  omegaMinusInelastic_.RegisterMe(leOmegaMinusModel_);
  omegaMinusInelastic_.RegisterMe(heOmegaMinusModel_);
  pManager->AddDiscreteProcess(&omegaMinusInelastic_);

  pManager->AddProcess(&omegaMinusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&omegaMinusMult_);
  pManager->SetProcessOrdering(&omegaMinusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&omegaMinusMult_, idxPostStep, 1);

  // anti-OmegaMinus
  pManager = G4AntiOmegaMinus::AntiOmegaMinus()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&elasticProcess_);

  leAntiOmegaMinusModel_ = new G4LEAntiOmegaMinusInelastic();
  heAntiOmegaMinusModel_ = new G4HEAntiOmegaMinusInelastic();
  antiOmegaMinusInelastic_.RegisterMe(leAntiOmegaMinusModel_);
  antiOmegaMinusInelastic_.RegisterMe(heAntiOmegaMinusModel_);
  pManager->AddDiscreteProcess(&antiOmegaMinusInelastic_);

  pManager->AddProcess(&antiOmegaMinusIonisation_, ordInActive,2, 2);

  pManager->AddProcess(&antiOmegaMinusMult_);
  pManager->SetProcessOrdering(&antiOmegaMinusMult_, idxAlongStep, 1);
  pManager->SetProcessOrdering(&antiOmegaMinusMult_, idxPostStep, 1);

}
