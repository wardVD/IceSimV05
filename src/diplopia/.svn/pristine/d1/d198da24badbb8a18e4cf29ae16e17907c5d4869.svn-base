#include <I3Test.h>
#include <stdlib.h>
#include "icetray/I3Units.h"
#include "diplopia/DiplopiaUtils.h"
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "dataclasses/physics/I3Particle.h"

// MODIFY: replace with include of the header file for the module you
// are testing, or whatever headers are necessary for this test.

//#include <multi-primary/.h>

TEST_GROUP(I3Diplopia);

TEST(MergeTrees)
{
  I3MCTreePtr mctree(new I3MCTree);
  unsigned int treesize = 4;

  for (unsigned int i=0; i< treesize; i++)
  {
          I3MCTreePtr newMCTree(new I3MCTree());
          I3Particle primary;
          double x,y,z,theta,phi,t,e;

          x = rand(); y = rand(); z = rand();
          theta = 180.*rand()/RAND_MAX; phi = 360.*rand()/RAND_MAX;
          t = rand();
          e = rand();

          primary.SetType(I3Particle::MuMinus);
          primary.SetPos(x*I3Units::m, y*I3Units::m, z*I3Units::m);
          primary.SetTime(t*I3Units::ns);
          primary.SetDir(theta*I3Units::deg,phi*I3Units::deg);
          primary.SetEnergy(e*I3Units::MeV);
          primary.SetLocationType(I3Particle::InIce);

          I3MCTreeUtils::AddPrimary(*newMCTree,primary);
          DiplopiaUtils::MergePrimaries(mctree, newMCTree, t);
  }
  ENSURE(I3MCTreeUtils::GetPrimaries(*mctree).size() == treesize, "MCTree size is wrong");
}
TEST(MergePEs)
{
    I3MCPESeriesMapPtr mcPESeriesMap1(new I3MCPESeriesMap);
    I3MCPESeriesMapPtr mcPESeriesMap2(new I3MCPESeriesMap);

    for (unsigned int i=0;i<20;i++)
    {
        I3MCPESeries mcPESeries1;
        I3MCPESeries mcPESeries2;
        for (unsigned int j=0;j<20;j++)
        {
            I3MCPE mchit1,mchit2;
            mchit1.time = rand();
            mcPESeries1.push_back(mchit1);

            mchit2.time = rand();
            mcPESeries2.push_back(mchit2);
        }
        (*mcPESeriesMap1)[OMKey(i,i)] = mcPESeries1;
        (*mcPESeriesMap2)[OMKey(i+1,i)] = mcPESeries2;
    }

   double offsetTime = 10000.*rand()*I3Units::ns/RAND_MAX;
   I3MCPESeriesMapPtr newMCPESeriesMap(new I3MCPESeriesMap());
   DiplopiaUtils::MergeHits(*newMCPESeriesMap,*mcPESeriesMap1,offsetTime);
   offsetTime = 10000.*rand()*I3Units::ns/RAND_MAX;
   DiplopiaUtils::MergeHits(*newMCPESeriesMap,*mcPESeriesMap2,offsetTime);

   unsigned int nhits=0;
   
   I3MCPESeriesMap::const_iterator om_iter;
   for(om_iter = newMCPESeriesMap->begin(); om_iter != newMCPESeriesMap->end(); om_iter++)
   {
           nhits += om_iter->second.size();
   }
   ENSURE(nhits == 20*20*2,"Wrong number of hits.");
}
           
