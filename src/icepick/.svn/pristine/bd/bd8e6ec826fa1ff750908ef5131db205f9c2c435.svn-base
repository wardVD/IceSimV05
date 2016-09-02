/**
 * copyright (C) 2007
 * the IceCube collaboration
 * $Id:$
 *
 * @file I3NChFilter.cxx
 * @version $Revision$
 * @author Anna Franckowiak
 * @date Oct 22 2007
 */

#include <icepick/I3NChFilter.h>

#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3IcePickInstaller.h>
#include <interfaces/I3IcePickModule.h>
#include <interfaces/I3IceForkModule.h>
#include <phys-services/I3CutValues.h>

using namespace std;


I3NChFilter::I3NChFilter(const I3Context& context) : 
    I3IcePick(context),
    cutsKey_(""),
    nch_(200)
{
  AddParameter("CutValuesKey","The name of the I3CutValues "
	       "object in the frame", cutsKey_);
  AddParameter("NCh","The number of channels that an event "
	       "must have to be kept", nch_);
}


I3NChFilter::~I3NChFilter()
{
}


void I3NChFilter::Configure()
{
  GetParameter("CutValuesKey",cutsKey_);
  GetParameter("NCh",nch_);
}


bool I3NChFilter::SelectFrame(I3Frame& frame)
{
  I3CutValuesConstPtr cuts = frame.Get<I3CutValuesConstPtr>(cutsKey_);
  if(cuts)
  {
    if(cuts->Nchan >= nch_)
    {
      log_debug("Event has %i NChannels and passes the threshold"
                " of %i NChannels.",cuts->Nchan, nch_);
      return true;
    }
    
    log_debug("Event has %i NChannels hit, and does not pass the threshold"
              " of %i NChannels.  Skip Event!!!",cuts->Nchan, nch_);
    return false;
  }
  
  log_debug("Event has no I3CutValues \"%s\".  Skip Event!!!",
            cutsKey_.c_str());
  return false;
}


I3_MODULE(I3IcePickModule<I3NChFilter>);
I3_MODULE(I3IceForkModule<I3NChFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3NChFilter>);
