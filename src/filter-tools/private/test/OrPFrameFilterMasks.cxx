#include <I3Test.h>

#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Bool.h>
#include <icetray/I3Int.h>
#include <dataclasses/physics/I3FilterResult.h>
#include <dataclasses/physics/I3EventHeader.h>

using namespace std;

namespace OrPFrameFilterMasksTestUtil
{
  class Source : public I3Module
  {
   public:
    Source(const I3Context& context)
      : I3Module(context)
    {
      AddOutBox("OutBox");
    }

    void Configure()
    {
    }
  
    void Process()
    {
      I3FramePtr qframe(new I3Frame(I3Frame::DAQ));
      PushFrame(qframe,"OutBox");      
      I3FramePtr pframe0(new I3Frame(I3Frame::Physics));
      I3FilterResultMapPtr result0 (new I3FilterResultMap());
      (*result0)["Filter1"].conditionPassed = true;
      (*result0)["Filter1"].prescalePassed = true;
      (*result0)["Filter2"].conditionPassed = false;
      (*result0)["Filter2"].prescalePassed = false;
      (*result0)["Filter3"].conditionPassed = false;
      (*result0)["Filter3"].prescalePassed = false;
      (*result0)["Filter4"].conditionPassed = true;
      (*result0)["Filter4"].prescalePassed = false;
      pframe0->Put("FilterMask", result0);
      PushFrame(pframe0,"OutBox");	  
      I3FramePtr pframe1(new I3Frame(I3Frame::Physics));
      I3FilterResultMapPtr result1 (new I3FilterResultMap());
      (*result1)["Filter1"].conditionPassed = false;
      (*result1)["Filter1"].prescalePassed = false;
      (*result1)["Filter2"].conditionPassed = true;
      (*result1)["Filter2"].prescalePassed = true;
      (*result1)["Filter3"].conditionPassed = false;
      (*result1)["Filter3"].prescalePassed = false;
      (*result1)["Filter4"].conditionPassed = true;
      (*result1)["Filter4"].prescalePassed = true;
      pframe1->Put("FilterMask", result1);

      PushFrame(pframe1,"OutBox");	
      
    }
  
   private:
  };

  class Test : public I3Module
  {
   public:
    Test(const I3Context& context)
      : I3Module(context)
    {
      AddOutBox("OutBox");
    }

    void Configure()
    {
    }
    
    void DAQ(I3FramePtr frame)
    {
      ENSURE(frame->Has("QFilterMask"));
      I3FilterResultMapConstPtr mymask = frame->Get<I3FilterResultMapConstPtr>("QFilterMask");
      I3FilterResultMap realmap = *mymask;
      ENSURE(realmap["Filter1"].conditionPassed == true);
      ENSURE(realmap["Filter1"].prescalePassed == true);
      ENSURE(realmap["Filter2"].conditionPassed == true);
      ENSURE(realmap["Filter2"].prescalePassed == true);
      ENSURE(realmap["Filter3"].conditionPassed == false);
      ENSURE(realmap["Filter3"].prescalePassed == false);
      ENSURE(realmap["Filter4"].conditionPassed == true);
      ENSURE(realmap["Filter4"].prescalePassed == true);

      PushFrame(frame,"OutBox");
    }
    
   private:
  };
}

I3_MODULE(OrPFrameFilterMasksTestUtil::Source);
I3_MODULE(OrPFrameFilterMasksTestUtil::Test);

TEST_GROUP(OrPFrameFilterMasksTest);

TEST(OrPFrames)
{
  I3Tray tray;
  
  tray.AddModule("OrPFrameFilterMasksTestUtil::Source", "input");
  
  tray.AddModule("OrPframeFilterMasks", "orpframes")
    ("InputName","FilterMask")
    ("OutputName","QFilterMask");
  
  tray.AddModule("OrPFrameFilterMasksTestUtil::Test", "test");

  tray.AddModule("Dump","dumper");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

