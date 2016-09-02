#include <I3Test.h> 
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "TestUtils.h"
#include <iostream>
#include <icetray/I3Logging.h>


TEST_GROUP(NuGInteraction);

TEST(MakeInteractionTest){

  //GetIcetrayLogger()->SetLogLevel(I3LOG_DEBUG);

  using namespace nugen;
  boost::shared_ptr<I3RandomService>  random = nugen::test::make_random();
  boost::shared_ptr<Steering> steer = nugen::test::make_steering();
  boost::shared_ptr<I3NuGInteractionInfo> interaction = nugen::test::make_interaction(random, steer);

  int nevts = 100;
  double ccratio = 0;
  double ncratio = 1.0;
  double grratio = 1.0;
  int numcc = 0; 
  int numnc = 0; 
  int numgr = 0; 

  // Does NCGRWeighted option works correctly?
  for (int i=0; i<nevts; ++i) {

     boost::shared_ptr<Particle> p = boost::shared_ptr<Particle>(new Particle(I3Particle::Null, I3Particle::NuEBar,steer));
     p->SetPos(I3Position(0., 0., -6000000.)); // almost center of the earth
     p->SetDir(I3Direction(0., 0., 1)); // almost center of the earth
     p->SetLength(1.0); // almost center of the earth
     p->SetEnergy(3e+6);
     //I3Position pos = p->GetPos();
     //std::cerr << "particle pos : " << pos.GetX() << "," << pos.GetY() << "," << pos.GetZ() << std::endl;

     // here make an interaction!
     InteractionType itype = interaction->MakeInteraction(p, ccratio, ncratio, grratio);

     if (itype == CC) {
        ++numcc;
     } else if (itype == NC) {
        ++numnc;
     } else if (itype == GR) {
        ++numgr;
     }
  }                          
  std::cerr << "cc : " << numcc << " nc : " << numnc << " gr : " << numgr << std::endl;
  ENSURE_EQUAL(numcc, 0, "With AUTODETECT NuE's CC interaction during InEarth propgation must be zero");
  ENSURE(numgr != 0); // you must have some gr interaction for 2PeV

}
