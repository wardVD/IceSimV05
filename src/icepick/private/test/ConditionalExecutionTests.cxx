#include <I3Test.h>

TEST_GROUP(ConditionalExecutionTests);

#include <icetray/I3Tray.h>
#include <icetray/I3Bool.h>
#include <icetray/I3IcePick.h>
#include <icetray/I3IcePickInstaller.h>
#include <icetray/I3ConditionalModule.h>

#include <icepick/I3OrServices.h>
#include <icepick/I3AndServices.h>

#include <string>
#include <vector>

using namespace std;

class TestConditionalPutModule : public I3ConditionalModule
{
public:
  TestConditionalPutModule(const I3Context& context) :
    I3ConditionalModule(context)
  {
    AddOutBox("OutBox");
  }

  void Configure() { }

  void Physics(I3FramePtr frame)
  {
    frame->Put("fired", I3FrameObjectPtr());
    PushFrame(frame,"OutBox");
  }
};
I3_MODULE(TestConditionalPutModule);

class DumbPick : public I3IcePick
{
public:
  DumbPick(const I3Context& context) :
    I3IcePick(context),
    key("")
  {
    AddParameter("CheckThisKeyInFrame",
		 "Key where there's a bool that says if the frame passes.",
		 key);
  }

  void Configure()
  {
    GetParameter("CheckThisKeyInFrame",key);
  }

  bool SelectFrame(I3Frame& frame)
  {
    return frame.Has(key);
  }

private:

  string key;
};
I3_SERVICE_FACTORY(I3IcePickInstaller<DumbPick>);

class TesterSource : public I3Module
{
public:
  TesterSource(const I3Context& context) :
    I3Module(context),
    called(0)
  {
    AddOutBox("OutBox");
  }

  void Process()
  {
    I3FramePtr frame(new I3Frame(I3Frame::Physics));

    if(called < 2) 
      frame->Put("lt_two",I3FrameObjectPtr());

    if(called >= 2) 
      frame->Put("ge_two",I3FrameObjectPtr());

    if(called % 2 == 0) 
      frame->Put("mod_two",I3FrameObjectPtr());

    if(called % 3 == 0) 
      frame->Put("mod_three",I3FrameObjectPtr());

    PushFrame(frame,"OutBox");
    ++called;
  }
private:
  int called;
};
I3_MODULE(TesterSource);

TEST(mod_two_or_mod_three)
{
  I3Tray tray;

  tray.AddService("I3IcePickInstaller<DumbPick>","mod_two_pick")
    ("CheckThisKeyInFrame","mod_two");

  tray.AddService("I3IcePickInstaller<DumbPick>","mod_three_pick")
    ("CheckThisKeyInFrame","mod_three");

  vector<string> picks;
  picks.clear();
  picks.push_back("mod_two_pick");
  picks.push_back("mod_three_pick");

  tray.AddService("I3IcePickInstaller<I3OrServices>","mod_two_or_mod_three_pick")
    ("ServicesToOr",picks);

  tray.AddModule("TesterSource","thesource");

  tray.AddModule("TestConditionalPutModule", "mod")
    ("IcePickServiceKey","mod_two_or_mod_three_pick");

  tray.AddModule("CountObject", "co_fired")
    ("where", "fired")
    ("expected", 7)
    ;

  tray.AddModule("CountObject", "co_mod_two")
    ("where", "mod_two")
    ("expected", 5)
    ;

  tray.AddModule("CountObject", "co_mod_three")
    ("where", "mod_three")
    ("expected", 4)
    ;

  tray.AddModule("Dump", "dump");
  tray.AddModule("TrashCan", "thecan");

  tray.Execute(10);
  tray.Finish();
}

TEST(mod_two_and_mod_three)
{
  I3Tray tray;

  tray.AddService("I3IcePickInstaller<DumbPick>","mod_two_pick")
    ("CheckThisKeyInFrame","mod_two");

  tray.AddService("I3IcePickInstaller<DumbPick>","mod_three_pick")
    ("CheckThisKeyInFrame","mod_three");

  vector<string> picks;
  picks.clear();
  picks.push_back("mod_two_pick");
  picks.push_back("mod_three_pick");

  tray.AddService("I3IcePickInstaller<I3AndServices>","mod_two_and_mod_three_pick")
    ("ServicesToAnd",picks);

  tray.AddModule("TesterSource","thesource");

  tray.AddModule("TestConditionalPutModule","dumbmodule")
    ("IcePickServiceKey","mod_two_and_mod_three_pick");

  tray.AddModule("CountObject", "co")
    ("where", "fired")
    ("expected", 2)
    ;

  tray.AddModule("Dump", "dump");
  tray.AddModule("TrashCan","thecan");

  tray.Execute(10);
  tray.Finish();
}

TEST(not_mod_three) // Check the InverService for CE
{
  I3Tray tray;

  tray.AddService("I3IcePickInstaller<DumbPick>","mod_three_pick")
    ("CheckThisKeyInFrame","mod_three");
  tray.AddService("I3IcePickInstaller<I3InverseServices>","not_mod_three_pick")
    ("ServiceToInvert","mod_three_pick");
  tray.AddModule("TesterSource","thesource");

  tray.AddModule("TestConditionalPutModule","dumbmodule")
    ("IcePickServiceKey","not_mod_three_pick");

  tray.AddModule("CountObject", "co")
    ("where", "fired")
    ("expected", 6)
    ;
  tray.AddModule("Dump", "dump");
  tray.AddModule("TrashCan","thecan");

  tray.Execute(10);
  tray.Finish();
}
