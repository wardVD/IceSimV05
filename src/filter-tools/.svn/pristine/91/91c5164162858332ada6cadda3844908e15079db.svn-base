#include <icetray/I3Bool.h>
#include <filter-tools/FilterMask2Bools.h>
#include <dataclasses/physics/I3FilterResult.h>

#include <boost/foreach.hpp>

using namespace std;
I3_MODULE(FilterMask2Bools);

/* ******************************************************************** */
/* Constructor                                                          */
/* ******************************************************************** */
FilterMask2Bools::FilterMask2Bools(const I3Context& ctx) : 
  I3Module(ctx),
  filter_result_name_("FilterMask"),
  ignore_prescale_(false)
{
  log_debug("Enter FilterMask2Bools::FilterMask2Bools().");

  AddOutBox("OutBox");
  AddParameter("FilterResultName",
               "Name of I3FilterResultMap object in frame",
               filter_result_name_);
  AddParameter("IgnorePrescale",
               "Whether or not to ignore filter result prescale booleans",
               ignore_prescale_);
}

/* ******************************************************************** */
/* Destructor                                                           */
/* ******************************************************************** */
FilterMask2Bools::~FilterMask2Bools() 
{
}

/* ******************************************************************** */
/* Configure                                                            */
/* ******************************************************************** */

void FilterMask2Bools::Configure() 
{
  log_debug("Entering FilterMask2Bools Configure().");

  GetParameter("FilterResultName",filter_result_name_);
  GetParameter("IgnorePrescale",ignore_prescale_);
}

/* ******************************************************************** */
/* Physics                                                              */
/* ******************************************************************** */

void FilterMask2Bools::Physics(I3FramePtr frame) 
{
  log_debug("Entering FilterMask2Bools Physics().");

  // Get result map from the frame
  I3FilterResultMapConstPtr filter_result_map =
    frame->Get<I3FilterResultMapConstPtr>(filter_result_name_);

  if (filter_result_map)
  {
    //get result form map
    BOOST_FOREACH(I3FilterResultMap::const_reference filter_result,*filter_result_map)
    {
      string name = filter_result.first;

      I3BoolPtr result(new I3Bool);
      result->value = 
        filter_result.second.conditionPassed
        && (ignore_prescale_ || filter_result.second.prescalePassed);
      
      frame->Put(name,result);
    }
  }

  // Don't forget to push the frame back to the outbox!!!
  PushFrame(frame,"OutBox"); 
}
