/**
 * copyright  (C) 2012
 * the icecube collaboration
 * @version $Id: $
 * @file WimpSimTools.cxx
 * @date $Date: 2012-11-18$
 * @author mzoll <marcel.zoll@fysik.su.se>
 * this is a library that provides definitions and functions important to read WimpSim
 */

#include <math.h>
#include "dataclasses/I3Constants.h"
#include "wimpsim-reader/WimpSimTools.h"

namespace WimpSimTools {
  using namespace WimpSim;
  
  //_______________________________________________________________________
  double IceCubeAziRadFromWimpSimAziRad(const double wimpsim_azi) {
    if (wimpsim_azi>2*I3Constants::pi || wimpsim_azi<0.)
      log_error("Encountered a negative or bigger than 2*pi WimpSim.azi= %f. This is not physical", wimpsim_azi);
    double icecube_azi = 0.5*I3Constants::pi - wimpsim_azi;
    if (icecube_azi < 0.)
      icecube_azi += 2.0*I3Constants::pi;
    return icecube_azi;
  }
  //_______________________________________________________________________
  double IceCubeZenRadFromWimpSimElRad(const double wimpsim_el) {
    if (wimpsim_el<-I3Constants::pi*0.5 || wimpsim_el>I3Constants::pi*0.5)
      log_error("Encountered a smaller than -pi/2 or bigger than +pi/2 WimpSim.elevation= %f. This is not physical", wimpsim_el);
    return 0.5*I3Constants::pi - wimpsim_el;
  }
  //_______________________________________________________________________
  double IceCubeZenRadFromWimpSimNadirRad(const double wimpsim_nadir) {
    if (wimpsim_nadir>I3Constants::pi || wimpsim_nadir<0.)
      log_error("Encountered a negative or bigger than pi WimpSim.nadir= %f. This is not physical", wimpsim_nadir);
    return I3Constants::pi - wimpsim_nadir;
  }
  //_______________________________________________________________________
  double GetParticleMass (const I3Particle::ParticleType partType) {
    //HACK had to hardcode constants in sim-services/I3SimConstants.h
    if (partType == I3Particle::EMinus || partType == I3Particle::EPlus)
      return 0.510998928 * I3Units::MeV / (I3Constants::c * I3Constants::c); // I3SimConstants::m_e;
    else if (partType == I3Particle::MuMinus || partType == I3Particle::MuPlus)
      return 105.6583715 * I3Units::MeV / (I3Constants::c * I3Constants::c); // I3SimConstants::m_mu;
    else if (partType == I3Particle::TauMinus || partType == I3Particle::TauPlus)
      return 1776.82 * I3Units::MeV / (I3Constants::c * I3Constants::c); // I3SimConstants::m_tau;
    else if (partType == I3Particle::PMinus || partType == I3Particle::PPlus)
      return 938.272046 * I3Units::MeV / (I3Constants::c * I3Constants::c); // I3SimConstants::m_p;
    else if (partType == I3Particle::Neutron)
      return 939.565379 * I3Units::MeV / (I3Constants::c * I3Constants::c); // I3SimConstants::m_n
    else if (partType == I3Particle::NuE
      || partType == I3Particle::NuEBar
      || partType == I3Particle::NuMu
      || partType == I3Particle::NuMuBar
      || partType == I3Particle::NuTau
      || partType == I3Particle::NuTauBar
    ) {
      return 0.2 * I3Units::eV / (I3Constants::c * I3Constants::c);
   } // http://pdg.lbl.gov/2012/listings/rpp2012-list-neutrino-prop.pdf;
    else if (partType == I3Particle::Hadrons) {
      log_debug("can not return mass for a general Hadron; falling back zero");
      return 0.;
    }
    else {
      log_fatal("unrecognized I3ParticleType::'%d': Cannot return a mass.", partType);
      return NAN;
    }
  };
  //_______________________________________________________________________
  double TotEnergyToKinEnergy(const double tot_energy, const I3Particle::ParticleType partType) {
    if (partType == I3Particle::Hadrons) { //per definition we have to return the total energy here and let downstream simulation take care of that
      return tot_energy;
    }
    const double mass = GetParticleMass(partType);
    const double mass_energy = mass * I3Constants::c * I3Constants::c;

    if (mass_energy>tot_energy) {
      log_error("Energy of the mass term exceeds the total energy of this particle: %f > %f", mass_energy, tot_energy);
      return 0.;
    }
    const double kin_energy2 = tot_energy*tot_energy - mass_energy*mass_energy;

    return sqrt(kin_energy2);
  };
  WimpSimBaseEvent::~WimpSimBaseEvent() {}
  //_______________________________________________________________________
  WimpSimSunEvent::~WimpSimSunEvent() {}

  double WimpSimSunEvent::GetIceCubeNu_Azimuth() const {return IceCubeAziRadFromWimpSimAziRad(nu_az_*I3Units::degree);}
  double WimpSimSunEvent::GetIceCubeNu_Zenith() const {return IceCubeZenRadFromWimpSimElRad(nu_el_*I3Units::degree);}

  double WimpSimSunEvent::GetIceCubeLep_Azimuth() const {return IceCubeAziRadFromWimpSimAziRad(lep_az_*I3Units::degree);}
  double WimpSimSunEvent::GetIceCubeLep_Zenith() const {return IceCubeZenRadFromWimpSimElRad(lep_el_*I3Units::degree);}

  double WimpSimSunEvent::GetIceCubeHad_Azimuth() const {return IceCubeAziRadFromWimpSimAziRad(had_az_*I3Units::degree);}
  double WimpSimSunEvent::GetIceCubeHad_Zenith() const {return IceCubeZenRadFromWimpSimElRad(had_el_*I3Units::degree);}

  double WimpSimSunEvent::GetIceCubeOrigin_Azimuth() const {return IceCubeAziRadFromWimpSimAziRad(sun_az_*I3Units::degree);}
  double WimpSimSunEvent::GetIceCubeOrigin_Zenith() const {return IceCubeZenRadFromWimpSimElRad(sun_el_*I3Units::degree);}

  double WimpSimSunEvent::GetMJD() const {return mjd_;}
  //_______________________________________________________________________
  WimpSimEarthEvent::~WimpSimEarthEvent() {}

  double WimpSimEarthEvent::GetIceCubeNu_Azimuth() const{return IceCubeAziRadFromWimpSimAziRad(nu_az_*I3Units::degree);}
  double WimpSimEarthEvent::GetIceCubeNu_Zenith() const{return IceCubeZenRadFromWimpSimNadirRad(nu_nadir_*I3Units::degree);}

  double WimpSimEarthEvent::GetIceCubeLep_Azimuth() const{return IceCubeAziRadFromWimpSimAziRad(lep_az_*I3Units::degree);}
  double WimpSimEarthEvent::GetIceCubeLep_Zenith() const{return IceCubeZenRadFromWimpSimNadirRad(lep_nadir_*I3Units::degree);}

  double WimpSimEarthEvent::GetIceCubeHad_Azimuth() const{return IceCubeAziRadFromWimpSimAziRad(had_az_*I3Units::degree);}
  double WimpSimEarthEvent::GetIceCubeHad_Zenith() const{return IceCubeZenRadFromWimpSimNadirRad(had_nadir_*I3Units::degree);};

  double WimpSimEarthEvent::GetIceCubeOrigin_Azimuth() const {return GetIceCubeNu_Azimuth();}
  double WimpSimEarthEvent::GetIceCubeOrigin_Zenith() const {return GetIceCubeNu_Zenith();}

  double WimpSimEarthEvent::GetMJD() const {return NAN;}
}
