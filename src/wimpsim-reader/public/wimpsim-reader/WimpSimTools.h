/**
 * copyright  (C) 2012
 * the icecube collaboration
 * @version $Id: WimpSimTools.h xxxxx 2012-10-10 17:01:33Z mzoll $
 * @file WimpSimTools.h
 * @date $Date: 2012-12-20$
 * @author mzoll marcel.zoll@fysik.su.se
 * this is a library that provides definitions and functions important to read WimpSim
 */

#ifndef WIMPSIMTOOLS_H_INCLUDED
#define WIMPSIMTOOLS_H_INCLUDED

#include <string>
#include "dataclasses/physics/I3Particle.h"
#include "simclasses/I3WimpParams.h"
    
namespace WimpSimTools{
  /** @brief Convert the azimuth given by WimpSim to IceCube Definitions of Azimuth [in rad]
    @param wimpsim_azi : from WimpSim [0...2*pi]
    @return Azimuth in IC coordinates
  */
  double IceCubeAziRadFromWimpSimAziRad (const double wimpsim_azi);

  /** @brief Convert the elevation given by WimpSim to IceCube Definitions of Zenith [in rad]
    @param wimpsim_el : from WimpSim [0...pi]
    @return Zenith in IC coordinates
  */
  double IceCubeZenRadFromWimpSimElRad (const double wimpsim_el);
  
  /** @brief Convert the Nadir given by WimpSim to IceCube Definitions of Zenith [in rad]
    @param wimpsim_nadir : from WimpSim [0...pi]
    @return Zenith in IC coordinates
  */
  double IceCubeZenRadFromWimpSimNadirRad (const double wimpsim_nadir);
  
  /** @brief Returns the mass of a particle
   * @param partType type of that particle
   * @return the mass of that particle
   */
  double GetParticleMass (const I3Particle::ParticleType partType);
  
  /** @brief Convert the energy from total energy for this particle type to the kinetic energy
   * @param tot_energy total energy of that particle
   * @param partType type of that particle
   * @return kinematic energy for that particle 
   */
  double TotEnergyToKinEnergy(const double tot_energy, const I3Particle::ParticleType partType);
  
  /// encapsule all base information WimpSim provides for every Events
  class WimpSimBaseEvent{
  public:
    virtual ~WimpSimBaseEvent();

    /// number of the event
    uint eventnbr_;
    /// type of the primary neutrino (text)
    std::string nu_type_;
    /// total energy of the primary neutrino (in GeV)
    double nu_energy_;
    /// angle of the primary neutrino (in deg)
    double nu_weight_;
    /// type of the secondary lepton inice (text)
    std::string lep_type_;
    /// total energy of the secondary lepton inice (in GeV)
    double lep_energy_;
    /// weight of the secondary lepton inice
    double lep_weight_;
    /// type of the secondary hadron inice (text)
    std::string had_type_;
    /// total energy of the secondary hadron inice (in GeV)
    double had_energy_;
    /// angle of the secondary hadron inice (in deg)
    double had_weight_;
    /// nadir of primary neutrino (in deg)
    
    //methods: all are pure virtual and are declared in the child classes
    /// Implementation to Get Azimuth of the Primary Neutrino
    virtual double GetIceCubeNu_Azimuth() const=0;
    /// Implementation to Get Zenith of the Primary Neutrino
    virtual double GetIceCubeNu_Zenith() const=0;
    /// Implementation to Get Azimuth of the Secondary Lepton
    virtual double GetIceCubeLep_Azimuth() const=0;
    /// Implementation to Get Zenith of the Secondary Lepton
    virtual double GetIceCubeLep_Zenith() const=0;
    /// Implementation to Get Azimuth of the Secondary Hadron
    virtual double GetIceCubeHad_Azimuth() const=0;
    /// Implementation to Get Zenith of the Secondary Hadron
    virtual double GetIceCubeHad_Zenith() const=0;
    /// Implementation to Get Zenith of the Origin of this event
    virtual double GetIceCubeOrigin_Azimuth() const=0;
    /// Implementation to Get Zenith of the Origin of this event
    virtual double GetIceCubeOrigin_Zenith() const=0;
    /// Implementation to Get the MJD time of this event
    virtual double GetMJD() const=0;
  };
  
  /// encapsulate all information WimpSim Provides for Sun Events
  class WimpSimSunEvent:public WimpSimBaseEvent{
  public:
    virtual ~WimpSimSunEvent();

    /// mjd of the event
    double mjd_;
    /// nadir of sun (in deg)
    double sun_nadir_;
    /// sun right asscention (in deg)
    double sun_ra_;
    /// declination of the sun (in deg)
    double sun_dec_;
    /// azimuth of the sun (in deg)
    double sun_az_;
    /// elevation (zenith) of the sun (in deg)
    double sun_el_;
    /// angle of the primary neutrino (in deg)
    double nu_angle_;
    /// weight of the primary neutrino
    double lep_angle_;
    /// angle of the secondary hadron inice (in deg)
    double had_angle_;
    /// nadir of primary neutrino (in deg)
    double nu_nadir_;
    /// azimuth of primary neutrino (in deg)
    double nu_az_;
    ///  nadir of secondary lepton inice (in deg)
    double lep_nadir_;
    /// azimuth of secondary lepton inice (in deg)
    double lep_az_;
    /// nadir of secondary hadron inice (in deg)
    double had_nadir_;
    /// azimuth of secondary hadron inice (in deg)
    double had_az_;
    /// elevation of primary neutrino (in deg)
    double nu_el_;
    /// elevation of secondary lepton inice (in deg)
    double lep_el_;
    /// elevation of secondary hadron inice (in deg)
    double had_el_;
    
    //methods
    /// Get Azimuth of the Primary Neutrino
    double GetIceCubeNu_Azimuth() const;
    /// Get Zenith of the Primary Neutrino
    double GetIceCubeNu_Zenith() const;
    /// Get Azimuth of the Secondary Lepton
    double GetIceCubeLep_Azimuth() const;
    /// Get Zenith of the Secondary Lepton
    double GetIceCubeLep_Zenith() const;
    /// Get Azimuth of the Secondary Hadron
    double GetIceCubeHad_Azimuth() const;
    /// Get Zenith of the Secondary Hadron
    double GetIceCubeHad_Zenith() const;
    /// Get Azimuth of the Origin of this event
    double GetIceCubeOrigin_Azimuth() const;
    /// Get Zenith of the Origin of this event
    double GetIceCubeOrigin_Zenith() const;
    /// Get the MJD time of this event
    double GetMJD() const;
  };

  /// encapsulate all information WimpSim Provides for Earth Events
  class WimpSimEarthEvent:public WimpSimBaseEvent{
  public:
    virtual ~WimpSimEarthEvent();

    /// angle of the primary neutrino (in deg)
    double nu_angle_;
    /// angle of the secondary lepton inice (in deg)
    double lep_angle_;
    /// angle of the secondary hadron inice (in deg)
    double had_angle_;
    /// weight of the secondary hadron inice
    double nu_nadir_;
    /// azimuth of primary neutrino (in deg)
    double nu_az_;
    ///  nadir of secondary lepton inice (in deg)
    double lep_nadir_;
    /// azimuth of secondary lepton inice (in deg)
    double lep_az_;
    /// nadir of secondary hadron inice (in deg)
    double had_nadir_;
    /// azimuth of secondary hadron inice (in deg)
    double had_az_;
    /// elevation of primary neutrino (in deg)
    double nu_el_;
    /// elevation of secondary lepton inice (in deg)
    double lep_el_;
    /// elevation of secondary hadron inice (in deg)
    double had_el_;
    
    //methods
    /// Get Azimuth of the Primary Neutrino
    double GetIceCubeNu_Azimuth() const;
    /// Get Zenith of the Primary Neutrino
    double GetIceCubeNu_Zenith() const;
    /// Get Azimuth of the Secondary Lepton
    double GetIceCubeLep_Azimuth() const;
    /// Get Zenith of the Secondary Lepton
    double GetIceCubeLep_Zenith() const;
    /// Get Azimuth of the Secondary Hadron
    double GetIceCubeHad_Azimuth() const;
    /// Get Zenith of the Secondary Hadron
    double GetIceCubeHad_Zenith() const;
    /// Get Azimuth of the Origin of this event
    double GetIceCubeOrigin_Azimuth() const;
    /// Get Zenith of the Origin of this event
    double GetIceCubeOrigin_Zenith() const;
    /// Get the MJD time of this event
    double GetMJD() const;
  };
  
  /// encapsulate all information WimpSim provides in the FileHeader
  class WimpSimBaseHeader{
  public:
    /// WIMP mass
    double mass_;
    /// Annihilation channel
    uint channel_;
    /// Source of the WIMPs (default is Sun, Earth)
    std::string source_;
    /// Number of annihilations simulated this file
    uint number_content_;
    /// Number of annihilations simulated total
    uint number_total_;
    /// index of this file
    uint files_index_;
    /// Number of files this run
    uint files_total_;
    /// initial seed
    int seed_;
    /// weak mixing angle 12
    double theta_12_;
    /// weak mixing angle 13
    double theta_13_;
    /// weak mixing angle 23
    double theta_23_;
    /// delta
    double delta_;
    /// neutrino mass squared difference 21
    double delta_m2_21_;
    /// neutrino mass squared difference 31
    double delta_m2_31_;
    /// latitude of the detector position in [deg]
    double latitude_;
    /// longitude of the detector position in [deg]
    double longitude_;
    /// start MJD of the simulation
    double mjd1_;
    /// end MJD of the simulation
    double mjd2_;
  };

};

#endif //WIMPSIMTOOLS_H_INCLUDED
