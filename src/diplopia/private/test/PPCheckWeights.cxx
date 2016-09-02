#include <test/PPCheckWeights.h>

#include <I3Test.h>
#include <icetray/I3Frame.h>
#include <icetray/OMKey.h>
#include <icetray/I3Units.h>
#include <simclasses/I3MCPE.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <phys-services/I3RandomService.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <simclasses/I3MCPE.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <simclasses/I3MMCTrack.h>
#include <boost/foreach.hpp>



using namespace std;

I3_MODULE(PPCheckWeights);

PPCheckWeights::PPCheckWeights(const I3Context& context) : 
  I3Module(context),
  mapName_("MCPESeriesMap"),
  mcTreeName_("I3MCTree"),
  mmcInfoName_("MMCTrackList"),
  weight_name_("CorsikaWeightMap"),	// I3Double in the Frame
  counter_(0)
{
  AddOutBox("OutBox");
  AddParameter("MCPESeriesMapName","Output map with noise hits",mapName_);
  AddParameter("MCTreeName","Name of I3MCTree in frame",mcTreeName_);
  AddParameter("MMCTrackListName","Name of MMCTrack list",mmcInfoName_);
  AddParameter("WeightName", "Name to give the frame", weight_name_);
}

void
PPCheckWeights::Configure()
{
  GetParameter("MCPESeriesMapName",mapName_);
  GetParameter("MCTreeName",mcTreeName_);
  GetParameter("MMCTrackListName",mmcInfoName_);
  GetParameter("WeightName", weight_name_);
}

void
PPCheckWeights::DAQ(I3FramePtr frame)
{
  counter_++;
  const I3MCTree mctree = frame->Get<I3MCTree>(mcTreeName_);
  const I3MMCTrackList mmcTrackList = frame->Get<I3MMCTrackList>(mmcInfoName_);
  const I3MCPESeriesMap& hitMap = frame->Get<I3MCPESeriesMap>(mapName_);
  const I3MapStringDouble& weightdict = frame->Get<I3MapStringDouble>(weight_name_);

  unsigned primary_cnt = I3MCTreeUtils::GetPrimaries(mctree).size();
  if (primary_cnt != weightdict.at("Multiplicity")) 
        log_fatal("Frame %u: Number of coincidences (%u) does not match info in weight dict (%f)", 
                        counter_, 
                        primary_cnt, 
                        weightdict.at("Multiplicity"));

  PushFrame(frame,"OutBox");
}

