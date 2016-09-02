/**
 * copyright  (C) 2006
 * the icecube collaboration
 * $Id:
 *
 * @file ClusterTrigger.cxx
 * @version
 * @date
 * @author toale
 */

#include <trigger-sim/modules/ClusterTrigger.h>
#include <trigger-sim/algorithms/ClusterTriggerAlgorithm.h>
#include <icetray/OMKey.h>
#include <dataclasses/I3Vector.h>
#include <dataclasses/I3Map.h>
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

I3_MODULE(ClusterTrigger);
const TriggerKey::SourceID SOURCEID(TriggerKey::IN_ICE);
const TriggerKey::TypeID TYPEID(TriggerKey::STRING);

/**
 * Lowest and highest string to consider for default string trigger.
 * This is the ClusterTrigger in the detector status
 *
 * const int MIN_CLUSTER_STRING(1);
 * const int MAX_CLUSTER_STRING(86);
 */

ClusterTrigger::ClusterTrigger(const I3Context& context) : 
  I3Module(context),
  dataReadoutName_("InIceRawData"),
  triggerName_("I3Triggers"),
  domSetsName_("DOMSets"),
  configIDParam_(INT_MIN),
  triggerWindow_(NAN),
  triggerThreshold_(INT_MAX),
  coherenceLength_(INT_MAX),
  eventCount_(0),
  triggerCount_(0)
{

  AddParameter("DataReadoutName",
	       "This holds the DOM launches",
	       dataReadoutName_);
  
  AddParameter("TriggerName",
	       "Name of the I3TriggerHierarchy to store the triggers in.",
	       triggerName_);
  
  AddParameter("TriggerConfigID",
	       "Config ID of the trigger.",
	       configIDParam_);

  AddParameter("DOMSetsName",
           "Name of the I3MapKeyVectorInt defining the DomSets for each DOM.",
           domSetsName_);

  AddOutBox("OutBox");
}

ClusterTrigger::~ClusterTrigger() {}

void ClusterTrigger::Configure()
{  
  GetParameter("DataReadoutName", dataReadoutName_);
  GetParameter("TriggerName",triggerName_);
  GetParameter("TriggerConfigID",configIDParam_);
  GetParameter("DOMSetsName", domSetsName_);

  if(configIDParam_ != INT_MIN)
    configID_ = configIDParam_;
}

void ClusterTrigger::DAQ(I3FramePtr frame)
{

  // Get the DOMSets from the frame
  I3MapKeyVectorIntConstPtr domSets = 
    frame->Get<I3MapKeyVectorIntConstPtr>(domSetsName_);

  // Create the trigger object
  ClusterTriggerAlgorithm stringTrigger(triggerWindow_, triggerThreshold_, coherenceLength_);

  log_debug("Checking for StringTriggers...");
  eventCount_++;

  std::vector<I3Trigger> tlist;

  /*------------------------------------------------------------*
   * Get DomLaunchSeriesMap from frame
   *------------------------------------------------------------*/
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
    log_debug("Got %d hits", count);

    /*------------------------------------------------------------*
     * Fill the TriggerHits
     *------------------------------------------------------------*/
    TriggerHitVectorPtr hits(new TriggerHitVector);
    FillHits(fullMap, hits, domSets);

    std::sort(hits->begin(), hits->end());
    Dump(hits);

    stringTrigger.AddHits(hits);

    unsigned int numTriggers = stringTrigger.GetNumberOfTriggers();
    for (unsigned int n = 0; n < numTriggers; n++) {

      TriggerHitVectorPtr triggerHits = stringTrigger.GetNextTrigger();

      // We have a trigger!
      triggerCount_++;

      I3Trigger tr;
      tr.GetTriggerKey() = triggerKey_;
      
      // The trigger times are defined by the hits in the time window
      double startTime(triggerHits->front().time);
      double stopTime(triggerHits->back().time);

      tr.SetTriggerFired(true);
      tr.SetTriggerTime(startTime);
      tr.SetTriggerLength(stopTime - startTime);
      
      tlist.push_back(tr);

    } // end loop over triggers 
    
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
  std::vector<I3Trigger>::const_iterator tIter;
  for (tIter = tlist.begin(); tIter != tlist.end(); tIter++)
    triggers->insert(triggers->begin(), *tIter);    

  frame->Put(triggerName_,triggers);
  PushFrame( frame );

}

void ClusterTrigger::Finish()
{
  log_info("Found %d triggers out of %d events", triggerCount_, eventCount_);
}

void ClusterTrigger::FillHits(I3DOMLaunchSeriesMapConstPtr launches, 
			      TriggerHitVectorPtr hits, 
			      I3MapKeyVectorIntConstPtr domSets)
{

  I3DOMLaunchSeriesMap::const_iterator mapIter;
  for (mapIter = launches->begin(); mapIter != launches->end(); mapIter++) {
    const OMKey& omKey = mapIter->first;
    if( DOMSetFunctions::InDOMSet(omKey, 2, domSets)) {
      I3DOMLaunchSeries::const_iterator seriesIter;
      for (seriesIter = mapIter->second.begin(); 
	   seriesIter != mapIter->second.end(); 
	   seriesIter++) {
        if( seriesIter->GetLCBit() ){
	  TriggerHitPtr hit(new TriggerHit);
	  hit->pos = omKey.GetOM();
	  hit->string = omKey.GetString();
	  hit->time = seriesIter->GetStartTime();
	  hits->push_back(*hit);
        }
      }
    }	
  }
}


void ClusterTrigger::Dump(TriggerHitVectorPtr input)
{
  log_debug("Dumping Event with %zd hits...", input->size());

  TriggerHitVector::const_iterator iter;
  for (iter = input->begin(); iter != input->end(); iter++) {
    const TriggerHit& hit = *iter;
    log_debug("  Time %f   Position %u", hit.time, hit.pos);
  }
}

void ClusterTrigger::DetectorStatus(I3FramePtr frame){
  // Get DetectorStatus from the frame
  I3DetectorStatusConstPtr detStatus = frame->Get<I3DetectorStatusConstPtr>();
  if(!detStatus) log_fatal("This DetectorStatus frame has no I3DetectorStatus object.");

  boost::optional<tk_ts_pair_t> tkts = GetTriggerStatus
    (detStatus, _sourceID = SOURCEID, _typeID = TYPEID, _configID = configID_ );

  if(!tkts) log_fatal("Failed to configure this module from the DetectorStatus.");

  // this needs to be passed on to the I3Trigger
  triggerKey_ = tkts.get().first;
  
  // now set the parameters
  tkts.get().second.GetTriggerConfigValue("multiplicity", triggerThreshold_);
  tkts.get().second.GetTriggerConfigValue("timeWindow", triggerWindow_);
  tkts.get().second.GetTriggerConfigValue("coherenceLength", coherenceLength_);

  std::stringstream sstr;
  sstr<<"multiplicity: "<<triggerThreshold_<<", timeWindow: "<<triggerWindow_
      <<", coherenceLength: "<<coherenceLength_;
  log_debug("%s",sstr.str().c_str());

  PushFrame( frame );
}

