#include "icepick/I3NLaunchFilter.h"
#include "interfaces/I3IcePickModule.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/geometry/I3Geometry.h"

I3NLaunchFilter::I3NLaunchFilter(const I3Context& context) : I3IcePick(context) ,
							     dataReadoutName_("InIceRawData"),
							     minNlaunch_(0),
							     maxNlaunch_(-1)
{
  AddParameter("DataReadoutName","",dataReadoutName_);
  AddParameter("MinNLaunch","",minNlaunch_);
  AddParameter("MaxNlaunch","",maxNlaunch_);
  countWhat_="doms";
  countWhatMap_["doms"]=COUNTDOMS;
  countWhatMap_["strings"]=COUNTSTRINGS;
  countWhatMap_["tanks"]=COUNTTANKS;
  countWhatMap_["stations"]=COUNTSTATIONS;
  AddParameter("CountWhat","count 'doms','tanks','stations' or 'strings'?",countWhat_);
  strings_.clear(); // default: empty, i.e.g all strings
  AddParameter("OnlyStrings","List of strings to use (empty=>all)",strings_);
}

void I3NLaunchFilter::Configure()
{
  GetParameter("DataReadoutName",dataReadoutName_);
  GetParameter("MinNLaunch",minNlaunch_);
  GetParameter("MaxNLaunch",maxNlaunch_);
  GetParameter("CountWhat",countWhat_);
  if (countWhatMap_.count(countWhat_)<=0)
    log_fatal("invalid CountWhat parameter '%s'",countWhat_.c_str());
  GetParameter("OnlyStrings",strings_);
}

bool I3NLaunchFilter::SelectFrame(I3Frame& frame)
{
  I3DOMLaunchSeriesMapConstPtr omr = frame.Get<I3DOMLaunchSeriesMapConstPtr>(dataReadoutName_);
  const I3OMGeoMap& geo = frame.Get<I3Geometry>().omgeo; // used to indentify IT/II doms

  if (!omr) {
    log_error("LaunchSeriesMap '%s' missing",dataReadoutName_.c_str());
    return false; // reject event
  }
  std::map <OMKey,int> domCount;
  std::map <int,int> stringCount;
  std::map <int,int> stationCount;
  std::map <int,int> tankCount;
  for(I3DOMLaunchSeriesMap::const_iterator iter = omr->begin(); iter != omr->end(); iter++) {
    if (strings_.size()>0) {
      if (find(strings_.begin(), strings_.end(), iter->first.GetString())
	  ==strings_.end()) continue;
    }
    if (iter->second.size()>0) {
      domCount[iter->first]++;
      if (geo.count(iter->first)>0)
	switch (geo.find(iter->first)->second.omtype) {
	case I3OMGeo::IceCube: stringCount[iter->first.GetString()]++; break;
	case I3OMGeo::IceTop:
	  stationCount[iter->first.GetString()]++;
	  // generate some tank id: e.g. 21/61..21/64 -> 2101,2101,2102,2102
	  tankCount[iter->first.GetString()*100+(iter->first.GetOM()-59)/2]++;
	  break;
	case I3OMGeo::Scintillator: break;
	case I3OMGeo::IceAct: break;
	case I3OMGeo::AMANDA: break;
	case I3OMGeo::UnknownType: break;
	}
      else {
	log_error("missing geo entry for DOM %d/%d",iter->first.GetString(),iter->first.GetOM());
      }
    } else
      log_warn("empty LaunchSeries in DOM %d/%d",iter->first.GetString(),
	       iter->first.GetOM());
  }
  int count=0;
  switch (countWhatMap_[countWhat_]) {
  case COUNTDOMS: count=domCount.size(); break;
  case COUNTTANKS: count=tankCount.size(); break;
  case COUNTSTATIONS: count=stationCount.size(); break;
  case COUNTSTRINGS: count=stringCount.size(); break;
  }
  log_debug("number of '%s' launches: %zu doms, %zu strings, %zu tanks, %zu stations",dataReadoutName_.c_str(),domCount.size(),stringCount.size(),tankCount.size(),stationCount.size());
  if (count<minNlaunch_) return false; // reject
  if (maxNlaunch_>=0 && count>maxNlaunch_) return false; // reject
  return true; // accept event
}
