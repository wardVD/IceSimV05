/**
 * copyright  (C) 2006
 * the icecube collaboration
 * $Id:
 *
 * @file SimpleMajorityTrigger.cxx
 * @version
 * @date
 * @author toale
 */

#include <trigger-sim/modules/SimpleMajorityTrigger.h>
#include <trigger-sim/algorithms/SimpleMajorityTriggerAlgorithm.h>
#include <icetray/OMKey.h>
#include <dataclasses/I3Vector.h>
#include <icetray/I3Units.h>
#include <icetray/I3Bool.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <boost/foreach.hpp>
#include <boost/assign/std/vector.hpp>

#include "trigger-sim/utilities/DOMSetFunctions.h"
#include "trigger-sim/utilities/DetectorStatusUtils.h"

using namespace boost::assign;

using DetectorStatusUtils::tk_ts_pair_t;
using DetectorStatusUtils::_sourceID;
using DetectorStatusUtils::_typeID;
using DetectorStatusUtils::_configID;
using DetectorStatusUtils::GetTriggerStatus;

I3_MODULE(SimpleMajorityTrigger);

const TriggerKey::TypeID TYPEID(TriggerKey::SIMPLE_MULTIPLICITY);

SimpleMajorityTrigger::SimpleMajorityTrigger(const I3Context& context) : 
  I3Module(context),
  dataReadoutName_(""),
  triggerName_("I3Triggers"),
  domSetsName_("DOMSets"),
  triggerSourceParam_(INT_MIN),
  triggerSource_(TriggerKey::UNKNOWN_SOURCE),
  configIDParam_(INT_MIN),
  triggerWindow_(NAN),
  triggerThreshold_(INT_MIN),
  domSet_(boost::optional<int>()),
  eventCount_(0),
  triggerCount_(0)
{

  AddParameter("DataReadoutName",
	       "This holds the DOM launches",
	       dataReadoutName_);
  
  AddParameter("TriggerName",
	       "Name of the I3TriggerHierarchy to store the triggers in.",
	       triggerName_);
  
  AddParameter("TriggerSource",
	       "Source of the trigger (IN_ICE = 0)",
	       triggerSourceParam_);
  
  AddParameter("TriggerConfigID",
	       "Config ID of the trigger.",
	       configIDParam_);
  
  AddParameter("DOMSetsName",
	       "Name of the I3MapKeyVectorInt defining the DomSets for each DOM.",
	       domSetsName_);

  AddOutBox("OutBox");
}

SimpleMajorityTrigger::~SimpleMajorityTrigger() {}

void SimpleMajorityTrigger::Configure()
{  
  GetParameter("DataReadoutName", dataReadoutName_);
  GetParameter("TriggerName",triggerName_);
  GetParameter("TriggerSource",triggerSourceParam_);
  GetParameter("TriggerConfigID",configIDParam_);
  GetParameter("DOMSetsName", domSetsName_);

  if(triggerSourceParam_ != INT_MIN)
    triggerSource_ = static_cast<TriggerKey::SourceID>(triggerSourceParam_);

  if(configIDParam_ != INT_MIN)
    configID_ = configIDParam_;

  if(!triggerName_.size()) triggerName_ = "I3Triggers";
}

void SimpleMajorityTrigger::DAQ(I3FramePtr frame)
{
  // Get the DOMSets from the frame
  I3MapKeyVectorIntConstPtr domSets = 
    frame->Get<I3MapKeyVectorIntConstPtr>(domSetsName_);
    
  log_debug("  TriggerWindow = %f", triggerWindow_);
  log_debug("  TriggerThreshold = %d", triggerThreshold_);
  // Create the trigger object
  SimpleMajorityTriggerAlgorithm smTrigger(triggerWindow_, triggerThreshold_);

  log_debug("Checking for Triggers...");
  eventCount_++;

  SimpleMajorityTriggerList tlist;

  /*------------------------------------------------------------*
   * Get DomLaunchSeriesMap from frame
   *------------------------------------------------------------*/
  if(dataReadoutName_.size() == 0){
    if(triggerKey_.GetSource() == TriggerKey::IN_ICE){
      dataReadoutName_ = "InIceRawData";
    }else{
      if(triggerKey_.GetSource() == TriggerKey::ICE_TOP){
        dataReadoutName_ = "IceTopRawData";
      }else{
        std::stringstream s;
        s<<triggerKey_;
        log_error("Couldn't determine the trigger source from this trigger : ");
        log_error("%s",s.str().c_str());
        log_fatal("You'll never find a nameless map with launches in it.");
      }
    }
  }

  I3DOMLaunchSeriesMapConstPtr fullMap = 
    frame->Get<I3DOMLaunchSeriesMapConstPtr>(dataReadoutName_);

  if (fullMap == 0) {
    log_debug("Frame does not contain an I3DOMLaunchSeriesMap named %s", 
	      dataReadoutName_.c_str());

  } else {

    // count the hits
    int count = 0;
    I3DOMLaunchSeriesMap::const_iterator mapIter;
    for (mapIter = fullMap->begin(); mapIter != fullMap->end(); mapIter++) {
      I3DOMLaunchSeries::const_iterator seriesIter;
      for (seriesIter = mapIter->second.begin(); 
	   seriesIter != mapIter->second.end(); 
	   seriesIter++) count++;
    }
    log_debug("Got %d hits to split", count);

    /*------------------------------------------------------------*
     * Fill the hits
     *------------------------------------------------------------*/
    TriggerHitVectorPtr hitVector(new TriggerHitVector);
    FillHits(fullMap, hitVector, domSets);

    /*------------------------------------------------------------*
     * Time order the hits
     *------------------------------------------------------------*/
    std::sort(hitVector->begin(), hitVector->end());
    Dump(hitVector);

    /*------------------------------------------------------------*
     * Check Trigger condition on this string
     *------------------------------------------------------------*/
    smTrigger.AddHits(hitVector);

    unsigned int numTriggers = smTrigger.GetNumberOfTriggers();
    for (unsigned int n = 0; n < numTriggers; n++) {

      TriggerHitVectorPtr timeHits = smTrigger.GetNextTrigger();

      // We have a trigger!
      triggerCount_++;

      // The trigger times are defined by the hits in the time window
      double startTime(timeHits->front().time);
      double stopTime(timeHits->back().time);

      I3Trigger tr;
      tr.GetTriggerKey() = triggerKey_;
      tr.SetTriggerFired(true);
      tr.SetTriggerTime(startTime);
      tr.SetTriggerLength(stopTime - startTime);

      tlist.push_back(tr);

    } // end loop over time windows

  } // endif
  
  // Check to see if a trigger hierarchy already exists
  I3TriggerHierarchyPtr triggers;
  if(frame->Has(triggerName_)){
    const I3TriggerHierarchy& old_th = frame->Get<I3TriggerHierarchy>(triggerName_);
    triggers = I3TriggerHierarchyPtr(new I3TriggerHierarchy(old_th));
    frame->Delete(triggerName_);
  }else{
    triggers = I3TriggerHierarchyPtr(new I3TriggerHierarchy);
  }

  // Fill the trigger hierarchy
  BOOST_FOREACH(I3Trigger& t, tlist)
    triggers->insert(triggers->begin(), t);

  frame->Put(triggerName_,triggers);

  PushFrame( frame );
}

void SimpleMajorityTrigger::Finish()
{
  std::stringstream sstr;
  sstr << "Found "<<triggerCount_<<" triggers out of "<<eventCount_<<" events";
  if(configID_) sstr<<" for config id "<<configID_.get()<<".";
  else sstr<<".";
  log_info("%s",sstr.str().c_str());
}

void SimpleMajorityTrigger::FillHits(I3DOMLaunchSeriesMapConstPtr fullMap, 
				     TriggerHitVectorPtr hitList, 
				     I3MapKeyVectorIntConstPtr domSets)
{

  I3DOMLaunchSeriesMap::const_iterator fullMapIter;
  for (fullMapIter = fullMap->begin(); fullMapIter != fullMap->end(); fullMapIter++) {
    const OMKey& omKey = fullMapIter->first;
    const I3DOMLaunchSeries& fullSeries = fullMapIter->second;

    I3DOMLaunchSeries::const_iterator fullSeriesIter;
    for (fullSeriesIter = fullSeries.begin(); 
	 fullSeriesIter != fullSeries.end(); fullSeriesIter++) {
      if( fullSeriesIter->GetLCBit() ){ 
	// TF : should there also be the "|| threshold ==1" like in I3SMTrigger ?	
	if( ( domSet_ && DOMSetFunctions::InDOMSet(omKey,domSet_.get(),domSets) ) 
	    || !domSet_ ){
	  
	  TriggerHitPtr hit(new TriggerHit);
	  hit->pos = omKey.GetOM();
	  hit->time = fullSeriesIter->GetStartTime();
	  hitList->push_back(*hit);
	}
      }
    }
  }
}

void SimpleMajorityTrigger::Dump(TriggerHitVectorPtr input)
{
  log_debug("Dumping Event with %zd hits...", input->size());

  TriggerHitVector::const_iterator iter;
  for (iter = input->begin(); iter != input->end(); iter++) {
    const TriggerHit& hit = *iter;
    log_debug("  Time %f   Position %u", hit.time, hit.pos);
  }
}

void SimpleMajorityTrigger::DetectorStatus(I3FramePtr frame){
  // Get DetectorStatus from the frame
  I3DetectorStatusConstPtr detStatus = frame->Get<I3DetectorStatusConstPtr>();
  if(!detStatus) log_fatal("This DetectorStatus frame has no I3DetectorStatus object.");

  // set the default triggerSource_ to UNKNOWN and
  // configID_ to "unset".   need to change the type 
  // boost::optional<int>.
  boost::optional<tk_ts_pair_t> tkts = GetTriggerStatus
    (detStatus, _sourceID = triggerSource_, _typeID = TYPEID, _configID = configID_ );

  if(!tkts) log_fatal("Failed to configure this module from the DetectorStatus.");

  // this needs to be passed on to the I3Trigger
  triggerKey_ = tkts.get().first;

  // now set the parameters
  tkts.get().second.GetTriggerConfigValue("threshold", triggerThreshold_);
  tkts.get().second.GetTriggerConfigValue("timeWindow", triggerWindow_);
  tkts.get().second.GetTriggerConfigValue("domSet", domSet_);

  log_debug("  TriggerWindow = %f", triggerWindow_);
  log_debug("  TriggerThreshold = %d", triggerThreshold_);

  PushFrame( frame );
}



