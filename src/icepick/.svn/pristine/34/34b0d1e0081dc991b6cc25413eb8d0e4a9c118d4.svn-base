#include <icepick/I3ZenithFilter.h>

#include <dataclasses/I3Direction.h>
#include <dataclasses/physics/I3Particle.h>
#include <icetray/I3Units.h>

#include <interfaces/I3IcePickModule.h>
#include <interfaces/I3IceForkModule.h>
#include <icetray/I3IcePickInstaller.h>

I3ZenithFilter::I3ZenithFilter(const I3Context& context) : I3IcePick(context),
    particleKey_("Particle"),
    minZenith_(0),
    maxZenith_(180*I3Units::degree)
   {
      AddParameter("ParticleKey",
		   "Name of the particle in the frame",
		   particleKey_);
      AddParameter("MinZenith",
		   "The zenith angle that below which events are cut",
		   minZenith_);
      AddParameter("MaxZenith",
		   "The zenith angle that above which events are cut",
		   maxZenith_);
    }

void I3ZenithFilter::Configure() {
    GetParameter("ParticleKey",particleKey_);
    GetParameter("MinZenith",minZenith_);   
    GetParameter("MaxZenith",maxZenith_);
  }

bool I3ZenithFilter::SelectFrame(I3Frame& frame) {
    if(frame.Has(particleKey_))
      {
	const I3Particle& particle = frame.Get<I3Particle>(particleKey_);
        if ( I3Particle::OK != particle.GetFitStatus() ){
          return false; // no successful fit -> reject event
        }
	const I3Direction& dir = particle.GetDir();
	if( dir.GetZenith() < minZenith_ )
	  {
	    log_trace("Particle %s has a zenith of %f and doesn't pass "
		      "the minimum threshold of %f",
		      particleKey_.c_str(),dir.GetZenith(),minZenith_);
	    return false;
	  }
	if( dir.GetZenith() > maxZenith_ )
	  {
	    log_trace("Particle %s has a zenith of %f and doesn't pass "
		      "the maximum threshold of %f",
		      particleKey_.c_str(),dir.GetZenith(),maxZenith_);
	    return false;
	  }
	return true;
      }
    else
      {
	log_trace("Event does not have particleKey_ %s.  Event being filtered.",
		  particleKey_.c_str());
	return false;
      }
  }

I3_MODULE(I3IcePickModule<I3ZenithFilter>);
I3_MODULE(I3IceForkModule<I3ZenithFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3ZenithFilter>);

