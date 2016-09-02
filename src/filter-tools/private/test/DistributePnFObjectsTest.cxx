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

namespace DistributePnFObjectsTestUtil
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
      I3IntPtr myint0 (new I3Int);
      myint0->value = 0;
      I3IntPtr myint1 (new I3Int);
      myint0->value = 1;
      I3IntPtr myint2 (new I3Int);
      myint0->value = 2;
      qframe->Put("Keeper_InIceSplit0",myint0);
      qframe->Put("Keeper_InIceSplit1",myint1);
      qframe->Put("Keeper_NullSplit0",myint1);
      I3EventHeaderPtr eh(new I3EventHeader);
      qframe->Put("I3EventHeader",eh);
      PushFrame(qframe,"OutBox");
      I3FramePtr pframe0(new I3Frame(I3Frame::Physics));
      I3EventHeaderPtr eh0(new I3EventHeader);
      eh0->SetSubEventStream("InIceSplit");
      eh0->SetSubEventID(0);
      pframe0->Put("I3EventHeader",eh0);
      PushFrame(pframe0,"OutBox");
      I3FramePtr pframe1(new I3Frame(I3Frame::Physics));
      I3EventHeaderPtr eh1(new I3EventHeader);
      eh1->SetSubEventStream("InIceSplit");
      eh1->SetSubEventID(1);
      pframe1->Put("I3EventHeader",eh1);
      PushFrame(pframe1,"OutBox");
      I3FramePtr pframe2(new I3Frame(I3Frame::Physics));
      I3EventHeaderPtr eh2(new I3EventHeader);
      eh2->SetSubEventStream("NullSplit");
      eh2->SetSubEventID(0);
      pframe2->Put("I3EventHeader",eh2);
      PushFrame(pframe2,"OutBox");	
      
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
      I3IntConstPtr myint = frame->Get<I3IntConstPtr>("Keeper");
      ENSURE(frame->Has("Keeper"));
      ENSURE(!frame->Has("Keeper_IceIceSplit0"));
      ENSURE(!frame->Has("Keeper_IceIceSplit1"));
      ENSURE(!frame->Has("Keeper_NullSplit0"));
      PushFrame(frame,"OutBox");
    }
    
   private:
  };
}

I3_MODULE(DistributePnFObjectsTestUtil::Source);
I3_MODULE(DistributePnFObjectsTestUtil::Test);

TEST_GROUP(DistributePnFObjTest);

TEST(DistributePnFObjects)
{
  I3Tray tray;
  
  tray.AddModule("DistributePnFObjectsTestUtil::Source", "input");
  
  tray.AddModule("DistributePnFObjects", "distribute");
  
  tray.AddModule("DistributePnFObjectsTestUtil::Test", "test");

  tray.AddModule("Dump","dumper");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}

