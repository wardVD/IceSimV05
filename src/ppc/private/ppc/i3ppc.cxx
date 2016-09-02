#include <ppc/i3ppc.h>
#include "dataclasses/physics/I3MCList.h"
#include <phys-services/I3RandomService.h>
#include "ppc.h"

#include <boost/foreach.hpp>

I3_MODULE(i3ppc);

using namespace std;

struct xtrPulses{
  map<OMKey, int> bad;
 
  void addOM(OMKey omkey, double V, double eff){
    if(bad.find(omkey)==bad.end() && isfinite(V) && V>0){
      if(!isfinite(eff) || eff<0) eff=1.0;
      xppc::ikey om;
      om.str=omkey.GetString();
      om.dom=omkey.GetOM();
      xppc::hvs[om]=V;
      xppc::rdes[om]=eff;
    }
  }

  void addHit(OMKey & key, I3MCPESeriesMap & hsm, I3MCPE & hit, double t){
    hit.time = t;
    hsm[key].push_back(hit);
  }
} pz;

i3ppc::i3ppc(const I3Context &context) : I3ConditionalModule(context),
		 ini(false),
		 verbose(false),
		 cyl(true),
		 keep_(0),
		 gpu(0),
		 nph(0),
		 wid(0*I3Units::ns)
{
  AddParameter ("gpu", "GPU to use", gpu);
  AddParameter ("bad", "DEPRECATED : DOMs not to use", bad);
  AddParameter ("fla", "Flasher position", fla);
  AddParameter ("nph", "Number of photons", nph);
  AddParameter ("wid", "Flasher pulse width", wid);
  AddParameter ("MCTree", "MCTree to use", mct);
  AddParameter ("cyl", "use cylinder (1) or strict +300 m (0) detector volume", cyl);
  AddParameter ("keep", "Keep events that don't produce PEs",keep_);

  AddOutBox("OutBox");

  xppc::start();

  Radius=0, Top=0, Bottom=0, Padding=300;
}

i3ppc::~i3ppc(){
  xppc::stop();
}

void i3ppc::Configure(){
  GetParameter ("gpu", gpu);
  GetParameter ("bad", bad);
  GetParameter ("fla", fla);
  GetParameter ("nph", nph);
  GetParameter ("wid", wid); wid/=I3Units::ns;
  GetParameter ("MCTree", mct);
  GetParameter ("cyl", cyl);
  GetParameter ("keep", keep_);
  fb=0, fe=0;

  if (bad.size() !=0)
    log_error("This option is DEPRECATED: " 
    "BadDomList is now read from the DetectorStatus; Lists now will be appended");
}

void i3ppc::DetectorStatus(I3FramePtr frame){
  // Build the BadDomList by searching the detectorstatus for doms, which have HV==0,
  // a sure sign that this DOM is either switched off or is bad;
  // A module should have been run at GCD validation level for simulation that did some sanity ckecks
  // that DOMs contained in <vectorOMKey>"BadDomList" are removed in that way from the GCD
  I3DetectorStatusConstPtr status = frame->Get<I3DetectorStatusConstPtr>();
  
  BOOST_FOREACH(I3DOMStatusMap::value_type omkey_domstatus, status->domStatus) {
    if (omkey_domstatus.second.pmtHV==0.) {
      pz.bad[omkey_domstatus.first]++;
      log_debug_stream("Excluded DOM " << omkey_domstatus.first << " from Simulation");
    }
  }
  PushFrame(frame,"OutBox");
}

void i3ppc::Physics(I3FramePtr frame){
  log_trace("Entering i3ppc::DAQ()");
  frames.push_back(frame);

  if(!ini){
    if(verbose) cout<<"Geometry frame"<<endl;
    I3GeometryConstPtr geo = frame->Get<I3GeometryConstPtr>();
    I3CalibrationConstPtr calib = frame->Get<I3CalibrationConstPtr>();
    I3DetectorStatusConstPtr status = frame->Get<I3DetectorStatusConstPtr>();
    if(verbose){
      if(!geo) cout<<"    --> empty"<<endl;
      else cout<<"    --> contains "<<geo->omgeo.size()<<" entries"<<endl;
    }
    if(geo) if(geo->omgeo.size()>0){
      for(I3OMGeoMap::const_iterator it=geo->omgeo.begin(); it!=geo->omgeo.end(); ++it){
	OMKey omkey=it->first;
	if(omkey.IsInIce()){
	  xppc::OM om;
	  om.str=omkey.GetString();
	  om.dom=omkey.GetOM();
	  const I3Position& pos = (it->second).position;
	  om.r[0]=pos.GetX()/I3Units::m;
	  om.r[1]=pos.GetY()/I3Units::m;
	  om.r[2]=pos.GetZ()/I3Units::m;
	  xppc::i3oms.push_back(om);

	  if(status){
	    map<OMKey, I3DOMStatus>::const_iterator om_stat = status->domStatus.find(omkey);
	    if(om_stat!=status->domStatus.end()){
	      double eff=1.0;
	      if(calib){
		map<OMKey, I3DOMCalibration>::const_iterator om_cal = calib->domCal.find(omkey);
		if(om_cal!=calib->domCal.end()) eff=om_cal->second.GetRelativeDomEff();
	      }
	      pz.addOM(omkey, om_stat->second.pmtHV/I3Units::V, eff);
	    }
	  }

	  double R=sqrt(om.r[0]*om.r[0]+om.r[1]*om.r[1]); if(R>Radius) Radius=R;
	  if(om.r[2]>Top) Top=om.r[2]; else if(om.r[2]<Bottom) Bottom=om.r[2];
	}
      }

      xppc::initialize(1.);
      xppc::choose(gpu);

      if(nph>0){
	xppc::flini(fla.GetString(), fla.GetOM()), fe++;
	// xppc::sett(0, 0, 1, make_pair(-1, -1), 0);
      }
      else xppc::ini(0);
      ini=true;
    }
  }

  if(nph>0){
    xppc::flone((unsigned long long)llroundf(nph)); fb=0;
  }
  else{
    for(I3Frame::const_iterator iter=frame->begin(); iter!=frame->end(); iter++){
      I3MMCTrackListConstPtr mmclist = boost::dynamic_pointer_cast<const I3MMCTrackList>(iter->second);
      if(mmclist){
	if(verbose) cout<<"  --> This is an I3MMCTrackList: "<<iter->first<<endl;
	for(I3MMCTrackList::const_iterator it=mmclist->begin(); it!=mmclist->end(); ++it){
	  const I3MMCTrack& m=*it;
	  const I3Particle& p=it->GetI3Particle();
	  i3mmctracks[make_pair(p.GetMinorID(), p.GetMajorID())]=&m;
	}
      }
    }

    xppc::eini();
    for(I3Frame::const_iterator iter=frame->begin(); iter!=frame->end(); iter++){
      string name = iter->first;
      if(mct.compare("")!=0 && name.compare(mct)!=0) continue;
      I3MCTreeConstPtr mctree = boost::dynamic_pointer_cast<const I3MCTree>(iter->second);
      if(mctree){
	if(verbose) cout<<"  --> This is an I3MCTree: "<<iter->first<<endl;
	for(I3MCTree::sibling_const_iterator it=mctree->begin(); it!=mctree->end_sibling(); ++it){
	  if(verbose) cout<<"extracting a particle "<<it->GetType()<<endl;
	  pparticle(mctree, it);
	}
      }
    }

    i3mmctracks.clear();
    fe++;
  }

  pushframe();
}

void i3ppc::popframes(int fx){
  for(; fb<fx; fb++){
    bool empty = true;
    if(mcpes.find(fb)==mcpes.end()) mcpes[fb]=I3MCPESeriesMapPtr(new I3MCPESeriesMap);
    else empty = mcpes[fb]->empty();

    if(mcpes[fb]) frames.front()->Put("MCPESeriesMap", mcpes[fb]);
    mcpes.erase(mcpes.find(fb));

    if(!empty || keep_){
      log_debug("Pushing the frame");
      PushFrame(frames.front(),"OutBox");
    }
    frames.pop_front();
  }
}

void i3ppc::pushframe(){
  int fc=0;
  for(unsigned int i=0; i<xppc::hitz.size(); i++){
    xppc::ihit & h = xppc::hitz[i];
    OMKey key(h.omkey.str, h.omkey.dom);

    fc=h.track.frame;
    if(mcpes.find(fc)==mcpes.end()) mcpes[fc]=I3MCPESeriesMapPtr(new I3MCPESeriesMap);

    I3MCPE hit(h.track.second, h.track.first);
    hit.npe = 1;
    if(wid>0) h.time+=wid*xppc::xrnd();
    pz.addHit(key, *mcpes[fc], hit, h.time*I3Units::ns);
  }
  xppc::efin();
  popframes(nph>0?fe:fc);
}

void i3ppc::Finish(){
  if(ini){
    if(nph<=0){
      xppc::eout();
      pushframe();
      popframes(fe);
    }
    Flush();
    xppc::fin();
  }
}

bool i3ppc::isinside(double x, double y, double z){
  if(cyl){
    return sqrt(x*x+y*y)<=Radius+Padding && Bottom-Padding<=z && z<=Top+Padding;
  }
  else{
    for(vector<xppc::OM>::const_iterator i=xppc::i3oms.begin(); i!=xppc::i3oms.end(); ++i){
	double dx=i->r[0]-x;
	double dy=i->r[1]-y;
	double dz=i->r[2]-z;
	if(sqrt(dx*dx+dy*dy+dz*dz)<=Padding) return true;
      }
    return false;
  }
}

bool i3ppc::isinside(double x, double y, double z, double nx, double ny, double nz, double l){
  if(cyl){
    double R=Radius+Padding, B=Bottom-Padding, T=Top+Padding;
    double a=nx*nx+ny*ny, c=R*R-(x*x+y*y), lo=-1, hi=l+1;

    double h1, h2;
    if(nz>0) h1=(B-z)/nz, h2=(T-z)/nz;
    else if(nz<0) h1=(T-z)/nz, h2=(B-z)/nz;
    else{
      if(z<B || z>T) h1=hi, h2=lo;
      else h1=0, h2=l;
    }

    double r1, r2;
    if(a>0){
      double b=x*nx+y*ny;
      double D=b*b+a*c;
      if(D>=0){
	D=sqrt(D);
	r1=(-b-D)/a, r2=(-b+D)/a;
      }
      else r1=hi, r2=lo;
    }
    else{
      if(c<0) r1=hi, r2=lo;
      else r1=0, r2=l;
    }

    double t1=max(r1, h1), t2=min(r2, h2);
    if(t1<0) t1=0; if(t2>l) t2=l;
    return t2>=t1;
  }
  else{
    for(vector<xppc::OM>::const_iterator i=xppc::i3oms.begin(); i!=xppc::i3oms.end(); ++i){
	double dx=i->r[0]-x;
	double dy=i->r[1]-y;
	double dz=i->r[2]-z;
	double r=dx*nx+dy*ny+dz*nz;
	if(r<0) r=0; else if(r>l) r=l;
	dx-=r*dx, dy-=r*dy, dz-=r*dz;
	if(sqrt(dx*dx+dy*dy+dz*dz)<=Padding) return true;
      }
    return false;
  }
}

int i3ppc::iType(const I3Particle& p){
  if(p.GetLocationType()!=I3Particle::InIce) return -4;
  else if(p.GetShape()==I3Particle::Dark) return -3;
  else{
    switch(p.GetType()){
    default:
      return -2;
    case I3Particle::MuPlus:
    case I3Particle::MuMinus:
      return -1;
    case I3Particle::DeltaE:
    case I3Particle::Brems:
    case I3Particle::PairProd:
      return 1;
    case I3Particle::EMinus:
      return 2;
    case I3Particle::EPlus:
      return 3;
    case I3Particle::Gamma:
      return 4;
    case I3Particle::NuclInt:
    case I3Particle::Hadrons:
      return 101;
    case I3Particle::PiPlus:
      return 102;
    case I3Particle::PiMinus:
      return 103;
    case I3Particle::K0_Long:
      return 104;
    case I3Particle::PPlus:
      return 105;
    case I3Particle::Neutron:
      return 106;
    case I3Particle::PMinus:
      return 107;
    }
  }
}

void i3ppc::pparticle(I3MCTreeConstPtr tree, I3MCTree::const_iterator sub){
  const I3Particle& p = *sub;
  pair<int, unsigned long long> id=make_pair(p.GetMinorID(), p.GetMajorID());

  double E0=p.GetEnergy()/I3Units::GeV;
  double t0=p.GetTime()/I3Units::ns;
  double ll=p.GetLength()/I3Units::m;

  if(!isfinite(ll) || ll<0) ll=0;

  double x=p.GetX()/I3Units::m;
  double y=p.GetY()/I3Units::m;
  double z=p.GetZ()/I3Units::m;

  const I3Direction& n = p.GetDir();
  double nx=n.GetX(), ny=n.GetY(), nz=n.GetZ();
  xppc::sett(nx, ny, nz, id, fe);

  int ptype = iType(p);

  if(ptype>=0){
    if(isfinite(E0)) if(isinside(x, y, z)) xppc::addp(x, y, z, t0, E0, ptype);
  }
  else if(ptype==-1){
    const double sol=0.299792458;

    map<pair<int, unsigned long long>, const I3MMCTrack *>::const_iterator mmctrack=i3mmctracks.find(id);
    if(mmctrack==i3mmctracks.end()){
      if(isinside(x, y, z, nx, ny, nz, ll)) xppc::addp(x, y, z, t0, E0, (float) ll);
    }
    else{
      const I3MMCTrack * i3mmctrack = mmctrack->second;
      double ti=i3mmctrack->GetTi()/I3Units::ns;
      double Ei=i3mmctrack->GetEi()/I3Units::GeV;
      double tf=i3mmctrack->GetTf()/I3Units::ns;
      double Ef=i3mmctrack->GetEf()/I3Units::GeV;
      if(Ei==0){
	ti=t0;
	Ei=E0;
      }
      if(Ef<0){
	tf=t0+ll/sol;
	Ef=0;
      }
      if(Ei>0){
	double totE=0;
	for(I3MCTree::sibling_const_iterator it=tree->children(sub); it!=tree->end(sub); ++it){
	  double t=it->GetTime()/I3Units::ns;
	  if(t>=ti && t<=tf) totE+=it->GetEnergy()/I3Units::GeV;
	}
	double sl0=tf>ti?((Ef-Ei+totE)/(ti-tf)):0;
	double slmax=(0.21+8.8e-3*log(Ei)/log(10.))*sol;  // for ice only at Ecut=500 MeV
	double sl=min(sl0, slmax);

	double dr=(ti-p.GetTime()/I3Units::ns)*sol;
	x+=dr*nx, y+=dr*ny, z+=dr*nz;

	double t=ti;
	double l;
	double E=Ei;
	double new_t;

	for(I3MCTree::sibling_const_iterator it=tree->children(sub); it!=tree->end(sub); ++it){
	  new_t=it->GetTime()/I3Units::ns;
	  if(new_t>=ti && new_t<=tf){
	    l=sol*(new_t-t);

	    if(isinside(x, y, z, nx, ny, nz, l)) xppc::addp(x, y, z, t, E, (float) l);

	    x=it->GetX()/I3Units::m;
	    y=it->GetY()/I3Units::m;
	    z=it->GetZ()/I3Units::m;
	    double e=it->GetEnergy()/I3Units::GeV;

	    E-=e+sl*max(new_t-t, 0.);
	    t=new_t;
	  }
	}
	l=sol*(tf-t);
	if(isinside(x, y, z, nx, ny, nz, l)) xppc::addp(x, y, z, t, E, (float) l);
      }
    }
  }

  for(I3MCTree::sibling_const_iterator it=tree->children(sub); it!=tree->end(sub); ++it) pparticle(tree, it);
}
