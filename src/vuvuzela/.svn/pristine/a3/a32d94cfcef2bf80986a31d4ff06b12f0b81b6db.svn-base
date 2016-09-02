#include <I3Test.h>

#include "icetray/I3Units.h"
#include "icetray/OMKey.h"
#include "vuvuzela/VuvuzelaFunctions.h"

TEST_GROUP(HitAddition);

TEST(HitSeriesMapAddition){
  I3MCPESeriesMap temp;
  {
    I3MCPESeries hitSeries;
    I3MCPE hit;
    hitSeries.push_back(hit);
    OMKey omkey(21,30);
    temp[omkey] = hitSeries;
  }
  I3MCPESeriesMapConstPtr firstterm(new I3MCPESeriesMap(temp));

  I3MCPESeriesMap temp2;
  {
    I3MCPESeries hitSeries;
    I3MCPE hit;
    hitSeries.push_back(hit);
    OMKey omkey(21,30);
    temp2[omkey] = hitSeries;

    OMKey omkey2(29,30);
    temp2[omkey2] = hitSeries;
  }
  I3MCPESeriesMapConstPtr secondterm(new I3MCPESeriesMap(temp2));

  I3MCPESeriesMapConstPtr lhs = AddHitMaps(firstterm, secondterm);

  int n_hits(0);
  I3MCPESeriesMap::const_iterator iter;
  for(iter = lhs->begin(); iter != lhs->end(); ++iter)
    n_hits += static_cast<int>(iter->second.size());

  ENSURE(n_hits == 3,"Wrong number of hits in the map.");
}
