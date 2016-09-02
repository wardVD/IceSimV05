#include <I3Test.h>

#include <icetray/I3Tray.h>
#include <icetray/I3Frame.h>

#include <icetray/I3Bool.h>
#include <phys-services/I3GSLRandomServiceFactory.h>

//#include <filter-tools/CreateFilterMask.h>

TEST_GROUP(I3FilterResultTest);

namespace TestI3FilterResult
{
  class TestSource : public I3Module
  {
  public:
    TestSource(const I3Context& context): 
      I3Module(context),
      frame_number_(0)
    {
      AddOutBox("OutBox");
    }
    void Process()
    {
      if(frame_number_ >=100 )
	{
	  RequestSuspension();
	  return;
	}

      I3RandomServicePtr random=context_.Get<I3RandomServicePtr>();

      I3FramePtr frame(new I3Frame(I3Frame::Physics));

      I3BoolPtr red(new I3Bool(random->Integer(2)));
      I3BoolPtr green(new I3Bool(random->Integer(2)));
      I3BoolPtr blue(new I3Bool(random->Integer(2)));

      frame->Put("Red",red);
      frame->Put("Green",green);
      frame->Put("Blue",blue);

      PushFrame(frame,"OutBox");
      frame_number_++;
    }
  private:
    unsigned frame_number_;
    
  };
  
  class TestFilterResult : public I3Module
  {
  public:
    TestFilterResult(const I3Context& context): I3Module(context)
    {
      AddOutBox("OutBox");
    }
    
    void Physics(I3FramePtr frame)
    {
      ENSURE(frame->Get<I3Bool>("Red").value||
	     frame->Get<I3Bool>("Green").value||
	     frame->Get<I3Bool>("Blue").value ==
	     frame->Get<I3Bool>("FilterMaskFilter").value);
      PushFrame(frame,"OutBox");
    }
    
  };

I3_MODULE(TestSource);
I3_MODULE(TestFilterResult);

};

TEST(TimeFiltering)
{
  I3Tray tray;

  tray.AddService("I3GSLRandomServiceFactory","random");
  
  tray.AddModule("TestSource","source");

  tray.AddModule("CreateFilterMask","createfiltermask");

  std::vector <std::string> v;
  v.push_back("Red");
  v.push_back("Green");
  v.push_back("Blue");
  
  tray.AddModule("I3IcePickModule<FilterMaskFilter>","filtermaskfilter")
    ("FilterNameList",v);

  tray.AddModule("TestFilterResult","client");

  tray.AddModule("TrashCan","trash");

  tray.Execute();
  tray.Finish();
}
