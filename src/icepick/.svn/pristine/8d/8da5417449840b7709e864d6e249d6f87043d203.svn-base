/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3RandomFilter.cxx
 * @version $Revision$
 * @date $Date$
 */
 
// class header file
#include <icepick/I3RandomFilter.h>

#include <cmath>
#include <stdlib.h>

#include <icetray/I3Context.h>
#include <icetray/I3DefaultName.h>
#include <icetray/I3Frame.h>

// namespace declarations

using namespace std;

// implementation

I3RandomFilter::I3RandomFilter(const I3Context& context) 
  : I3IcePick(context),
    prescale_(1u),
    seed_(1u),
    useRandomService_(false)
{
  AddParameter("Prescale",
               "Determines the fraction of selected events",
               prescale_);
  AddParameter("Seed",
               "Seed for a local random function used for prescaling",
               seed_);
  AddParameter("UseRandomService",
               "Use random service for prescaling "
               "instead of a local random function",
               useRandomService_);
}


I3RandomFilter::~I3RandomFilter()
{
}


void I3RandomFilter::Configure()
{
  GetParameter("Prescale", prescale_);
  GetParameter("Seed", seed_);
  GetParameter("UseRandomService", useRandomService_);
  
  if(!useRandomService_ && (prescale_ > RAND_MAX))
    log_fatal("invalid prescale - %u > RAND_MAX", prescale_);
  else if(useRandomService_
          && !(randomService_ = GetContext().Get<I3RandomServicePtr>()))
    log_fatal("context contains nothing at slot \"%s\"",
              I3DefaultName<I3RandomServicePtr>::value());
}


bool I3RandomFilter::SelectFrame(I3Frame& frame)
{
  bool retVal;
  
  if(!prescale_) retVal = false;
  else if(prescale_ == 1) retVal = true;
  else if(!useRandomService_)
    retVal = !(static_cast<unsigned int>(rand_r(&seed_)) % prescale_);
  else retVal = !(randomService_->Integer(prescale_));

  return(retVal);
}



#include <interfaces/I3IcePickModule.h>
#include <interfaces/I3IceForkModule.h>
#include <icetray/I3IcePickInstaller.h>

I3_MODULE(I3IcePickModule<I3RandomFilter>);
I3_MODULE(I3IceForkModule<I3RandomFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3RandomFilter>);
