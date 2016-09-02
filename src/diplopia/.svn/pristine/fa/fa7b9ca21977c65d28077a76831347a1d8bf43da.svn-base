#include <I3Test.h>

#include "icetray/I3Tray.h"
#include "icetray/I3Units.h" 
//#include <TestWeights.h>
#include <test/PPTestSource.h>
#include "diplopia/I3PolyplopiaExp.h"
#include "icetray/test/ConstructorTest.h" 
#include "diplopia/I3PolyplopiaExp.h" 

class TestWeights;

TEST_GROUP(IceTrayTests);

using namespace std;

TEST(icetray_inspect){
  clean_constructor_test<I3PolyplopiaExp>();
} 


TEST(IceCubeOnlyTest)
{

  I3Tray tray;
  
  int nFrames = 20;

  tray.AddService("I3GSLRandomServiceFactory", "random")
    ("Seed",1);

  string I3_BUILD(getenv("I3_BUILD"));
  string I3_TESTDATA(getenv("I3_TESTDATA"));
  string gcdfile(I3_TESTDATA+"/sim/GeoCalibDetectorStatus_IC86.55697_corrected.i3.gz");
  string summary(I3_BUILD+"/diplopia/resources/summary.xml");
  

  tray.AddService("I3XMLSummaryServiceFactory","summary")
   ("InputFileName",summary)
   ("OutputFileName",".summary.xml");
  
  tray.AddModule("I3InfiniteSource", "source")
  ("Prefix", gcdfile);
  
  tray.AddModule("I3MCEventHeaderGenerator", "times");
  

  const int NPES(500);

  tray.AddModule("PPTestSource", "test_source")
    ("NPEs",NPES)
    ("MCTreeName","I3MCTree")
    ("MCPESeriesMapName","I3MCPESeriesMap");

  tray.AddModule("TestWeights", "weights");

  tray.AddModule("I3PolyplopiaExp", "merge")
	("MCTreeName","I3MCTree")
	("MMCTrackListName","MMCTrackList")
    ("TimeWindow", 50*I3Units::microsecond)
    ("Rate",10.5*I3Units::kilohertz);
  
  
  tray.AddModule("PPCheckWeights", "test")
	("MCTreeName","I3MCTree")
	("MMCTrackListName","MMCTrackList")
	("MCPESeriesMapName","I3MCPESeriesMap");

  tray.AddModule("TrashCan", "trash");

  tray.Execute(nFrames);
  tray.Finish();


}

