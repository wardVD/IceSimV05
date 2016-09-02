#include <I3Test.h>

#include "phys-services/I3GSLRandomService.h"
#include "icetray/I3Units.h"

#include "vuvuzela/VuvuzelaFunctions.h"

using namespace std;

TEST_GROUP(NonThermalNoise);

TEST(NonThermalNoise){
  I3GSLRandomServicePtr randomService(new I3GSLRandomService(1000));
  ENSURE((bool)randomService);
  
  std::vector<double> buffer(4, 100);
  double bufferTime = 3 * I3Units::second;
  double nhits = 10;
  double mean = 3;
  double sigma = 3;
  double rate = 100 * I3Units::hertz;
  double start = 0 * I3Units::second;
  double stop = 1 * I3Units::second;

  double expected = 1022;
  std::set<double> bufferset(buffer.begin(), buffer.end());
  MakeNonThermalHits(randomService,
		     bufferset,
		     bufferTime,
		     rate,
		     nhits,
		     mean,
		     sigma,
		     start,
		     stop,
		     true);

  ENSURE_DISTANCE(bufferset.size(), expected, 10, "Unexpected number of hits for nonthermal processes in 1 s.");
}
