#include <I3Test.h> 
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "TestUtils.h"
#include <iostream>
#include <icetray/I3Logging.h>


TEST_GROUP(NuGInteractionInfo);

TEST(weighted_interaction_test){

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

TEST(calc_total_columndepth_test){

  //GetIcetrayLogger()->SetLogLevel(I3LOG_DEBUG);

  using namespace nugen;
  boost::shared_ptr<I3RandomService>  random = nugen::test::make_random();
  boost::shared_ptr<Steering> steer = nugen::test::make_steering("IceSheet", FINALONLY);
  boost::shared_ptr<I3NuGInteractionInfo> interaction = nugen::test::make_interaction(random, steer);

  boost::shared_ptr<Particle> p = boost::shared_ptr<Particle>(new Particle(I3Particle::Null, I3Particle::NuMu,steer));
  p->SetPos(I3Position(0., 0., -6000000.)); // almost center of the earth
  p->SetDir(I3Direction(0., 0., 1)); // almost center of the earth
  p->SetLength(1.0); // almost center of the earth

  double logE = 5;

  while (logE > 2) {
     p->SetEnergy(pow(10, logE));
     InteractionGeo &intgeo = p->GetInteractionGeoRef();
     I3Position enterI3 = intgeo.GetEnterDetVolDC();
     I3Position exitI3 = intgeo.GetExitDetVolDC();
     double range_m = intgeo.GetLeptonRangeMeter();
     double length_after = intgeo.GetModDetLenAfter();

     std::cerr << "Enter pos " << enterI3 << " Exit pos " << exitI3 
               << " range_m " << range_m << " length_after " << length_after << std::endl;

     // CAUTION
     // for histrical reasons, column depth are internally stored in g/m^3.
     earthmodel::EarthModelServicePtr earth = steer->GetEarthModelService();
     I3Position enterCE = earth->GetEarthCoordPosFromDetCoordPos(enterI3);
     I3Position exitCE = earth->GetEarthCoordPosFromDetCoordPos(exitI3);
     double totcolumndep = interaction->CalculateTotalColumnDepth(enterCE, exitCE);
     std::cerr << "Total column depth " << totcolumndep << std::endl;

     std::map<double,double> cdep_vs_len_map = interaction->GetColumndepthVsLengthMap();
     std::cerr << "Number of layers " << cdep_vs_len_map.size() << std::endl;
     std::map<double,double>::iterator iter;
     double lastcdep = 0;
     double lastlen= 0;
     double cdep, len, dcdep, dlen, midcdep, len_in_vol, density;
     int count = 0;
     double lastdensity  = -1;
     for (iter=cdep_vs_len_map.begin(); iter!=cdep_vs_len_map.end(); ++iter) {
        cdep = iter->first;
        len = iter->second;
        dcdep = cdep - lastcdep;
        dlen = len - lastlen;
        midcdep = 0.5 * (lastcdep + cdep);
        len_in_vol = interaction->ConvertColumnDepthToMeter(midcdep);  
        density = dcdep/dlen;
        char buf[2048];
        sprintf(buf, "i=%d, elog %g, cdep %g, len %g, density %g, midcdep %g, len_in_vol %g\n", 
               count, logE, cdep, len, (dcdep/dlen), midcdep, len_in_vol);
        std::cerr << buf << std::endl;
        ENSURE(density != lastdensity);
        lastcdep = cdep;
        lastlen = len;
        lastdensity = density;
        ++count;
     }

     std::map<double,double> len_vs_cdep_map = interaction->GetLengthVsColumndepthMap();
     lastcdep = 0;
     lastlen= 0;
     count = 0;
     double midlen, cdep_in_vol;
     for (iter=len_vs_cdep_map.begin(); iter!=len_vs_cdep_map.end(); ++iter) {
        len = iter->first;
        cdep = iter->second;
        dcdep = cdep - lastcdep;
        dlen = len - lastlen;
        midlen = 0.5 * (lastlen + len);
        cdep_in_vol = interaction->ConvertMeterToColumnDepth(midlen);  
        char buf[2048];
        sprintf(buf, "i=%d, elog %g, len %g, cdep %g, density %g, midlen %g, cdep_in_vol %g\n", 
               count, logE, len, cdep, (dcdep/dlen), midlen, cdep_in_vol);
        std::cerr << buf << std::endl;
        lastcdep = cdep;
        lastlen = len;
        ++count;
     }

     logE -= 0.5;

  }
}
