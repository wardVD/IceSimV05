#include <I3Test.h>

#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Bool.h>
#include <dataclasses/physics/I3FilterResult.h>

using namespace std;

namespace FilterMask2BoolsTestUtil
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
      (*result)["FilterMask2BoolsTestName"].conditionPassed = condition_;
      (*result)["FilterMask2BoolsTestName"].prescalePassed = prescale_;
      frame->Put("FilterMask2BoolsTestResult", result);
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
        expectation_(true)
    {
      AddParameter("Expectation", "blah", expectation_);

      AddOutBox("OutBox");
    }

    void Configure()
    {
      GetParameter("Expectation", expectation_);
    }
    
    void Physics(I3FramePtr frame)
    {
      I3BoolConstPtr input = frame->Get<I3BoolConstPtr>("FilterMask2BoolsTestName");
      ENSURE((bool)input);
      if(expectation_) ENSURE(input->value);
      else ENSURE(!input->value);
      PushFrame(frame,"OutBox");
    }
    
   private:
    bool expectation_;
  };
}

I3_MODULE(FilterMask2BoolsTestUtil::Source);
I3_MODULE(FilterMask2BoolsTestUtil::Test);

TEST_GROUP(FilterMask2BoolsTest);

TEST(ConsiderPrescale)
{
  I3Tray tray;
  
  tray.AddModule("FilterMask2BoolsTestUtil::Source", "input")
    ("Condition", true)
    ("Prescale", false);
  
  tray.AddModule("FilterMask2Bools", "processor")
    ("FilterResultName", "FilterMask2BoolsTestResult")
    ("IgnorePrescale", false);
  
  tray.AddModule("FilterMask2BoolsTestUtil::Test", "test")
    ("Expectation", false);
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

TEST(IgnorePrescale)
{
  I3Tray tray;
  
  tray.AddModule("FilterMask2BoolsTestUtil::Source", "input")
    ("Condition", true)
    ("Prescale", false);
  
  tray.AddModule("FilterMask2Bools", "processor")
    ("FilterResultName", "FilterMask2BoolsTestResult")
    ("IgnorePrescale", true);
  
  tray.AddModule("FilterMask2BoolsTestUtil::Test", "test")
    ("Expectation", true);
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}
