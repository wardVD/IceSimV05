#include <icepick/I3RecoPulseFilter.h>

#include <dataclasses/physics/I3RecoPulse.h>

I3RecoPulseFilter::I3RecoPulseFilter(const I3Context& context) :
  I3IcePick(context),
  frameKey_("InitialPulseSeriesReco"),
  minhitperchan_(0),
  maxhitperchan_(-1),
  minchargeperchan_(0),
  maxchargeperchan_(-1)
{
  AddParameter("FrameKey",
	       "Key in the frame to find the I3RecoPulseSeries",
	       frameKey_);
  
  AddParameter("MaxNHitPerNChannel",
	       "Maximum nhit per nchannel that an event can have\n"
	       "(Negative values for no filtering on this quantity)",
	       maxhitperchan_);
  
  AddParameter("MinNHitPerNChannel",
	       "Minimum nhit per nchannel that an event can have\n"
	       "(Negative values for no filtering on this quantity)",
	       minhitperchan_);

  AddParameter("MaxChargePerNChannel",
	       "Maximum charge per nchannel that an event can have\n"
	       "(Negative values for no filtering on this quantity)",
	       maxchargeperchan_);
  
  AddParameter("MinChargePerNChannel",
	       "Minimum charge per nchannel that an event can have\n"
	       "(Negative values for no filtering on this quantity)",
	       minchargeperchan_);

}

void I3RecoPulseFilter::Configure()
{
  GetParameter("FrameKey",frameKey_);
  GetParameter("MaxNHitPerNChannel",maxhitperchan_);
  GetParameter("MinNHitPerNChannel",minhitperchan_);
  GetParameter("MaxChargePerNChannel",maxchargeperchan_);
  GetParameter("MinChargePerNChannel",minchargeperchan_);
}

bool I3RecoPulseFilter::SelectFrame(I3Frame& frame)
{
  I3RecoPulseSeriesMapConstPtr rhsm = frame.Get<I3RecoPulseSeriesMapConstPtr>(frameKey_);
  if(!rhsm)
    {
      log_warn("Frame does not have I3RecoPulseSeriesMap at key %s. Failing event.",
	       frameKey_.c_str());
      return false;
    }

  float charge = 0.0;
  int nhit = 0;
  int nchan = 0;
  for(I3RecoPulseSeriesMap::const_iterator miter = rhsm->begin();
      miter != rhsm->end();
      ++miter)
    {
      I3RecoPulseSeries::const_iterator siter = miter->second.begin();
      if(siter == miter->second.end()) continue;
      ++nchan;
      for(;siter != miter->second.end();++siter)
	{
	  ++nhit;
	  charge += siter->GetCharge();
	}
    }
  float chargeperchan = charge / nchan;
  float hitperchan = nhit / (float) nchan;
  
  if(minhitperchan_ != -1)
    {
      if(hitperchan < minhitperchan_)
	{
	  log_debug("Nhit per nchan of %f is less than minimum of %f.  Filtering.",
		    hitperchan,minhitperchan_);
	  return false;
	}
    }

  if(maxhitperchan_ != -1)
    {
      if(hitperchan > maxhitperchan_)
	{
	  log_debug("Nhit per nchan of %f is greater than maximum of %f.  Filtering.",
		    hitperchan,maxhitperchan_);
	  return false;
	}
    }

  if(minchargeperchan_ != -1)
    {
      if(chargeperchan < minchargeperchan_)
	{
	  log_debug("Charge per nchan of %f is less than minimum of %f.  Filtering.",
		    chargeperchan,minchargeperchan_);
	  return false;
	}
    }

  if(maxchargeperchan_ != -1)
    {
      if(chargeperchan > maxchargeperchan_)
	{
	  log_debug("Charge per nchan of %f is greater than minimum of %f.  Filtering.",
		    chargeperchan,maxchargeperchan_);
	  return false;
	}
    }

  log_trace("Event passed all filtering.");
  return true;
}

#include <interfaces/I3IcePickModule.h>
I3_MODULE(I3IcePickModule<I3RecoPulseFilter>);
#include <interfaces/I3IceForkModule.h>
I3_MODULE(I3IceForkModule<I3RecoPulseFilter>);
#include <icetray/I3IcePickInstaller.h>
I3_SERVICE_FACTORY(I3IcePickInstaller<I3RecoPulseFilter>);
