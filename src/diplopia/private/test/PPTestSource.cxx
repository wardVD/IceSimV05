#include <test/PPTestSource.h>

#include <I3Test.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Int.h>
#include <icetray/OMKey.h>
#include <icetray/I3Units.h>
#include <simclasses/I3MCPE.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <phys-services/I3RandomService.h>
#include <boost/foreach.hpp>

using namespace std;

I3_MODULE(PPTestSource);

PPTestSource::PPTestSource(const I3Context& context) : 
  I3Module(context),
  nPEs_(1000),
  first_(0.*I3Units::ns),
  last_(6000.*I3Units::ns),
  emin_(600*I3Units::GeV),
  emax_(1e11*I3Units::GeV),
  icmapName_("I3MCPESeriesMap"),
  mcTreeName_("I3MCtree"),
  mmcInfoName_("MMCTrackList"),
  counter_(0)
{
  AddOutBox("OutBox");
  AddParameter("NPEs","Number of hits to put in the frame",nPEs_);
  AddParameter("EarliestTime","Time of first hit",first_);
  AddParameter("LatestTime","Time of last hit",last_);
  AddParameter("MCPESeriesMapName","Output map with noise hits",icmapName_);
  AddParameter("MCTreeName","Name of I3MCTree in frame",mcTreeName_);
  AddParameter("MMCTrackListName","Name of MMCTrack list",mmcInfoName_);
  AddParameter("EnergyMin","Minimum energy",emin_);
  AddParameter("EnergyMax","Maximum energy",emax_);
}

void
PPTestSource::Configure()
{
  GetParameter("NPEs",nPEs_);
  GetParameter("EarliestTime",first_);
  GetParameter("LatestTime",last_);
  GetParameter("MCPESeriesMapName",icmapName_);
  GetParameter("MCTreeName",mcTreeName_);
  GetParameter("MMCTrackListName",mmcInfoName_);
  GetParameter("EnergyMin",emin_);
  GetParameter("EnergyMax",emax_);
  if (emin_ > emax_) 
          log_fatal("emin > emax. Check your configuration");

}

void
PPTestSource::DAQ(I3FramePtr frame)
{
  counter_++;
  I3RandomServicePtr random  = context_.Get<I3RandomServicePtr>();
  if(!random) log_fatal("no random number service installed");

  I3MCTreePtr mctree(new I3MCTree());
  I3ParticlePtr primary(new I3Particle());
  double x,y,z,theta,phi,t,e;

  x = rand(); y = rand(); z = rand();
  theta = 180.*rand()/RAND_MAX; phi = 360.*rand()/RAND_MAX;
  t = rand();
  e = emin_*(1.-random->Exp(.5)); // E^-2
  while (e > emax_) 
          e = emin_*(1.-random->Exp(.5)); 

  primary->SetType(I3Particle::MuMinus);
  primary->SetPos(x*I3Units::m, y*I3Units::m, z*I3Units::m);
  primary->SetTime(t*I3Units::ns);
  primary->SetDir(theta*I3Units::deg,phi*I3Units::deg);
  primary->SetEnergy(e*I3Units::MeV);
  primary->SetLocationType(I3Particle::InIce);
  I3MCTreeUtils::AddPrimary(*mctree,*primary);

  frame->Put(mcTreeName_,mctree);
  frame->Put("frame_number",I3IntPtr(new I3Int(counter_)));

  I3MMCTrackListPtr mmcTrackList(new I3MMCTrackList);
  frame->Put(mmcInfoName_,mmcTrackList);
  
  vector<OMKey> omlist;
  const I3Geometry geo = frame->Get<I3Geometry>();
  BOOST_FOREACH(I3OMGeoMap::const_reference g, geo.omgeo){
    if( g.second.omtype == I3OMGeo::IceCube || g.second.omtype == I3OMGeo::IceTop ) 
            omlist.push_back(g.first);
  }

  I3MCPESeriesMapPtr ichitMap(new I3MCPESeriesMap);
  I3MCPESeriesMapPtr hitMap;

  for(int i=0; i<nPEs_; i++){
    I3MCPE h;
    h.time = random->Uniform(first_,last_);
    h.ID = primary->GetID();
    OMKeyPtr om;

    int index = static_cast<int>(random->Uniform(0,omlist.size()-1));
    om = OMKeyPtr(new OMKey(omlist[index]));
    hitMap = ichitMap;

    if(hitMap->find(*om) != hitMap->end()){
      hitMap->find(*om)->second.push_back(h);
    }else{
      I3MCPESeries hs;
      hs.push_back(h);
      (*hitMap)[*om] = hs;
    }
  }
  
  frame->Put(icmapName_,ichitMap);
  frame->Put("DrivingTime", I3TimePtr(new I3Time(2011, 0)));
  PushFrame(frame,"OutBox");
}

