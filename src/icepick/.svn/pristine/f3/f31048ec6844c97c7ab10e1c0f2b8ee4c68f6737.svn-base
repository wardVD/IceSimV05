#include <I3Test.h>

#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Bool.h>
#include <icetray/OMKey.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <interfaces/I3IcePickModule.h>

#include <string>
#include <vector>

using namespace std;

namespace I3DOMLaunchTriggerTypeFilterTestUtil
{
  class Source : public I3Module
  {
   public:
    Source(const I3Context& context)
      : I3Module(context),
        dummy_(false)
    {
      AddParameter("Dummy", "blah", dummy_);
      
      AddOutBox("OutBox");
    }

    void Configure()
    {
      GetParameter("Dummy", dummy_);
    }
  
    void Process()
    {
      I3FramePtr frame(new I3Frame(I3Frame::Physics));
      I3DOMLaunchSeriesMapPtr launches(new I3DOMLaunchSeriesMap());
      if(!dummy_)
      {
        OMKey key(1, 1);
        (*launches)[key].push_back(I3DOMLaunch());
      }
      frame->Put("I3DOMLaunchTriggerTypeFilterTestMap", launches);
      PushFrame(frame,"OutBox");
    }
  
   private:
    bool dummy_;
  };

  class Test : public I3Module
  {
   public:
    Test(const I3Context& context)
      : I3Module(context),
        inputDecisionName_("I3DOMLaunchTriggerTypeFilterTestDecision"),
        expectation_(true)
    {
      AddParameter("InputDecisionName", "blah", inputDecisionName_);
      AddParameter("Expectation", "blah", expectation_);

      AddOutBox("OutBox");
    }

    void Configure()
    {
      GetParameter("InputDecisionName", inputDecisionName_);
      ENSURE(!inputDecisionName_.empty());
      GetParameter("Expectation", expectation_);
    }
    
    void Physics(I3FramePtr frame)
    {
      I3BoolConstPtr inputDecision = frame->Get<I3BoolConstPtr>(inputDecisionName_);
      ENSURE((bool)inputDecision);
      if(expectation_) ENSURE(inputDecision->value);
      else ENSURE(!inputDecision->value);
      PushFrame(frame,"OutBox");
    }
    
   private:
    string inputDecisionName_;
    bool expectation_;
  };
}

I3_MODULE(I3DOMLaunchTriggerTypeFilterTestUtil::Source);
I3_MODULE(I3DOMLaunchTriggerTypeFilterTestUtil::Test);

TEST_GROUP(I3DOMLaunchTriggerTypeFilterTest);

TEST(NoData)
{
  I3Tray tray;
  
  tray.AddModule("I3DOMLaunchTriggerTypeFilterTestUtil::Source", "input");
  
  vector<string> names;
  names.push_back("I3DOMLaunchTriggerTypeFilterTestMapWrong");
  tray.AddModule("I3IcePickModule<I3DOMLaunchTriggerTypeFilter>", "filter")
    ("DecisionName", "I3DOMLaunchTriggerTypeFilterTestDecision")
    ("DiscardEvents", false)
    ("Names", names);
  
  tray.AddModule("I3DOMLaunchTriggerTypeFilterTestUtil::Test", "test")
    ("Expectation", true);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(NoNamesDefined)
{
  I3Tray tray;
  
  tray.AddModule("I3DOMLaunchTriggerTypeFilterTestUtil::Source", "input");
  
  vector<string> names;
  tray.AddModule("I3IcePickModule<I3DOMLaunchTriggerTypeFilter>", "filter")
    ("DecisionName", "I3DOMLaunchTriggerTypeFilterTestDecision")
    ("DiscardEvents", false)
    ("Names", names);
  
  tray.AddModule("I3DOMLaunchTriggerTypeFilterTestUtil::Test", "test")
    ("Expectation", true);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(DataButEmpty)
{
  I3Tray tray;
  
  tray.AddModule("I3DOMLaunchTriggerTypeFilterTestUtil::Source", "input")
    ("Dummy", true);
  
  vector<string> names;
  names.push_back("I3DOMLaunchTriggerTypeFilterTestMap");
  tray.AddModule("I3IcePickModule<I3DOMLaunchTriggerTypeFilter>", "filter")
    ("DecisionName", "I3DOMLaunchTriggerTypeFilterTestDecision")
    ("DiscardEvents", false)
    ("Names", names);
  
  tray.AddModule("I3DOMLaunchTriggerTypeFilterTestUtil::Test", "test")
    ("Expectation", true);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(Data)
{
  I3Tray tray;
  
  tray.AddModule("I3DOMLaunchTriggerTypeFilterTestUtil::Source", "input");
  
  vector<string> names;
  names.push_back("I3DOMLaunchTriggerTypeFilterTestMap");
  tray.AddModule("I3IcePickModule<I3DOMLaunchTriggerTypeFilter>", "filter")
    ("DecisionName", "I3DOMLaunchTriggerTypeFilterTestDecision")
    ("DiscardEvents", false)
    ("Names", names);
  
  tray.AddModule("I3DOMLaunchTriggerTypeFilterTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}
