#include <ppc-eff/I3AdjEff.h>

using namespace std;

I3_MODULE(AdjEff);

AdjEff::AdjEff(const I3Context& ctx) : I3Module(ctx){
  AddParameter("eff", "Efficiency factor", eff);
  AddParameter("set", "Set of DOMs to adjust", set);
  AddOutBox("OutBox");
}

void AdjEff::Configure(){
  GetParameter("eff", eff);
  GetParameter("set", set);
  for(vector<OMKey>::const_iterator i=set.begin(); i!=set.end(); ++i) oms[*i]++;
}

void AdjEff::DAQ(I3FramePtr frame){
  log_trace("Entering adjeff::DAQ()");

  bool flag=false;
  vector<std::string> keys;
  I3MCPESeriesMapPtr newlist = I3MCPESeriesMapPtr(new I3MCPESeriesMap);

  for(I3Frame::const_iterator iter=frame->begin(); iter!=frame->end(); iter++){
    I3MCPESeriesMapConstPtr hitlist;
    try{ hitlist = boost::dynamic_pointer_cast<const I3MCPESeriesMap>(iter->second); } catch(...){ }
    if(hitlist){
      flag=true;

      for(I3MCPESeriesMap::const_iterator it=hitlist->begin(); it!=hitlist->end(); ++it){
	OMKey omkey=it->first;
	I3MCPESeries hits=it->second;

        if(oms.empty() || oms.find(omkey) != oms.end()){
          for(I3MCPESeries::const_iterator i = hits.begin(); i != hits.end(); ++i){
            if(drand48() < eff) (*newlist)[omkey].push_back(*i);
	  }
	}
	else (*newlist)[omkey] = hits;
      }

      keys.push_back(iter->first);
    }
  }

  if(flag){
    for(vector<std::string>::const_iterator i=keys.begin(); i!=keys.end(); ++i) frame->Delete(*i);
    frame->Put("MCPESeriesMap", newlist);
  }
  PushFrame(frame,"OutBox");
}
