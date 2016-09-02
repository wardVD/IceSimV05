
/**
 * copyright  (C) 2009
 * the icecube collaboration

 *
 * @file I3Pruner.cxx
 */

#include <iostream>
#include "icetray/I3TrayHeaders.h"
#include "dataclasses/physics/I3Trigger.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/TriggerKey.h"
#include "icetray/I3Units.h"

#include "trigger-sim/modules/I3Pruner.h"
#include "trigger-sim/utilities/ReadoutWindowUtil.h"

#include <boost/foreach.hpp>

I3_MODULE(I3Pruner);

using namespace std;

I3Pruner::I3Pruner(const I3Context& ctx) : 
  I3ConditionalModule(ctx),
  triggerName_("I3TriggerHierarchy")
{
   dataReadoutNames_.push_back("InIceRawData");
   dataReadoutNames_.push_back("IceTopRawData");
   AddParameter("DOMLaunchSeriesMapNames", "This holds the DOM launches", dataReadoutNames_);
   AddParameter("GlobalTriggerName", "Name of the global trigger hierarchy", triggerName_);
   AddOutBox("OutBox");
}

I3Pruner::~I3Pruner()
{
}

void I3Pruner::Configure()
{  
   GetParameter("DOMLaunchSeriesMapNames", dataReadoutNames_); 
   GetParameter("GlobalTriggerName",triggerName_);
}

void I3Pruner::DAQ(I3FramePtr frame){

  // Get the detector status
  const I3DetectorStatus& status = frame->Get<I3DetectorStatus>();

  // Create the ReadoutWindowUtil helper class
  ReadoutWindowUtil rwUtil(status);

  // Get the trigger hierarchy
  I3TriggerHierarchyConstPtr gTrigger = frame->Get<I3TriggerHierarchyConstPtr>(triggerName_);
  
  I3TriggerHierarchy::iterator th_iter;
  if (gTrigger && gTrigger->size()) {

    BOOST_FOREACH(std::string dataReadoutName, dataReadoutNames_) { // suppose you have several input maps, loops through all

      // skip if the map isn't found in the frame
      if ( ! frame->Has(dataReadoutName) ) continue;      

      I3DOMLaunchSeriesMapPtr pruned_map = I3DOMLaunchSeriesMapPtr(new I3DOMLaunchSeriesMap);
      I3DOMLaunchSeriesMapConstPtr dlsInMap = frame->Get<I3DOMLaunchSeriesMapConstPtr>(dataReadoutName);
      // if the pointer is still NULL something very strange happened
      if (!dlsInMap)
	      log_fatal("No I3DOMLaunchSeriesMap named \"%s\" found in frame.",
		      dataReadoutName.c_str());
         
      I3DOMLaunchSeriesMap::const_iterator iter;
      for (iter = dlsInMap->begin(); iter != dlsInMap->end(); ++iter) { // loop thorugh all doms in this dataReadoutName

	// get the sub-detector of these launches
	bool hitIsInIce = iter->first.IsInIce();
	bool hitIsIceTop = iter->first.IsIceTop();

	I3DOMLaunchSeries launch_series;
	I3DOMLaunchSeries::const_iterator dlIter;
	for (dlIter = iter->second.begin(); dlIter != iter->second.end(); ++dlIter) { // loop through the launches per dom

	  bool keepHit = false;
	  double hitTime = dlIter->GetStartTime();

	  // check the trigger hierarchy for a readout window that includes this hit
	  for (th_iter = gTrigger->begin(); th_iter != gTrigger->end(); th_iter++) {

	    // make sure this is a subdetector trigger (inice or icetop)
	    bool triggerIsInIce = th_iter->GetTriggerKey().GetSource() == TriggerKey::IN_ICE;
	    bool triggerIsIceTop = th_iter->GetTriggerKey().GetSource() == TriggerKey::ICE_TOP;

	    if ( triggerIsInIce || triggerIsIceTop ) {

	      // Get the readout windows
	      const I3Trigger& trigger = *th_iter;
	      std::pair<double,double> iniceReadoutWindow = rwUtil.GetInIceReadoutWindow(trigger);
	      std::pair<double,double> icetopReadoutWindow = rwUtil.GetIceTopReadoutWindow(trigger);
			 
	      // Get the times (initalize to inice)
	      double earliestTime = iniceReadoutWindow.first;
	      double latestTime = iniceReadoutWindow.second;
	      if (hitIsIceTop) {
		earliestTime = icetopReadoutWindow.first;
		latestTime = icetopReadoutWindow.second;
	      }

	      // See if this hit should be kept
	      if (hitIsInIce && earliestTime != NAN && latestTime != NAN) {
		// This is an inice hit and the inice readout window is valid
		if ( (hitTime >= earliestTime) && (hitTime <= latestTime) )
		  keepHit = true;
	      }

	      if (hitIsIceTop && earliestTime != NAN && latestTime != NAN) {
		// This is an icetop hit and the icetop readout window is valid
		if ( (hitTime >= earliestTime) && (hitTime <= latestTime) )
		  keepHit = true;
	      }

	    }
 
	    // if we know we want it, break the iteration over sub-detector triggers
	    if (keepHit) break;

	  } // end loop over triggers

	  // now see if we kept this hit
	  if (keepHit) {
	    //only push back events within the readout time window
	    launch_series.push_back(*dlIter);             
	  }

	} // end loop over hits

	if (launch_series.size()) {
	  (*pruned_map)[iter->first] = launch_series;
	}

      } // end loop over doms

      log_debug("Datareadoutname: %s ",dataReadoutName.c_str());
      frame->Delete(dataReadoutName);
      frame->Put(dataReadoutName, pruned_map);

    } // end loop over dlmaps
   
  }
   
  PushFrame(frame,"OutBox");
}

