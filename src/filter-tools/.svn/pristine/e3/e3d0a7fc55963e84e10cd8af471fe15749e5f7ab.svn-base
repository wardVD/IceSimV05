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

namespace KeepFromSubstreamsTestUtil
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
      I3EventHeaderPtr eh(new I3EventHeader);
      qframe->Put("I3EventHeader",eh);
      PushFrame(qframe,"OutBox");

      I3FramePtr pframe0(new I3Frame(I3Frame::Physics));
      I3EventHeaderPtr eh0(new I3EventHeader);
      eh0->SetSubEventStream("InIceSplit");
      eh0->SetSubEventID(0);
      pframe0->Put("I3EventHeader",eh0);
      I3IntPtr myint0 (new I3Int);
      myint0->value = 0;
      I3IntPtr myint1 (new I3Int);
      myint0->value = 1;
      pframe0->Put("KeepMe",myint0);
      pframe0->Put("DropMe",myint1);
      PushFrame(pframe0,"OutBox");

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
    
    void Physics(I3FramePtr frame)
    {
      ENSURE(frame->Has("Any"),"Should never be called, I eat PFrames");
      PushFrame(frame,"OutBox");
    }
    void DAQ(I3FramePtr frame)
    {
      ENSURE(frame->Has("KeepMe_InIceSplit0"));
      ENSURE(!frame->Has("DropMe_InIceSplit0"));
      ENSURE(!frame->Has("DropMe"));
      ENSURE(!frame->Has("KeepMe"));
      PushFrame(frame,"OutBox");
    }
    
   private:
  };
}

I3_MODULE(KeepFromSubstreamsTestUtil::Source);
I3_MODULE(KeepFromSubstreamsTestUtil::Test);

TEST_GROUP(KeepFromSubstreamsTest);

TEST(KeepFromSubstreams)
{
  I3Tray tray;
  
  tray.AddModule("KeepFromSubstreamsTestUtil::Source", "input");
  
  std::vector <std::string> keepers;
  keepers.push_back("KeepMe");
    
  tray.AddModule("KeepFromSubstream", "distribute")
    ("StreamName","InIceSplit")
    ("KeepKeys",keepers);

  tray.AddModule("Dump","dumper");
  
  tray.AddModule("KeepFromSubstreamsTestUtil::Test", "test");

    tray.AddModule("TrashCan", "trash");

  tray.Execute(3);
  tray.Finish();  
}

