/**
 * copyright (C) 2007
 * the IceCube collaboration
 * $Id:$
 *
 * @file I3DOMLaunchTriggerTypeFilter.cxx
 * @version $Revision$
 * @author Anna Franckowiak
 * @date Oct 22 2007
 */

#include <icepick/I3DOMLaunchTriggerTypeFilter.h>

#include <dataclasses/physics/I3DOMLaunch.h>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3IcePickInstaller.h>
#include <interfaces/I3IcePickModule.h>
#include <interfaces/I3IceForkModule.h>


using namespace std;


I3DOMLaunchTriggerTypeFilter::I3DOMLaunchTriggerTypeFilter(const I3Context& context) : 
    I3IcePick(context)
{  
  AddParameter("Names",
               "Names specifying DOM launch series maps to check for",
               ids_);
}


I3DOMLaunchTriggerTypeFilter::~I3DOMLaunchTriggerTypeFilter()
{
}


void I3DOMLaunchTriggerTypeFilter::Configure()
{ 
  GetParameter("Names",ids_);
}


bool I3DOMLaunchTriggerTypeFilter::SelectFrame(I3Frame& frame)
{
  bool retVal = true;

  for(unsigned int i = 0; retVal && (i < ids_.size()); ++i)
    {
      I3DOMLaunchSeriesMapConstPtr launches =
        frame.Get<I3DOMLaunchSeriesMapConstPtr>(ids_.at(i));
      if(!(retVal = !(launches && !launches->empty())))
        log_debug("I3DOMLauchSeriesMap \"%s\" is not empty.  Skip Event!!!",
                  ids_[i].c_str());
    }
  
  return retVal;
}



I3_MODULE(I3IcePickModule<I3DOMLaunchTriggerTypeFilter>);
I3_MODULE(I3IceForkModule<I3DOMLaunchTriggerTypeFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3DOMLaunchTriggerTypeFilter>);
