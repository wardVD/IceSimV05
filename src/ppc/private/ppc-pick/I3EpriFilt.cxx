#include <ppc-pick/I3EpriFilt.h>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3MCList.h>

#include <interfaces/I3IcePickModule.h>
#include <interfaces/I3IceForkModule.h>
#include <icetray/I3IcePickInstaller.h>

using namespace std;

I3EpriFilt::I3EpriFilt(const I3Context& context) : I3IcePick(context), maxE_(1.e9*I3Units::GeV) {
  AddParameter("MaxEpri", "maximum allowed energy of the primary", maxE_);
}

void I3EpriFilt::Configure() {
  GetParameter("MaxEpri", maxE_);
}

bool I3EpriFilt::SelectFrame(I3Frame& frame) {

  bool flag=true, verbose=false;
  for (I3Frame::const_iterator iter=frame.begin(); iter!=frame.end(); iter++){
    I3MCTreeConstPtr mctree = boost::dynamic_pointer_cast<const I3MCTree>(iter->second);
    if(mctree){
      if(verbose) cout<<"  --> This is an I3MCTree: "<<iter->first<<endl;
      int num=0;
      const vector<I3Particle>& primaries = I3MCTreeUtils::GetPrimaries(*mctree);
      for(vector<I3Particle>::const_iterator it=primaries.begin(); it!=primaries.end(); ++it, num++){
	const I3Particle& p=*it;
	if(verbose) cout<<"inspecting particle "<<p.GetType()<<endl;
	if(p.GetEnergy()>maxE_){ flag=false; break; }
      }
    }
  }

  return flag;
}

I3_MODULE(I3IcePickModule<I3EpriFilt>);
I3_MODULE(I3IceForkModule<I3EpriFilt>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3EpriFilt>);

