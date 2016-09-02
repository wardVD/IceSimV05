#ifndef PDGCODETOI3PARTICLETYPECONVERTER_H_INCLUDED
#define PDGCODETOI3PARTICLETYPECONVERTER_H_INCLUDED

#include "dataclasses/physics/I3Particle.h"

/**
 * Helper function: Converts PDG codes to I3Particle::Types
 */
namespace I3GENIE_Helper
{
    inline I3Particle::ParticleType ConvertPDGEncodingToI3ParticleType(int pdg_code)
    {
        switch (pdg_code)
        {
            case -3122:  return I3Particle::Hadrons; // DeltaBar
            case -2212:  return I3Particle::PMinus;
            case -2112:  return I3Particle::Hadrons; // NeutronBar
            case -321:   return I3Particle::KMinus;
            case -311:   return I3Particle::K0_Short; // not really right.. (K0)
            case -211:   return I3Particle::PiMinus;
            case -16:    return I3Particle::NuTauBar;
            case -15:    return I3Particle::TauPlus;
            case -14:    return I3Particle::NuMuBar;
            case -13:    return I3Particle::MuPlus;
            case -12:    return I3Particle::NuEBar;
            case -11:    return I3Particle::EPlus;
            case 0:      return I3Particle::unknown; // invalid
            case 11:     return I3Particle::EMinus;
            case 12:     return I3Particle::NuE;
            case 13:     return I3Particle::MuMinus;
            case 14:     return I3Particle::NuMu;
            case 15:     return I3Particle::TauMinus;
            case 16:     return I3Particle::NuTau;
            case 22:     return I3Particle::Gamma; 
            case 111:    return I3Particle::Pi0;
            case 130:    return I3Particle::K0_Long;
            case 211:    return I3Particle::PiPlus;
            case 310:    return I3Particle::K0_Short;
            case 311:    return I3Particle::K0_Long; // not really right.. (K0Bar)
            case 321:    return I3Particle::KPlus;
            //case 411:    return I3Particle::Hadrons; // D+
            //case 421:    return I3Particle::Hadrons; // D0
            //case 431:    return I3Particle::Hadrons; // (something else.. we should store PDG codes directly..)
            case 2112:   return I3Particle::Neutron;
            case 2212:   return I3Particle::PPlus;
            case 3112:   return I3Particle::Hadrons; // Sigma-
            case 3122:   return I3Particle::Hadrons; // Delta
            case 3212:   return I3Particle::Hadrons; // (something else.. we should store PDG codes directly..)
            case 3222:   return I3Particle::Hadrons; // Sigma+
            //case 4122:   return I3Particle::Hadrons; // (something else.. we should store PDG codes directly..)
            //case 4212:   return I3Particle::Hadrons; // (something else.. we should store PDG codes directly..)
            //case 4222:   return I3Particle::Hadrons; // (something else.. we should store PDG codes directly..)
            
            case 1000020040:  return I3Particle::He4Nucleus;
            case 1000030070:  return I3Particle::Li7Nucleus;
            case 1000040090:  return I3Particle::Be9Nucleus;
            case 1000050110:  return I3Particle::B11Nucleus;
            case 1000060120:  return I3Particle::C12Nucleus;
            case 1000070140:  return I3Particle::N14Nucleus;
            case 1000080160:  return I3Particle::O16Nucleus;
            case 1000090190:  return I3Particle::F19Nucleus;
            case 1000100200:  return I3Particle::Ne20Nucleus;
            case 1000110230:  return I3Particle::Na23Nucleus;
            case 1000120240:  return I3Particle::Mg24Nucleus;
            case 1000130270:  return I3Particle::Al27Nucleus;
            case 1000140280:  return I3Particle::Si28Nucleus;
            case 1000150310:  return I3Particle::P31Nucleus;
            case 1000160320:  return I3Particle::S32Nucleus;
            case 1000170350:  return I3Particle::Cl35Nucleus;
            case 1000180400:  return I3Particle::Ar40Nucleus;
            case 1000190390:  return I3Particle::K39Nucleus;
            case 1000200400:  return I3Particle::Ca40Nucleus;
            case 1000210450:  return I3Particle::Sc45Nucleus;
            case 1000220480:  return I3Particle::Ti48Nucleus;
            case 1000230510:  return I3Particle::V51Nucleus;
            case 1000240520:  return I3Particle::Cr52Nucleus;
            case 1000250550:  return I3Particle::Mn55Nucleus;
            case 1000260560:  return I3Particle::Fe56Nucleus;
                
            default: return I3Particle::unknown; // invalid/unknown
        }
    }
    
};

#endif //PDGCODETOI3PARTICLETYPECONVERTER_H_INCLUDED
