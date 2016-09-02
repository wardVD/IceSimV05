#include <I3Test.h>

#include "phys-services/I3GSLRandomService.h"
#include "icetray/I3Units.h"

#include "vuvuzela/VuvuzelaFunctions.h"

using namespace std;

TEST_GROUP(ThermalNoise);

TEST(ThermalNoise){
  I3GSLRandomServicePtr randomService(new I3GSLRandomService(1000));
  ENSURE((bool)randomService);
  
  std::vector<double> buffer(4, 100);
  double bufferTime = 3 * I3Units::second;
  double rate = 100 * I3Units::hertz;
  double start = 0 * I3Units::second;
  double stop = 1 * I3Units::second;

  double expected = (stop-start)*rate + buffer.size();
  std::set<double> bufferset(buffer.begin(), buffer.end());
  MakeThermalHits(randomService,
		  bufferset,
		  bufferTime,
		  rate,
		  start,
		  stop);

  ENSURE_DISTANCE(bufferset.size(), expected, 10, "Unexpected number of hits for 100 hz in 1 second");
}
