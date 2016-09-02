#include"NuGenTestModule.h"

#include<I3Test.h>
#include<icetray/I3Frame.h>
#include<dataclasses/physics/I3Particle.h>
#include<dataclasses/physics/I3MCTree.h>
#include<dataclasses/physics/I3MCTreeUtils.h>

using namespace std;

I3_MODULE(NuGenTestModule);

NuGenTestModule::NuGenTestModule(const I3Context& context) : 
  I3Module(context),
  nInIceParticles_(0)
{
  AddOutBox("OutBox");
  AddParameter("NeutrinoFlavor","Neutrino Flavor",flavorString_);
}

void
NuGenTestModule::Configure()
{
  GetParameter("NeutrinoFlavor",flavorString_);
}

void
NuGenTestModule::DAQ(I3FramePtr frame)
{

  I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>();
  ENSURE((bool)mctree);
  ENSURE(mctree->size() != 0);

  cerr<<"*************************************"<<endl;

  /**
   * Test that there are no duplicates in the tree
   */
  I3MCTree::const_iterator slow_iter;
  for(slow_iter = mctree->begin(); slow_iter != mctree->end(); slow_iter++){
    if(slow_iter->GetLocationType() == I3Particle::InIce){
      nInIceParticles_++;
      vector<I3Particle> children(I3MCTreeUtils::GetDaughters(*mctree,*slow_iter));
      vector<I3Particle>::iterator ci;
      //if the parent is InIce the children should be too
      for(ci = children.begin(); ci != children.end(); ci++)
	ENSURE(ci->GetLocationType() == I3Particle::InIce);
    }

    I3MCTree::const_iterator fast_iter(slow_iter);
    for(++fast_iter; fast_iter != mctree->end(); fast_iter++){
      if(slow_iter->GetMajorID() == fast_iter->GetMajorID() &&
	 slow_iter->GetMinorID() == fast_iter->GetMinorID()){
	cerr<<"----------------------------"<<endl;
	cerr<<" SLOW ITER"<<endl;
	cerr<<"type = "<<slow_iter->GetTypeString()<<endl
	    <<"  major id = "<<slow_iter->GetMajorID()<<endl
	    <<"  id = "<<slow_iter->GetMinorID()<<endl
	    <<"  energy = "<<slow_iter->GetEnergy()<<endl
	    <<"  x = "<<slow_iter->GetX()<<endl
	    <<"  y = "<<slow_iter->GetY()<<endl
	    <<"  z = "<<slow_iter->GetZ()<<endl
	    <<"  zenith = "<<slow_iter->GetZenith()<<endl
	    <<"  azimuth = "<<slow_iter->GetAzimuth()<<endl
	    <<endl;
	cerr<<"----------------------------"<<endl;
	cerr<<" FAST ITER"<<endl;
	cerr<<"type = "<<fast_iter->GetTypeString()<<endl
	    <<"  major id = "<<fast_iter->GetMajorID()<<endl
	    <<"  id = "<<fast_iter->GetMinorID()<<endl
	    <<"  energy = "<<fast_iter->GetEnergy()<<endl
	    <<"  x = "<<fast_iter->GetX()<<endl
	    <<"  y = "<<fast_iter->GetY()<<endl
	    <<"  z = "<<fast_iter->GetZ()<<endl
	    <<"  zenith = "<<fast_iter->GetZenith()<<endl
	    <<"  azimuth = "<<fast_iter->GetAzimuth()<<endl
	    <<endl;

      }
      ENSURE(slow_iter->GetMajorID() == fast_iter->GetMajorID());
      ENSURE(slow_iter->GetMinorID() != fast_iter->GetMinorID());
    }
  }

  vector<I3Particle> primaries = I3MCTreeUtils::GetPrimaries(*mctree);
  cerr<<"primaries.size() = "<<primaries.size()<<endl;
  for(vector<I3Particle>::iterator i = primaries.begin();
      i != primaries.end(); i++){
    cerr<<"type = "<<i->GetTypeString()<<endl
	<<"  major id = "<<i->GetMajorID()<<endl
	<<"  id = "<<i->GetMinorID()<<endl
	<<"  energy = "<<i->GetEnergy()<<endl
	<<"  x = "<<i->GetX()<<endl
	<<"  y = "<<i->GetY()<<endl
	<<"  z = "<<i->GetZ()<<endl
	<<"  zenith = "<<i->GetZenith()<<endl
	<<"  azimuth = "<<i->GetAzimuth()<<endl
	<<endl;
  }
  ENSURE(primaries.size()==1);
  /**
   * Test that the neutrino is in the right place and
   * is the right flavor
   */
  if(flavorString_ == "NuMu"){
    ENSURE(primaries[0].GetType() == I3Particle::NuMu || 
	   primaries[0].GetType() == I3Particle::NuMuBar);
  }
  else if(flavorString_ == "NuE"){
    ENSURE(primaries[0].GetType() == I3Particle::NuE || 
	   primaries[0].GetType() == I3Particle::NuEBar);
  }
  else if(flavorString_ == "NuTau"){
    ENSURE(primaries[0].GetType() == I3Particle::NuTau || 
	   primaries[0].GetType() == I3Particle::NuTauBar);
  }
  else{
    log_fatal("Unknown flavor!!");
  }

  PushFrame(frame,"OutBox");
}

void
NuGenTestModule::Finish()
{
  ENSURE(nInIceParticles_ > 0);
}
