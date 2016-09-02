/**
 * Copyright  (C) 2005
 * The IceCube Collaboration
 * $Id: $
 *
 * @file InteractionGR.h
 * @version $Revision: $
 * @date    $Date:     $ 
 * @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 * @brief InteractionGR header file
 * Grashow Resonance Crosssection implimentation 
 * for I3NeutrinoGenerator 
 */
////////////////////////////////////////////////////////////////////////
// here you find all the final state classes for processes in the     //
// Standard Model. The default data is supplied  through tables.      //
// The cross section part is hardcoded in this file.


#include "neutrino-generator/interactions/InteractionBase.h"
#include "neutrino-generator/interactions/InteractionGR.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/utils/Constants.h"
#include "dataclasses/I3Constants.h"


using namespace std;

namespace nugen {
using namespace Constants;

////////////////////////////////////////////////////////////////////////
InteractionGR::InteractionGR(I3RandomServicePtr random,
                             SteeringPtr steer):
  InteractionBase(random, steer)
{
  log_debug("Constructing GR interaction");
}
////////////////////////////////////////////////////////////////////////
InteractionGR::~InteractionGR()
{
  log_trace("deconstruct InteractionGR");
}
////////////////////////////////////////////////////////////////////////
void InteractionGR::FillDaughterParticles(ParticlePtr nuin_ptr, 
                                          double)
{
  // first obtain the incoming neutrino: nu_in_ptr
  
  //I3Position secondary_production = nuin->GetInteractionPosition();
  //I3Position intersection = nuin->GetIntersectionPosition();
  //I3Position end = nuin->GetExitPosition();
  // now get energy and id
  //TLorentzVector nu_momentum = nuin->GetFourMomentum();
  // now create the final state
  double finalstate[7]={0.,0.,0.,0.,0.,0.,0.}; // init. final state kinematic
  FillResonanceStates(finalstate);
  int n_daughters=0;
  // make the final state particles
  // 0) anu_e; 1) nu_e; 2) anu_mu; 3) nu_mu; 4) nu_tau; 5) tau; 6) hadrons;
  /*
        addData( tb, "e-"       ,    11, 0.0   , -1. ,      -1. ,  0.5);
        addData( tb, "nue_bar"  ,   -12, 0.0   ,  0. ,      -1, ,  0.5);
        addData( tb, "mu-"      ,    13, 0.106 , -1. ,  2.197e-6,  0.5);
	addData( tb, "numu_bar" ,   -14, 0.0   ,  0. ,      -1. ,  0.5);
        addData( tb, "tau-"     ,    15, 1.777 , -1. , 2.906e-13,  0.5);
	addData( tb, "nutau_bar",   -16, 0.0   ,  0. ,      -1. ,  0.5);
        addData( tb, "pi+"      ,   211, 0.1396, +1. , 2.6033e-8,    1);
        addData( tb, "pi-"      ,  -211, 0.1396, -1. , 2.6033e-8,    1);
*/
  if(finalstate[0] > 0) {
    //create neutrino
    ParticlePtr neutrino_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
    ++n_daughters;
    neutrino_daughter_particle->SetShape(I3Particle::Null);//4
    neutrino_daughter_particle->SetFitStatus(I3Particle::NotSet);//5
    neutrino_daughter_particle->SetPos(nuin_ptr->GetEndPosition());
    neutrino_daughter_particle->SetTime(nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed());//7
    neutrino_daughter_particle->SetDir(nuin_ptr->GetDir());//8
    neutrino_daughter_particle->SetLength(NAN);//9
    neutrino_daughter_particle->SetSpeed(I3Constants::c);//10
    neutrino_daughter_particle->SetType(I3Particle::NuEBar);//anti-neutrino_electron
    neutrino_daughter_particle->SetEnergy(finalstate[0]*nuin_ptr->GetEnergy());
    neutrino_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

    //create charged_lepton
    ParticlePtr charged_lepton_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
    ++n_daughters;
    charged_lepton_daughter_particle->SetShape(I3Particle::Cascade);
    charged_lepton_daughter_particle->SetFitStatus(I3Particle::NotSet);
    charged_lepton_daughter_particle->SetPos(nuin_ptr->GetEndPosition());
    charged_lepton_daughter_particle->SetTime(nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed());//7
    charged_lepton_daughter_particle->SetDir(nuin_ptr->GetDir());//8
    charged_lepton_daughter_particle->SetLength(NAN);//9
    charged_lepton_daughter_particle->SetSpeed(I3Constants::c);//10
    charged_lepton_daughter_particle->SetType(I3Particle::EMinus);
    charged_lepton_daughter_particle->SetEnergy(finalstate[1]*nuin_ptr->GetEnergy());
    charged_lepton_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

    nuin_ptr->AddDaughter(neutrino_daughter_particle);
    nuin_ptr->AddDaughter(charged_lepton_daughter_particle);
    
  } else if (finalstate[2] > 0) {

    //create neutrino
    ParticlePtr neutrino_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
    ++n_daughters;
    neutrino_daughter_particle->SetShape(I3Particle::Null);
    neutrino_daughter_particle->SetFitStatus(I3Particle::NotSet);
    neutrino_daughter_particle->SetPos(nuin_ptr->GetEndPosition());
    neutrino_daughter_particle->SetTime(nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed());//7
    neutrino_daughter_particle->SetDir(nuin_ptr->GetDir());//8
    neutrino_daughter_particle->SetLength(NAN);//9
    neutrino_daughter_particle->SetSpeed(I3Constants::c);//10
    neutrino_daughter_particle->SetType(I3Particle::NuMuBar);
    neutrino_daughter_particle->SetEnergy(finalstate[2]*nuin_ptr->GetEnergy());
    neutrino_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

    //create charged_lepton
    ParticlePtr charged_lepton_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
    ++n_daughters;
    charged_lepton_daughter_particle->SetShape(I3Particle::Null);
    charged_lepton_daughter_particle->SetFitStatus(I3Particle::NotSet);
    charged_lepton_daughter_particle->SetPos(nuin_ptr->GetEndPosition());
    charged_lepton_daughter_particle->SetTime(nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed());//7
    charged_lepton_daughter_particle->SetDir(nuin_ptr->GetDir());//8
    charged_lepton_daughter_particle->SetLength(NAN);//9
    charged_lepton_daughter_particle->SetSpeed(I3Constants::c);//10
    charged_lepton_daughter_particle->SetType(I3Particle::MuMinus);
    charged_lepton_daughter_particle->SetEnergy(finalstate[3]*nuin_ptr->GetEnergy());
    charged_lepton_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

    nuin_ptr->AddDaughter(neutrino_daughter_particle);
    nuin_ptr->AddDaughter(charged_lepton_daughter_particle);

  } else if (finalstate[4] > 0) {

   //create neutrino
    ParticlePtr neutrino_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
    ++n_daughters;
    neutrino_daughter_particle->SetShape(I3Particle::Null);
    neutrino_daughter_particle->SetFitStatus(I3Particle::NotSet);
    neutrino_daughter_particle->SetPos(nuin_ptr->GetEndPosition());
    neutrino_daughter_particle->SetTime(nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed());//7
    neutrino_daughter_particle->SetDir(nuin_ptr->GetDir());//8
    neutrino_daughter_particle->SetLength(NAN);//9
    neutrino_daughter_particle->SetSpeed(I3Constants::c);//10
    neutrino_daughter_particle->SetType(I3Particle::NuTauBar);
    neutrino_daughter_particle->SetEnergy(finalstate[4]*nuin_ptr->GetEnergy());
    neutrino_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

    //create charged_lepton
    ParticlePtr charged_lepton_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
    ++n_daughters;
    charged_lepton_daughter_particle->SetShape(I3Particle::Null);
    charged_lepton_daughter_particle->SetFitStatus(I3Particle::NotSet);
    charged_lepton_daughter_particle->SetPos(nuin_ptr->GetEndPosition());
    charged_lepton_daughter_particle->SetTime(nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed());//7
    charged_lepton_daughter_particle->SetDir(nuin_ptr->GetDir());//8
    charged_lepton_daughter_particle->SetLength(NAN);//9
    charged_lepton_daughter_particle->SetSpeed(I3Constants::c);//10
    charged_lepton_daughter_particle->SetType(I3Particle::TauMinus);
    charged_lepton_daughter_particle->SetEnergy(finalstate[5]*nuin_ptr->GetEnergy());
    charged_lepton_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

    nuin_ptr->AddDaughter(neutrino_daughter_particle);
    nuin_ptr->AddDaughter(charged_lepton_daughter_particle);

  } else {
    ParticlePtr hadron_daughter_particle(new Particle(I3Particle::Null, I3Particle::unknown, steer_));
    ++n_daughters;
    hadron_daughter_particle->SetShape(I3Particle::Cascade);
    hadron_daughter_particle->SetFitStatus(I3Particle::NotSet);
    hadron_daughter_particle->SetPos(nuin_ptr->GetEndPosition());
    hadron_daughter_particle->SetTime(nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed());//7
    hadron_daughter_particle->SetDir(nuin_ptr->GetDir());//8
    hadron_daughter_particle->SetLength(NAN);//9
    hadron_daughter_particle->SetSpeed(I3Constants::c);//10
    hadron_daughter_particle->SetType(I3Particle::Hadrons);
    hadron_daughter_particle->SetEnergy(finalstate[6]*nuin_ptr->GetEnergy());
    hadron_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

    nuin_ptr->AddDaughter(hadron_daughter_particle);
  }

  log_trace(" # We have %d daughter particles", nuin_ptr->GetTotalNDaughters());

}

//_________________________________________________________________
double InteractionGR::GetXsecCGS(const double energy) 
{
  double sigma=0.0;
  const double crs0 = GF2*MW2/I3Constants::pi*GeV2_MBARN; // Standard cross-section in mbarn
  //Sep. 16 2014 this is old parameter.
  //const double m_electron = 5.10999E-4;// which is Electron mass in GeV
  const double SW = 2*M_e*energy/MW2;
  // The total width is devided by the lepton (here muon) one 1/RWmu
  //branching ratios RWe, RWmu, RWtau
  sigma = crs0*SW/( (1 - SW)*(1 - SW) + GW2)/RWmu/3; 

  // conversion : mb to cm2
  return sigma * 1e-27;
}

////////////////////////////////////////////////////////////////////////
// Choose the W-boson decay channal
////////////////////////////////////////////////////////////////////////
InteractionGR::GRDecayChannel InteractionGR::ChooseChannel(){
  log_trace("Branching ratios RWe, RWmu, RWtau are defined in AnisGlobal.hh");
  // decay channel initialized by 0
  GRDecayChannel ichan = NOTSET_GR;
  double p1 = RWe;
  double p2 = p1 + RWmu;
  double p3 = p2 + RWtau;
  double rndm = random_->Uniform( 0.0, 1.0 );
  if      ( rndm <= p1 ) ichan = ELECTRON_GR;  // electron
  else if ( rndm <= p2 ) ichan = MUON_GR;      // muon
  else if ( rndm <= p3 ) ichan = TAU_GR;       // tau
  else if ( rndm <=  1 ) ichan = HADRON_GR;    // hadrons
  else 
    log_error(" Something is wrong with ichan assignment !!! ");
  return ichan;
}
////////////////////////////////////////////////////////////////////////
// This function fills the final state of W-boson decay:
// It depends on 2 random numbers and returns the array of 7 energy fractions
// Difinition of the array coloumns:
// 0) anu_e; 1) nu_e; 2) anu_mu; 3) nu_mu; 4) nu_tau; 5) tau; 6) hadrons;
////////////////////////////////////////////////////////////////////////
void InteractionGR::FillResonanceStates(double fstate[]){
  log_trace("This function fills the final state of W-boson decay");
  int j;
  double y, y1;
  InteractionGR::GRDecayChannel ichan =  ChooseChannel();         // channal of decay
  
  // throw the fraction of the transferred energy
  double  rndm_y = random_->Uniform( 0.0, 1.0 );
  // fraction of energy transferred to the final antineutrino
  y1 = pow(rndm_y, 1./3);            
  // fraction of energy transferred to the final lepton (e.g. $\mu^-$)
  y  = 1 - y1;                   
  for (j=0; j < 7; j++)
    fstate[j] = 0;
  // Choose the decay channal according to ichan
  switch (ichan) {
  case ELECTRON_GR:  // electron
    fstate[0] = y1; fstate[1] = y;  break;
  case MUON_GR:  // muon
    fstate[2] = y1; fstate[3] = y;  break;
  case TAU_GR:  // tau
    fstate[4] = y1; fstate[5] = y;  break;
  case HADRON_GR:  // hadrons
    fstate[6] =  1;                 break;
  default:
    log_error(" Something is wrong with branching ratios" 
              " of the resonance W-decay");
  }
}

//////////////// End of the Glashow Resonance ////////////////////////////

}

