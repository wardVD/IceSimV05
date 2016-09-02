/**
    copyright  (C) 2004
    the icecube collaboration

    @version $Revision: 137657 $
    @date $Date: 2015-09-16 18:12:00 -0500 (Wed, 16 Sep 2015) $
    @author Kevin J Meagher
*/

#include <I3Test.h>
#include <filter-tools/ParticleCheck.h>
#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Position.h>
#include <icetray/I3Tray.h>
#include <dataclasses/physics/I3Particle.h>

using namespace std;

namespace ParticleCheckTestUtil
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
      I3FramePtr pframe(new I3Frame(I3Frame::Physics));
      I3ParticlePtr mypart0 (new I3Particle);
      mypart0->SetTime(0.0);
      mypart0->SetLength(100.0);
      mypart0->SetPos(1.0, 2.0, 3.0);
      mypart0->SetDir(2.5,3.5);
      cout<<AsXML(mypart0);
      I3ParticlePtr mypart1 (mypart0);

      pframe->Put("TestParticle0",mypart0);
      pframe->Put("TEST_TestParticle0",mypart1);
      PushFrame(pframe,"OutBox");
    }
    
  private:
  };
}
I3_MODULE(ParticleCheckTestUtil::Source);

TEST_GROUP(ParticleCheckTest);


TEST(RunTheParticleCheckInTray)
{
  I3Tray tray;
  
  tray.AddModule("ParticleCheckTestUtil::Source", "input");
  
  std::vector <std::string> part_names;
  part_names.push_back("TestParticle0");

  tray.AddModule("ParticleCheck","checkparts")
    ("ParticleNames",part_names)
    ("ParticlePrefix","TEST_");

  tray.AddModule("Dump","dumper");
  
  tray.AddModule("TrashCan", "trash");
  
  tray.Execute(1);
  tray.Finish();  
}



