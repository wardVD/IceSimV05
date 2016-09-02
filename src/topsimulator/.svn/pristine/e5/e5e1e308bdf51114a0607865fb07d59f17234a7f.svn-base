/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3CorsikaThinnedInjector.h
 * @date $Date: $
 * @authors Hans Dembinski
 */


#ifndef _TOPSIMULATOR_I3_CORSIKA_THINNED_INJECTOR_H_
#define _TOPSIMULATOR_I3_CORSIKA_THINNED_INJECTOR_H_


#include <topsimulator/interface/I3InjectorService.h>
#include <topsimulator/injectors/I3CorsikaReader.h>
#include <topsimulator/injectors/ParticleUnthinner.h>
#include <phys-services/I3RandomService.h>

#include <vector>
#include <string>

/**
 * \brief InjectorService that scans through a CORSIKA file and passes the particles
 * to I3TopSimulator.
 *
 * It randomly choses the the core position within a circular
 * sampling area and resamples the same shower several times. It also performs the
 * un-thinning of thinned CORSIKA files and can optionally insert high pT muons.
 */

class I3CorsikaThinnedInjector: public I3InjectorService
{
public:
  I3CorsikaThinnedInjector(const I3Context& context);
  ~I3CorsikaThinnedInjector();
  
  void Configure();
  
  bool NextEvent(int& runID, int& evtID, I3Particle& primary, I3FrameConstPtr frame);
  
  bool NextParticle(ExtendedI3Particle& primary);

  std::map<std::string, int> GetAirShowerComponentNameMap() const;
  
  I3FrameObjectConstPtr GetEventInfo();

private:

  AirShowerComponent GetAirShowerComponent(const I3Particle& p, double hadgen) const;

  std::vector<std::string> corsikaFiles_;
  std::vector<int> ignoreTypes_;
  I3CorsikaReader reader_;
  double raiseObsLevel_;  // For hacking when the ObsLevel is set too low.

  ParticleUnthinner unthinner_;

  double relocationX_;
  double relocationY_;
  double relocationR_;
  double shiftX_;
  double shiftY_;

  int currentRunID_;
  int currentEvtID_;
  unsigned numSamples_;
  unsigned sampleIndex_;

  std::vector<ExtendedI3Particle> clones_;

  std::string randomServiceName_;
  I3RandomServicePtr randomService_;
  bool smartUnthinning_;
    
  SET_LOGGER("I3CorsikaThinnedInjector");
};


#endif
