#include <I3Test.h>

#include <icetray/I3Tray.h>
#include <icetray/I3IcePick.h>
#include <interfaces/I3IcePickModule.h>
#include <icetray/I3Bool.h>

class TrueEventFilter : public I3IcePick
{

public:

  TrueEventFilter(const I3Context& context) : I3IcePick(context)
  {
  }

  bool SelectFrame(I3Frame& frame)
  {
    I3BoolConstPtr bp = frame.Get<I3BoolConstPtr>("bool");
    ENSURE((bool)bp);
    return bp->value;
  }

};

typedef I3IcePickModule<TrueEventFilter> TrueEventFilterPick;

I3_MODULE(TrueEventFilterPick);

class RandBoolFillingModule : public I3Module
{
public:

  RandBoolFillingModule(const I3Context& context)  : I3Module(context)
  {
    AddOutBox("OutBox");
  }

  void Physics(I3FramePtr frame)
  {
    I3BoolPtr b(new I3Bool(rand()%2));
    frame->Put("bool",b);
    PushFrame(frame,"OutBox");
  }

};

I3_MODULE(RandBoolFillingModule);

class FalseCheckingModule : public I3Module
{
public:
  FalseCheckingModule(const I3Context& context) : I3Module(context)
  { }

  void Physics(I3FramePtr frame)
  {
    I3BoolConstPtr bp = frame->Get<I3BoolConstPtr>("bool");
    ENSURE((bool)bp);
    // Dropped pretzian logic:  "true" now passes filter
    ENSURE(bp->value == true); // since if it were true the pick
				// woulda filtered it
  }
};
I3_MODULE(FalseCheckingModule);


TEST_GROUP(TrueEventFilterTest);

TEST(simple)
{
  I3Tray tray;

  tray.AddModule("BottomlessSource","source");

  tray.AddModule("RandBoolFillingModule", "rand");

  tray.AddModule("TrueEventFilterPick","pick")
    ("DiscardEvents", true);

  tray.AddModule("FalseCheckingModule", "falsecheck");

  tray.Execute(100);
  tray.Finish();
}
