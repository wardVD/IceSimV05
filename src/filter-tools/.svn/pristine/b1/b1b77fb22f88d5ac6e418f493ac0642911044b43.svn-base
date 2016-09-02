#include <filter-tools/FilterCheckModule.h>
#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3FilterResult.h>
#include <icetray/I3Bool.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <boost/foreach.hpp>

/*
 *  A stupid FilterMask counter tool.  Not used generally in core processing, but in testing
 *       ** No coverage testing **
 *
 */


FilterCheckModule::FilterCheckModule(const I3Context& context) : 
  I3Module(context),
  filter_result_name_("FilterMask")
{
  AddOutBox("OutBox");
  AddParameter("FilterResultName","Name of I3FilterResultMap object in frame",filter_result_name_);
}

void FilterCheckModule::Configure()
{
  GetParameter("FilterResultName",filter_result_name_);
}

void FilterCheckModule::Physics(I3FramePtr frame)
{
  //get filter result from frame
  const I3FilterResultMap& filter_result_map=frame->Get<I3FilterResultMap>(filter_result_name_);
  
  //for each filter in filter result
  BOOST_FOREACH(I3FilterResultMap::const_reference filter_result,filter_result_map)
    {
      //if there is an object with the filteres name in the frame
      if (frame->Has(filter_result.first))
	{
	  bool south_filter=filter_result.second.conditionPassed;
	  bool north_filter=frame->Get<I3Bool>(filter_result.first).value;
	  
	  //if there isn't an entry for this filter yet
	  if (filter_matches_.find(filter_result.first)==filter_matches_.end())
	    {
	      //initilize to zero
	      filter_matches_[filter_result.first]=std::pair<int,int>(0,0);
	    }
	  if (north_filter==south_filter)
	    {
	      log_info("filters match for %s",filter_result.first.c_str());
	      //increment the match count
	      filter_matches_[filter_result.first].first+=1;
	    }
	  else
	    {
	      log_warn("filters don't match for %s",filter_result.first.c_str());
	    }
	  //increment the comparisons count
	  filter_matches_[filter_result.first].second+=1;
	}
    }
  PushFrame(frame,"OutBox");  
}

void FilterCheckModule::Finish()
{
  BOOST_FOREACH(FilterStatisticsMap::const_reference filter_result,filter_matches_)
    {
      printf("%30s matched %6d/%6d  %3.2f%%\n",
	     filter_result.first.c_str(),
	     filter_result.second.first,
	     filter_result.second.second,
	     100.*filter_result.second.first/filter_result.second.second);
    }
}

I3_MODULE(FilterCheckModule);
