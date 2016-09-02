#include <I3Test.h>

#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Bool.h>
#include <interfaces/I3IcePickModule.h>
#include <dataclasses/physics/I3FilterResult.h>

#include <string>
#include <vector>

using namespace std;

namespace FilterMaskFilterTestUtil
{
  class Source : public I3Module
  {
   public:
    Source(const I3Context& context)
      : I3Module(context),
        condition_(true),
        prescale_(true)
    {
      AddParameter("Condition", "blah", condition_);
      AddParameter("Prescale", "blah", prescale_);
      
      AddOutBox("OutBox");
    }

    void Configure()
    {
      GetParameter("Condition", condition_);
      GetParameter("Prescale", prescale_);
    }
  
    void Process()
    {
      I3FramePtr frame(new I3Frame(I3Frame::Physics));
      I3FilterResultMapPtr result (new I3FilterResultMap());
      (*result)["FilterMaskFilterTestName"].conditionPassed = condition_;
      (*result)["FilterMaskFilterTestName"].prescalePassed = prescale_;
      frame->Put("FilterMaskFilterTestResult", result);
      PushFrame(frame,"OutBox");
    }
  
   private:
    bool condition_;
    bool prescale_;
  };

  class Test : public I3Module
  {
   public:
    Test(const I3Context& context)
      : I3Module(context),
        inputDecisionName_("FilterMaskFilterTestDecision"),
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

I3_MODULE(FilterMaskFilterTestUtil::Source);
I3_MODULE(FilterMaskFilterTestUtil::Test);

TEST_GROUP(FilterMaskFilterTest);

TEST(NoData)
{
  I3Tray tray;
  
  tray.AddModule("FilterMaskFilterTestUtil::Source", "input");
  
  vector<string> names;
  names.push_back("FilterMaskFilterTestName");
  tray.AddModule("I3IcePickModule<FilterMaskFilter>", "filter")
    ("DecisionName", "FilterMaskFilterTestDecision")
    ("DiscardEvents", false)
    ("FilterResultName", "FilterMaskFilterTestResultWrong")
    ("FilterNameList", names);
  
  tray.AddModule("FilterMaskFilterTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(NoNamesDefined)
{
  I3Tray tray;
  
  tray.AddModule("FilterMaskFilterTestUtil::Source", "input");
  
  vector<string> names;
  tray.AddModule("I3IcePickModule<FilterMaskFilter>", "filter")
    ("DecisionName", "FilterMaskFilterTestDecision")
    ("DiscardEvents", false)
    ("FilterResultName", "FilterMaskFilterTestResult")
    ("FilterNameList", names);
  
  tray.AddModule("FilterMaskFilterTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(InvalidName)
{
  I3Tray tray;
  
  tray.AddModule("FilterMaskFilterTestUtil::Source", "input");
  
  vector<string> names;
  names.push_back("FilterMaskFilterTestNameWrong");
  tray.AddModule("I3IcePickModule<FilterMaskFilter>", "filter")
    ("DecisionName", "FilterMaskFilterTestDecision")
    ("DiscardEvents", false)
    ("FilterResultName", "FilterMaskFilterTestResult")
    ("FilterNameList", names);
  
  tray.AddModule("FilterMaskFilterTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(DataButNoCondition)
{
  I3Tray tray;
  
  tray.AddModule("FilterMaskFilterTestUtil::Source", "input")
    ("Condition",false);
  
  vector<string> names;
  names.push_back("FilterMaskFilterTestName");
  tray.AddModule("I3IcePickModule<FilterMaskFilter>", "filter")
    ("DecisionName", "FilterMaskFilterTestDecision")
    ("DiscardEvents", false)
    ("FilterResultName", "FilterMaskFilterTestResult")
    ("FilterNameList", names);
  
  tray.AddModule("FilterMaskFilterTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(DataButNoPrescale)
{
  I3Tray tray;
  
  tray.AddModule("FilterMaskFilterTestUtil::Source", "input")
    ("Prescale",false);
  
  vector<string> names;
  names.push_back("FilterMaskFilterTestName");
  tray.AddModule("I3IcePickModule<FilterMaskFilter>", "filter")
    ("DecisionName", "FilterMaskFilterTestDecision")
    ("DiscardEvents", false)
    ("FilterResultName", "FilterMaskFilterTestResult")
    ("FilterNameList", names);
  
  tray.AddModule("FilterMaskFilterTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(DataButNoConditionAndNoPrescale)
{
  I3Tray tray;
  
  tray.AddModule("FilterMaskFilterTestUtil::Source", "input")
    ("Condition",false)
    ("Prescale",false);
  
  vector<string> names;
  names.push_back("FilterMaskFilterTestName");
  tray.AddModule("I3IcePickModule<FilterMaskFilter>", "filter")
    ("DecisionName", "FilterMaskFilterTestDecision")
    ("DiscardEvents", false)
    ("FilterResultName", "FilterMaskFilterTestResult")
    ("FilterNameList", names);
  
  tray.AddModule("FilterMaskFilterTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(Data)
{
  I3Tray tray;
  
  tray.AddModule("FilterMaskFilterTestUtil::Source", "input");
  
  vector<string> names;
  names.push_back("FilterMaskFilterTestName");
  tray.AddModule("I3IcePickModule<FilterMaskFilter>", "filter")
    ("DecisionName", "FilterMaskFilterTestDecision")
    ("DiscardEvents", false)
    ("FilterResultName", "FilterMaskFilterTestResult")
    ("FilterNameList", names);
  
  tray.AddModule("FilterMaskFilterTestUtil::Test", "test")
    ("Expectation", true);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}
