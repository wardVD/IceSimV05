#ifndef I3TRIGGERSATISFIEDFILTER_H
#define I3TRIGGERSATISFIEDFILTER_H

#include "icetray/I3IcePick.h"
#include "dataclasses/physics/I3Trigger.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include "dataclasses/TriggerKey.h"
#include <sstream>

/**
 * @brief An IcePick that only selects events which 
 * have satisfied some named trigger in the I3TriggerHierarchy */

class I3TriggerSatisfiedFilter : public I3IcePick
{
  SET_LOGGER("I3TriggerSatisfiedFilterPick");
  std::vector<std::string> sourceNames_;
  std::vector<std::string> typeNames_;
  std::string hierarchyName_;
  std::vector<int> sourceIds_;
  std::vector<int> typeIds_;

  std::map<std::string, int> txt2sourceid; // maybe TriggerKey.h would be a better place
  std::map<std::string, int> txt2typeid;   // for these 2 maps

 public:
  I3TriggerSatisfiedFilter(const I3Context& context) : I3IcePick(context)
    {
      txt2sourceid["IN_ICE"]=TriggerKey::IN_ICE;
      txt2sourceid["ICE_TOP"]=TriggerKey::ICE_TOP;
      txt2sourceid["EXTERNAL"]=TriggerKey::EXTERNAL;
      txt2sourceid["GLOBAL"]=TriggerKey::GLOBAL;
      txt2sourceid["AMANDA_MUON_DAQ"]=TriggerKey::AMANDA_MUON_DAQ;
      txt2sourceid["AMANDA_TWR_DAQ"]=TriggerKey::AMANDA_TWR_DAQ;
      txt2sourceid["SPASE"]=TriggerKey::SPASE;
      txt2sourceid["UNKNOWN_SOURCE"]=TriggerKey::UNKNOWN_SOURCE;
      txt2sourceid["*"]=-1;
      txt2typeid["SIMPLE_MULTIPLICITY"]=TriggerKey::SIMPLE_MULTIPLICITY;
      txt2typeid["CALIBRATION"]=TriggerKey::CALIBRATION;
      txt2typeid["MIN_BIAS"]=TriggerKey::MIN_BIAS;
      txt2typeid["THROUGHPUT"]=TriggerKey::THROUGHPUT;
      txt2typeid["TWO_COINCIDENCE"]=TriggerKey::TWO_COINCIDENCE;
      txt2typeid["THREE_COINCIDENCE"]=TriggerKey::THREE_COINCIDENCE;
      txt2typeid["MERGED"]=TriggerKey::MERGED;
      txt2typeid["STRING"]=TriggerKey::STRING;
      txt2typeid["SPASE_2"]=TriggerKey::SPASE_2;
      txt2typeid["UNKNOWN_TYPE"]=TriggerKey::UNKNOWN_TYPE;
      txt2typeid["VOLUME"]=TriggerKey::VOLUME;
      txt2typeid["FRAGMENT_MULTIPLICITY"]=TriggerKey::FRAGMENT_MULTIPLICITY;
      txt2typeid["*"]=-1;

      sourceNames_.push_back("*");
      typeNames_.push_back("*");
      hierarchyName_="TriggerHierarchy";

      AddParameter("Source","trigger source",sourceNames_);
      AddParameter("Type","trigger type",typeNames_);
      AddParameter("TriggerHierarchyName","",hierarchyName_);
    }

  void Configure()
    {
      GetParameter("Source",sourceNames_);
      GetParameter("Type",typeNames_);
      GetParameter("TriggerHierarchyName",hierarchyName_);
      if (sourceNames_.size() != typeNames_.size()) {
	log_fatal("number of trigger sources must be equal to number of trigger types!");
      }
      for (unsigned int i=0; i<sourceNames_.size(); i++) {
	if (txt2sourceid.count(sourceNames_[i])==0) {
	  std::stringstream o;
	  for (std::map<std::string,int>::iterator iter=txt2sourceid.begin(); iter!=txt2sourceid.end(); iter++) {
	    o<<iter->first<<" ";
	  }
	  log_fatal("unknown trigger source %s, valid sources: %s",sourceNames_[i].c_str(),o.str().c_str());
	}
	if (txt2typeid.count(typeNames_[i])==0) {
	  std::stringstream o;
	  for (std::map<std::string,int>::iterator iter=txt2typeid.begin(); iter!=txt2typeid.end(); iter++) {
	    o<<iter->first<<" ";
	  }
	  log_fatal("unknown trigger type %s, valid types: %s",typeNames_[i].c_str(),o.str().c_str());
	}

	typeIds_.push_back(txt2typeid[typeNames_[i]]);
	sourceIds_.push_back(txt2sourceid[sourceNames_[i]]);
      }
    }

  bool SelectFrame(I3Frame& frame)
    {
      if (!frame.Has(hierarchyName_)) {
	log_fatal("no trigger hierarchy '%s' in the frame",hierarchyName_.c_str());
      }
      I3TriggerHierarchyConstPtr th=frame.Get<I3TriggerHierarchyConstPtr>(hierarchyName_);
      for (I3TriggerHierarchy::iterator triggerIter=th->begin();
	   triggerIter!=th->end(); triggerIter++) {
	const I3Trigger& tr=*triggerIter;
	if (!tr.GetTriggerFired()) continue;
	const TriggerKey& key=tr.GetTriggerKey();
	for (unsigned int i=0; i<sourceIds_.size(); i++) {
	  bool sourceOk=(sourceIds_[i]==-1 || sourceIds_[i]==key.GetSource());
	  bool typeOk=(typeIds_[i]==-1 || typeIds_[i]==key.GetType());
	  if (sourceOk && typeOk) {
	    log_debug("trigger %s/%s present",
		      sourceNames_[i].c_str(),typeNames_[i].c_str());
	    return true;
	  }
	}
      }
      std::stringstream o;
      for (unsigned int i=0; i<sourceIds_.size(); i++)
	o<<sourceNames_[i]<<"/"<<typeNames_[i]<<" ";
      log_debug("none of the following triggers present: %s",
		o.str().c_str());
      return false;
    }
	 
};

#endif
