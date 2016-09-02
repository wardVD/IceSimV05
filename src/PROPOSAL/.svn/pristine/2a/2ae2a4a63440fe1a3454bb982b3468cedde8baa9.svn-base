#include <I3Test.h>

#include <dataclasses/physics/I3Particle.h>
#include <phys-services/I3GSLRandomService.h>
#include <PROPOSAL-icetray/I3PropagatorServicePROPOSAL.h>
#include <PROPOSAL-icetray/SimplePropagator.h>

#include <PROPOSAL/EpairStochastic.h>

#include <boost/make_shared.hpp>

TEST_GROUP(Repeatablility);

namespace{
I3Particle make_particle(I3RandomServicePtr rng, I3Particle::ParticleType type)
{
  I3Particle p;

  double x = rng->Uniform(-1*I3Units::km, 1*I3Units::km);
  double y = rng->Uniform(-1*I3Units::km, 1*I3Units::km);
  double z = rng->Uniform(-1*I3Units::km, 1*I3Units::km);

  double zenith = rng->Uniform(0,I3Constants::pi);
  double azimuth = rng->Uniform(0,2*I3Constants::pi);

  double energy = rng->Uniform(1*I3Units::GeV, 10*I3Units::TeV);

  p.SetPos(x,y,z);
  p.SetDir(zenith,azimuth);
  p.SetTime(0);
  p.SetType(type);
  p.SetLocationType(I3Particle::InIce);
  p.SetEnergy(energy);
  
  return p;
}
}

// Setting the mass allows for monopole and stau tests, but table generation for
// those types is a bit too stressful right now.
bool stress_test(I3Particle::ParticleType type, double mass = NAN){
  I3RandomServicePtr rng(new I3GSLRandomService(I3Constants::e));
  
  double h(1600*I3Units::m);
  double r(800*I3Units::m);
  I3PropagatorServicePROPOSALPtr prop(new I3PropagatorServicePROPOSAL("","",r,h,type, mass));
  prop->SetRandomNumberGenerator(rng);
  
  I3PropagatorService::DiagnosticMapPtr frame(new I3PropagatorService::DiagnosticMap);
  
  for (int i=0; i<10000; i++) {
    I3Particle p = make_particle(rng, type);
    std::vector<I3Particle> d = prop->Propagate(p, frame);
    if(!std::isfinite(p.GetLength()))
      return false;
  }
    return true;
}

TEST(MuMinusStressTest){
  ENSURE(stress_test(I3Particle::MuMinus));
}
TEST(MuPlusStressTest){
  ENSURE(stress_test(I3Particle::MuPlus));
}
TEST(TauMinusStressTest){
  ENSURE(stress_test(I3Particle::TauMinus));
}
TEST(TauPlusStressTest){
  ENSURE(stress_test(I3Particle::TauPlus));
}
