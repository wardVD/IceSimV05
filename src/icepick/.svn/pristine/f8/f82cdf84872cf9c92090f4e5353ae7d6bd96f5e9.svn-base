#include "icetray/serialization.h"
#include "icepick/I3NHitChannelFilter.h"
#include "interfaces/I3IcePickModule.h"
#include "icetray/OMKey.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include <vector>
#include <map>

I3NHitChannelFilter::I3NHitChannelFilter(const I3Context& context) : 
  I3IcePick(context) ,
  responseKey_("response"),
  minThreshold_(0),
  maxThreshold_(INT_MAX)

{
  AddParameter("ResponseKey","Name of the response in the frame",responseKey_);
  AddParameter("MinThreshold",
	       "The Minimum number of hit channels required to allow the event",
	       minThreshold_);
  AddParameter("MaxThreshold",
	       "The Maximum number of hit channels required to allow the event",
	       maxThreshold_);
}

void I3NHitChannelFilter::Configure()
{
  GetParameter("ResponseKey",responseKey_);
  GetParameter("MinThreshold",minThreshold_);
  GetParameter("MaxThreshold",maxThreshold_);
}

bool I3NHitChannelFilter::SelectFrame(I3Frame& frame)
{
  if(frame.Has(responseKey_))
    {
	  I3RecoPulseSeriesMapConstPtr inicePulses
	    = frame.Get<I3RecoPulseSeriesMapConstPtr>(responseKey_);
	  if( (int) inicePulses->size() < minThreshold_ )
	    {
	      log_trace("Event has %zu hits and doesn't pass "
			"the minimum threshold of %d",
			inicePulses->size(),minThreshold_);
	      return false;
	    }
          if ( (int) inicePulses->size() > maxThreshold_ ) 
	    {
	      log_trace("Event has %zu hits and doesn't pass "
			"the maximum threshold of %d",
			inicePulses->size(),maxThreshold_);
	      return false;
	    }

	  return true;
    }
  else
    {
      log_trace("Event does not have responseKey_ %s.  Event being filtered.",
	       responseKey_.c_str());
      return false;
    }
}
