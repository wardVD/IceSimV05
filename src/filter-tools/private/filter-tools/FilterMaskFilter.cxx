#include <filter-tools/FilterMaskFilter.h>

#include <boost/foreach.hpp>

#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3IcePickInstaller.h>
#include <interfaces/I3IcePickModule.h>
#include <interfaces/I3IceForkModule.h>
#include <dataclasses/physics/I3FilterResult.h>

FilterMaskFilter::FilterMaskFilter(const I3Context& context) : 
  I3IcePick(context),
  filter_result_name_("FilterMask")
{
  AddParameter("FilterNameList",
               "Pass Frames where at least one of these filters is true",
               filter_name_list_); 
  AddParameter("FilterResultName",
               "Name of I3FilterResultMap object in frame",
               filter_result_name_) ;
}

void FilterMaskFilter::Configure()
{
  GetParameter("FilterNameList",filter_name_list_);
  GetParameter("FilterResultName",filter_result_name_);
}

bool FilterMaskFilter::SelectFrame(I3Frame &frame)
{
  //get result map form the frame
  I3FilterResultMapConstPtr filter_result_map =
    frame.Get<I3FilterResultMapConstPtr>(filter_result_name_);
  
  if (filter_result_map)
    {
      //loop over all the names in filter_name_list_
      BOOST_FOREACH(std::string filter_name, filter_name_list_)
        {
          //get result form map
          I3FilterResultMap::const_iterator iter =
            filter_result_map->find(filter_name);
          
          if(iter != filter_result_map->end())
            {
              //if condition passed
              if (iter->second.conditionPassed && iter->second.prescalePassed)
                {
                  //keep event
                  return true;
                }
            }
          else
            {
              log_warn("Filterresult \"%s\" NOT AVAILABLE in current filtermask.",
                       filter_name.c_str());
            }
        }
    }
  else
    {
      log_info("Filtermask \"%s\" NOT AVAILABLE in current frame.  Skip Event!!!",
               filter_result_name_.c_str());
    }
      
  //if none of them passed reject event
  return false;
}

I3_MODULE(I3IcePickModule<FilterMaskFilter>);
I3_MODULE(I3IceForkModule<FilterMaskFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<FilterMaskFilter>);
