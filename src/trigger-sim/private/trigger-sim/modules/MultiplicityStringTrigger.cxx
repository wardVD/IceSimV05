/**
 * copyright  (C) 2006
 * the icecube collaboration
 * $Id:
 *
 * @file MultiplicityStringTrigger.cxx
 * @version
 * @date
 * @author toale
 */

#include <trigger-sim/modules/MultiplicityStringTrigger.h>
#include <trigger-sim/algorithms/MultiplicityStringTriggerAlgorithm.h>
#include <icetray/OMKey.h>
#include <dataclasses/I3Vector.h>
#include <icetray/I3Units.h>
#include <icetray/I3Bool.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <boost/foreach.hpp>
#include <boost/assign/std/vector.hpp>
#include <trigger-sim/utilities/DetectorStatusUtils.h>

using namespace boost::assign;
using DetectorStatusUtils::tk_ts_pair_t;
using DetectorStatusUtils::_sourceID;
using DetectorStatusUtils::_typeID;
using DetectorStatusUtils::_configID;
using DetectorStatusUtils::GetTriggerStatus;

const TriggerKey::SourceID SOURCEID(TriggerKey::IN_ICE);
const TriggerKey::TypeID TYPEID(TriggerKey::STRING);

I3_MODULE(MultiplicityStringTrigger);

MultiplicityStringTrigger::MultiplicityStringTrigger(const I3Context& context) : 
  I3Module(context),
  dataReadoutName_("InIceRawData"),
  triggerName_("I3Triggers"),
  configIDParam_(INT_MIN),
  triggerWindow_(NAN),
  triggerThreshold_(INT_MAX),
  maxSpan_(INT_MAX),
  vetoDepthName_(""),
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
  
  AddParameter("VetoDepthName",
	       "String key to use to get the VetoDepth from the I3TriggerStatus",
	       vetoDepthName_);
  
  AddOutBox("OutBox");
}

MultiplicityStringTrigger::~MultiplicityStringTrigger() {}

void MultiplicityStringTrigger::Configure()
{  
  GetParameter("DataReadoutName", dataReadoutName_);
  GetParameter("TriggerName",triggerName_);
  GetParameter("TriggerConfigID",configIDParam_);
  GetParameter("VetoDepthName",vetoDepthName_);	       	       

  if(configIDParam_ != INT_MIN)
    configID_ = configIDParam_;

}

void MultiplicityStringTrigger::DAQ(I3FramePtr frame)
{

  // Create the trigger object
  MultiplicityStringTriggerAlgorithm stringTrigger
    (triggerWindow_, 
     triggerThreshold_, 
     maxSpan_, 
     vetoDepth_ ? vetoDepth_.get() : 0 );

  log_debug("Checking for StringTriggers...");
  eventCount_++;

  MultiplicityStringTriggerMap tmap;

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
    log_debug("Got %d hits to split", count);

    /*------------------------------------------------------------*
     * Fill the StringMap
     *------------------------------------------------------------*/
    IntTriggerHitVectorMapPtr stringMap(new IntTriggerHitVectorMap);
    FillHits(fullMap, stringMap);

    /*------------------------------------------------------------*
     * Loop over the StringMap
     *------------------------------------------------------------*/
    IntTriggerHitVectorMap::iterator stringMapIter;
    for (stringMapIter = stringMap->begin(); 
	 stringMapIter != stringMap->end(); 
	 stringMapIter++) {
      int stringNumber = stringMapIter->first;
      TriggerHitVectorPtr hitVector(new TriggerHitVector(stringMapIter->second));

      log_debug("Working on String %i:", stringNumber);

      /*------------------------------------------------------------*
       * Time order the hits
       *------------------------------------------------------------*/
      std::sort(hitVector->begin(), hitVector->end());
      Dump(hitVector);

      /*------------------------------------------------------------*
       * Check Trigger condition on this string
       *------------------------------------------------------------*/
      stringTrigger.AddHits(hitVector);

      unsigned int numTriggers = stringTrigger.GetNumberOfTriggers();
      for (unsigned int n = 0; n < numTriggers; n++) {

	TriggerHitVectorPtr timeHits = stringTrigger.GetNextTrigger();

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

	if(tmap.find(stringNumber) == tmap.end()){
	  std::vector<I3Trigger> tlist;
	  tlist.push_back(tr);
	  tmap[stringNumber] = tlist;
	}else{
	  tmap[stringNumber].push_back(tr);
	}

      } // end loop over time windows

    } // end loop over strings

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
  MultiplicityStringTriggerMap::iterator tmap_iter;
  for(tmap_iter = tmap.begin(); tmap_iter != tmap.end(); tmap_iter ++){
    BOOST_FOREACH(I3Trigger& t, tmap_iter->second)
      triggers->insert(triggers->begin(), t);    
  }

  frame->Put(triggerName_,triggers);
  PushFrame(frame,"OutBox");
}

void MultiplicityStringTrigger::Finish()
{
  log_info("Found %d triggers out of %d events", triggerCount_, eventCount_);
}

void MultiplicityStringTrigger::FillHits(I3DOMLaunchSeriesMapConstPtr fullMap, 
					 IntTriggerHitVectorMapPtr stringMap)
{

  I3DOMLaunchSeriesMap::const_iterator fullMapIter;
  for (fullMapIter = fullMap->begin(); fullMapIter != fullMap->end(); fullMapIter++) {
    const OMKey& omKey = fullMapIter->first;
    const I3DOMLaunchSeries& fullSeries = fullMapIter->second;

    // See if this string exists in the map
    int stringNumber = omKey.GetString();    
    if(stringNumber < 0 ) continue; // no AMANDA
    // if stringNumber_ is not set consider all strings
    // otherwise just operate on the one string.
    if(!stringNumber_ || stringNumber == stringNumber_.get()){
      if (stringMap->find(stringNumber) == stringMap->end()){

	// This is a new string
	TriggerHitVectorPtr hitVector(new TriggerHitVector);
	I3DOMLaunchSeries::const_iterator fullSeriesIter;
	for (fullSeriesIter = fullSeries.begin(); 
	     fullSeriesIter != fullSeries.end(); 
	     fullSeriesIter++) {
	  if( fullSeriesIter->GetLCBit() ){
	    TriggerHitPtr hit(new TriggerHit);
	    hit->pos = omKey.GetOM();
	    hit->time = fullSeriesIter->GetStartTime();
	    hitVector->push_back(*hit);
	  }
	}
	if(hitVector->size()){
	  (*stringMap)[stringNumber] = *hitVector;
	}
	
      } else {

	// This is an existing string
	TriggerHitVector& hitVector = stringMap->find(stringNumber)->second;
	I3DOMLaunchSeries::const_iterator fullSeriesIter;
	for (fullSeriesIter = fullSeries.begin(); 
	     fullSeriesIter != fullSeries.end();
	     fullSeriesIter++) {
	  if( fullSeriesIter->GetLCBit() ){
	    TriggerHitPtr hit(new TriggerHit);
	    hit->pos = omKey.GetOM();
	    hit->time = fullSeriesIter->GetStartTime();
	    hitVector.push_back(*hit);
	  }
	}
      }
    }
  }

}


void MultiplicityStringTrigger::Dump(TriggerHitVectorPtr input)
{
  log_debug("Dumping Event with %zd hits...", input->size());

  TriggerHitVector::const_iterator iter;
  for (iter = input->begin(); iter != input->end(); iter++) {
    const TriggerHit& hit = *iter;
    log_debug("  Time %f   Position %u", hit.time, hit.pos);
  }
}

void MultiplicityStringTrigger::DetectorStatus(I3FramePtr frame){
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

  boost::optional<int> maxSpan;
  tkts.get().second.GetTriggerConfigValue("coherenceLength", maxSpan);
  if(!maxSpan){
    // could be ULEE
    tkts.get().second.GetTriggerConfigValue("maxLength", maxSpan);
    tkts.get().second.GetTriggerConfigValue("string", stringNumber_);
    if(!stringNumber_)
      log_fatal("This is the ULEE, but can't find the 'string' "
		"parameter in the I3TriggerStatus.");

  }
  if(maxSpan){
    maxSpan_ = maxSpan.get();
  }else{
    log_fatal("Coulnd't find coherenceLength or maxLength in the I3TriggerStatus.");
  }

  tkts.get().second.GetTriggerConfigValue(vetoDepthName_, vetoDepth_);

  PushFrame( frame );
}
