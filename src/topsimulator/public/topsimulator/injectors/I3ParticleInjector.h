/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3ParticleInjector.h
 * @version $Rev: $
 * @date $Date: $
 * @authors Tilo Waldenmaier
 */


#ifndef _TOPSIMULATOR_I3PARTICLEINJECTOR_H_
#define _TOPSIMULATOR_I3PARTICLEINJECTOR_H_


#include <topsimulator/interface/I3InjectorService.h>
#include <topsimulator/ExtendedI3Particle.h>
#include <phys-services/I3RandomService.h>
#include <dataclasses/TankKey.h>
#include <dataclasses/physics/I3Particle.h>

#include <map>
#include <string>

/**
 * \brief Generates particles according to various distributions
 * at the positions of specified tanks and passes them to I3TopSimulator.
 *
 * It may be used to tabulate the tank response for analytical
 * purposes and other cases, where user-defined particles need
 * to be injected into tanks.
 */

class I3ParticleInjector: public I3InjectorService
{
public:
  I3ParticleInjector(const I3Context& context);
  
  ~I3ParticleInjector();
  
  void Configure();
  
  bool NextEvent(int& runID, int& evtID, I3Particle& primary, I3FrameConstPtr frame);
  
  bool NextParticle(ExtendedI3Particle& primary);
  
  std::map<std::string, int> GetAirShowerComponentNameMap() const;

private:
  
  void FillTankList(const std::vector<std::string>& tankList);
  
  I3Particle::ParticleType GetParticleType(const std::string& typeName);
  
  double GetEnergy();
  
  double GetZenith();
  
  double GetAzimuth();
  
  void GetXY(double& x, double& y);
  
  void GetRangeParameter(const std::string& name, std::vector<double>& range,  double unit=1.0);
  
  
  void ShiftAlongTrack(I3Particle& particle, double z);
  
  
  int numParticles_;
  
  int numEvents_;
  
  std::vector<TankKey> tankKeys_;
  std::vector<TankKey>::const_iterator tankKey_iter_;
  
  bool useAllTanks_;
  
  I3Particle::ParticleType particleType_;

  std::string randomServiceName_;
  I3RandomServicePtr randomService_;

  
  int eventCounter_;
  int particleCounter_;
  
  
  std::vector<double> energyRange_;
  double specIndex_;

  std::vector<double> radiusRange_;
  std::vector<double> phiRange_;
  
  double startHeight_;
  
  std::vector<double> zenithRange_;
  double zenithPower_;

  std::vector<double> azimuthRange_;
  
  
  SET_LOGGER("I3ParticleInjector");
};


#endif
