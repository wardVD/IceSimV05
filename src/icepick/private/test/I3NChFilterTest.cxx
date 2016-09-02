#include <I3Test.h>

#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Bool.h>
#include <interfaces/I3IcePickModule.h>
#include <phys-services/I3CutValues.h>

#include <string>

using namespace std;

namespace I3NChFilterTestUtil
{
  class Source : public I3Module
  {
   public:
    Source(const I3Context& context)
      : I3Module(context),
        nCh_(100)
    {
      AddParameter("NCh", "blah", nCh_);
      
      AddOutBox("OutBox");
    }

    void Configure()
    {
      GetParameter("NCh", nCh_);
      ENSURE(nCh_ >= 0);
    }
  
    void Process()
    {
      I3FramePtr frame(new I3Frame(I3Frame::Physics));
      I3CutValuesPtr cuts(new I3CutValues());
      cuts->Nchan = nCh_;
      frame->Put("I3NChFilterTestCuts", cuts);
      PushFrame(frame,"OutBox");
    }
  
   private:
    int nCh_;
  };

  class Test : public I3Module
  {
   public:
    Test(const I3Context& context)
      : I3Module(context),
        inputDecisionName_("I3NChFilterTestDecision"),
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

I3_MODULE(I3NChFilterTestUtil::Source);
I3_MODULE(I3NChFilterTestUtil::Test);

TEST_GROUP(I3NChFilterTest);

TEST(NChLt)
{
  I3Tray tray;
  
  tray.AddModule("I3NChFilterTestUtil::Source", "input");
  
  tray.AddModule("I3IcePickModule<I3NChFilter>", "filter")
    ("DecisionName", "I3NChFilterTestDecision")
    ("DiscardEvents", false)
    ("CutValuesKey", "I3NChFilterTestCuts")
    ("NCh", 101);
  
  tray.AddModule("I3NChFilterTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(NChEq)
{
  I3Tray tray;
  
  tray.AddModule("I3NChFilterTestUtil::Source", "input");
  
  tray.AddModule("I3IcePickModule<I3NChFilter>", "filter")
    ("DecisionName", "I3NChFilterTestDecision")
    ("DiscardEvents", false)
    ("CutValuesKey", "I3NChFilterTestCuts")
    ("NCh", 100);
  
  tray.AddModule("I3NChFilterTestUtil::Test", "test")
    ("Expectation", true);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(NChBt)
{
  I3Tray tray;
  
  tray.AddModule("I3NChFilterTestUtil::Source", "input");
  
  tray.AddModule("I3IcePickModule<I3NChFilter>", "filter")
    ("DecisionName", "I3NChFilterTestDecision")
    ("DiscardEvents", false)
    ("CutValuesKey", "I3NChFilterTestCuts")
    ("NCh", 99);
  
  tray.AddModule("I3NChFilterTestUtil::Test", "test")
    ("Expectation", true);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(NoData)
{
  I3Tray tray;
  
  tray.AddModule("I3NChFilterTestUtil::Source", "input");
  
  tray.AddModule("I3IcePickModule<I3NChFilter>", "filter")
    ("DecisionName", "I3NChFilterTestDecision")
    ("DiscardEvents", false)
    ("CutValuesKey", "I3NChFilterTestCutsWrong")
    ("NCh", 99);
  
  tray.AddModule("I3NChFilterTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("Dump", "dump");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}
