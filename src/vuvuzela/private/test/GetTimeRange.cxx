#include <I3Test.h>

#include "icetray/I3Units.h"
#include "vuvuzela/VuvuzelaFunctions.h"

#include <boost/random.hpp>

TEST_GROUP(TimeRangeTest);

TEST(GetTimeRange){

  const double FIRST(0.045*I3Units::ns);
  const double LAST(12345.*I3Units::ns);

  I3MCPESeriesMap hitmap;
  {
    /**
     * make and plant the first hit
     */
    I3MCPE first_hit;
    first_hit.time = FIRST;

    I3MCPESeries first_hs;
    first_hs.push_back(first_hit);

    hitmap[OMKey(21,5)] = first_hs;

    /**
     * make and plant the last hit
     */
    I3MCPE last_hit;
    last_hit.time = LAST;

    I3MCPESeries last_hs;
    last_hs.push_back(last_hit);

    hitmap[OMKey(21,55)] = last_hs;
  }

  /**
   *  Fill the hit map with random hits
   */
  {
    boost::rand48 rng(static_cast<int>(time(0)));
    boost::uniform_smallint<int> string_rng(1,80);
    boost::uniform_smallint<unsigned int> om_rng(1,60);
    boost::uniform_real<double> time_rng(FIRST,LAST);

    for(int i = 0; i<100000; i++){
      I3MCPE h;
      h.time = time_rng(rng);

      OMKey omKey(string_rng(rng), om_rng(rng));
      if(hitmap.find(omKey) == hitmap.end()){
	//add an empty list if one doesn't exist
	I3MCPESeries hs;
	hitmap[omKey] = hs;
      }

      hitmap[omKey].push_back(h);    

    }
  }

  I3MCPESeriesMapConstPtr m(new I3MCPESeriesMap(hitmap));

  std::pair<double,double> range = GetTimeRange(m);

  std::cerr<<"FIRST = "<<FIRST << std::endl;
  std::cerr<<"LAST = "<<LAST << std::endl;
  std::cerr<<"range.first = "<<range.first << std::endl;
  std::cerr<<"range.second = "<<range.second << std::endl;

  ENSURE_DISTANCE(range.first,FIRST,0.001*I3Units::ns);
  ENSURE_DISTANCE(range.second,LAST,0.001*I3Units::ns);
}
