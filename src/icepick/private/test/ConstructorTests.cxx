#include <I3Test.h>
#include <icetray/I3Tray.h>

#include <string>
#include <vector>

using namespace std;

TEST_GROUP(ConstructorTests);

TEST(I3NHitChannelFilterTest)
{
  I3Tray tray;

  tray.AddModule("I3IcePickModule<I3NHitChannelFilter>","filter");
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(0);
}

TEST(I3EventIdFilterTest)
{
  I3Tray tray;

  tray.AddModule("I3IcePickModule<I3EventIdFilter>","filter");
  tray.AddModule("TrashCan","trash");
  
  tray.Execute(0);
}

TEST(I3TriggerSatisfiedFilterTest)
{
  I3Tray tray;

  tray.AddModule("I3IcePickModule<I3TriggerSatisfiedFilter>","filter");

  tray.AddModule("TrashCan","trash");
  
  tray.Execute(0);
}

TEST(I3RecoPulseFilterTest)
{
  I3Tray tray;

  tray.AddModule("I3IcePickModule<I3RecoPulseFilter>","filter");

  tray.AddModule("TrashCan","trash");
  
  tray.Execute(0);
}

TEST(I3UnFilterTest)
{
  I3Tray tray;

  tray.AddModule("I3IcePickModule<I3UnFilter>","filter");

  tray.AddModule("TrashCan","trash");

  tray.Execute(0);
}

TEST(I3OrFilterTest)
{
  I3Tray tray;

  vector<string> inputDecisions;
  inputDecisions.push_back("blahblah");
  tray.AddModule("I3IcePickModule<I3OrFilter>","filter")
       ("InputDecisions",inputDecisions);

  tray.AddModule("TrashCan","trash");

  tray.Execute(0);
}

TEST(I3OrServicesTest)
{
  I3Tray tray;

  vector<string> inputDecisions;
  inputDecisions.push_back("blahblah");
  tray.AddService("I3IcePickInstaller<I3OrFilter>","filterservice")
       ("InputDecisions",inputDecisions);

  vector<string> servicesToOr;
  servicesToOr.push_back("filterservice");
  tray.AddModule("I3IcePickModule<I3OrServices>","filtermodule")
       ("ServicesToOr",servicesToOr);

  tray.AddModule("TrashCan","trash");

  tray.Execute(0);
}

TEST(I3AndFilterTest)
{
  I3Tray tray;

  vector<string> inputDecisions;
  inputDecisions.push_back("blahblah");
  tray.AddModule("I3IcePickModule<I3AndFilter>","filter")
       ("InputDecisions",inputDecisions);

  tray.AddModule("TrashCan","trash");

  tray.Execute(0);
}

TEST(I3AndServicesTest)
{
  I3Tray tray;

  vector<string> inputDecisions;
  inputDecisions.push_back("blahblah");
  tray.AddService("I3IcePickInstaller<I3AndFilter>","filterservice")
       ("InputDecisions",inputDecisions);

  vector<string> servicesToAnd;
  servicesToAnd.push_back("filterservice");
  tray.AddModule("I3IcePickModule<I3AndServices>","filtermodule")
       ("ServicesToAnd",servicesToAnd);

  tray.AddModule("TrashCan","trash");

  tray.Execute(0);
}

TEST(I3RandomFilterTest)
{
  I3Tray tray;

  tray.AddModule("I3IcePickModule<I3RandomFilter>","filter");

  tray.AddModule("TrashCan","trash");

  tray.Execute(0);
}

TEST(I3DOMLaunchTriggerTypeFilterTest)
{
  I3Tray tray;

  tray.AddModule("I3IcePickModule<I3DOMLaunchTriggerTypeFilter>","filter");

  tray.AddModule("TrashCan","trash");

  tray.Execute(0);
}

TEST(I3NChFilterTest)
{
  I3Tray tray;

  tray.AddModule("I3IcePickModule<I3NChFilter>","filter");

  tray.AddModule("TrashCan","trash");

  tray.Execute(0);
}
