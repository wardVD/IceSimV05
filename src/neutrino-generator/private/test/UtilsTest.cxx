#include <I3Test.h>
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/utils/Utils.h"
#include "neutrino-generator/sources/SourceUtils.h"
#include "phys-services/I3GSLRandomService.h"
#include "TestUtils.h"


TEST_GROUP(NuGUtils);

TEST(UtilsTest){
  using namespace nugen;
  ENSURE(Utils::GetParticleType("Numu") == I3Particle::NuMu);
  ENSURE(Utils::GetParticleType("NumubaR") == I3Particle::NuMuBar);
  ENSURE(Utils::GetParticleType("nuE") == I3Particle::NuE);
  ENSURE(Utils::GetParticleType("nuEbar") == I3Particle::NuEBar);
  ENSURE(Utils::GetParticleType("nUTau") == I3Particle::NuTau);
  ENSURE(Utils::GetParticleType("NutauBAR") == I3Particle::NuTauBar);
  
}  
 
TEST(SourceUtilsTest){

  using namespace nugen;

  std::map<I3Particle::ParticleType, double> pmap;
  pmap[Utils::GetParticleType("NuEBar")] = 0.5;
  pmap[Utils::GetParticleType("NuE")] = 0.5;
  std::map<I3Particle::ParticleType, double>::iterator iter;

  int nevts = 1000;
  int nNuE = 0;
  int nNuEbar = 0;
  SteeringPtr steer = nugen::test::make_steering();
  boost::shared_ptr<I3RandomService>  random = nugen::test::make_random();

  for (int i=0; i<nevts; ++i) {
     ParticlePtr p = SourceUtils::GeneratePrimaryNeutrino(pmap, random, steer);
     I3Particle::ParticleType ptype = p->GetType();
     if (ptype == I3Particle::NuE) {
         ++nNuE;
     } else if (ptype == I3Particle::NuEBar) {
         ++nNuEbar;
     }
  }

  ENSURE_EQUAL(nNuE + nNuEbar, nevts, "sum of NuE and NuEbar is not total events");
  ENSURE_DISTANCE(nNuE, nNuEbar, 100, "ratio of NuE and NuEbar is wrong?");

  // make one of them inactive 
  pmap[Utils::GetParticleType("NuE")] = 0;

  nNuE = 0;
  nNuEbar = 0;
  for (int i=0; i<nevts; ++i) {
     ParticlePtr p = SourceUtils::GeneratePrimaryNeutrino(pmap, random, steer);
     I3Particle::ParticleType ptype = p->GetType();
     if (ptype == I3Particle::NuE) {
         ++nNuE;
     } else if (ptype == I3Particle::NuEBar) {
         ++nNuEbar;
     }
  }

  ENSURE_EQUAL(nNuE, 0, "NuE is not inactivated!");
  ENSURE_EQUAL(nNuEbar, nevts, "total events doesn't match");
}
