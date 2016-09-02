/**
    Copyright  (C) 2006
    The IceCube Collaboration
    $Id$
    @version 00-00-01
    @date $Date$
    @author Seon-Hee Seo <shseo@phys.psu.edu>
*/

#include "icepick/I3PickRawNHitEventFilter.h"
#include <dataclasses/physics/I3DOMLaunch.h>

I3PickRawNHitEventFilter::I3PickRawNHitEventFilter(const I3Context& context) : 
  I3IcePick(context) 
{
  domLaunchSource_ = 0;  //--both IceIce and IceTop
  hitThresholdType_ = 1; // chooses event w/ hits between lowThresh and highThreshold
  hitThresholdHigh_ = 1000000;
  hitThresholdLow_ = 1;

  AddParameter("DOMlaunchSource",
               "Select Source of DOMlaunch: InIce (1), IceTop (2), or both (any other integer)",
               domLaunchSource_);
  
  AddParameter("HitThresholdType", 
	       "HitThreshold Selection Type: choose high (2), low (0) or between (1) (Default = 1)", 
	       hitThresholdType_);
  
  AddParameter("HitThresholdHigh", 
	       "Number of maximum required hits for an event to select", 
	       hitThresholdHigh_);
  
  AddParameter("HitThresholdLow", 
	       "Number of minimum required hits for an event to select", 
	       hitThresholdLow_);
  
  iniceDOMLaunchSeriesMapName_ = "InIceRawData";
  AddParameter("InIceDOMLaunchSeriesMapName",
	       "Name of the in Ice DOM Launch series map to count hits for",
	       iniceDOMLaunchSeriesMapName_);
  
  icetopDOMLaunchSeriesMapName_ = "IceTopRawData";
  AddParameter("IceTopDOMLaunchSeriesMapName",
	       "Name of the Ice Top DOM Launch series map to count hits for",
	       icetopDOMLaunchSeriesMapName_);
      
}

void I3PickRawNHitEventFilter::Configure()
{
  // telling the world where we are at
    log_info("Configuring the I3PickRawNHitEventFilter");

  // retrieve the parameter
    GetParameter("DOMlaunchSource", domLaunchSource_);
    GetParameter("HitThresholdType", hitThresholdType_);
    GetParameter("HitThresholdHigh", hitThresholdHigh_);
    GetParameter("HitThresholdLow",  hitThresholdLow_);
    GetParameter("InIceDOMLaunchSeriesMapName",iniceDOMLaunchSeriesMapName_);
    GetParameter("IceTopDOMLaunchSeriesMapName",icetopDOMLaunchSeriesMapName_);
}

bool I3PickRawNHitEventFilter::SelectFrame(I3Frame& frame)
{
  if(domLaunchSource_ != 2){
      if ( ! frame.Has(iniceDOMLaunchSeriesMapName_) ) {
         log_debug("-> event missing InIceDOMLaunch series ... discarding it");
         return false;
      }
  } 
  if(domLaunchSource_ != 1){
      if ( ! frame.Has(icetopDOMLaunchSeriesMapName_) ) {
         log_debug("-> event missing IceTopDOMLaunch series ... discarding it");
         return false;
      }
  }

  const I3DOMLaunchSeriesMap iniceDOMLaunchSeriesMap = 
    frame.Get<I3DOMLaunchSeriesMap>(iniceDOMLaunchSeriesMapName_);
  
  const I3DOMLaunchSeriesMap icetopDOMLaunchSeriesMap = 
    frame.Get<I3DOMLaunchSeriesMap>(icetopDOMLaunchSeriesMapName_);
  
  int totRawHitsInice = iniceDOMLaunchSeriesMap.size();
  int totRawHitsIcetop = icetopDOMLaunchSeriesMap.size();
  int totRawHits = totRawHitsInice + totRawHitsIcetop;

  if(domLaunchSource_ == 1) totRawHits = totRawHitsInice;
  if(domLaunchSource_ == 2) totRawHits = totRawHitsIcetop;

  if(hitThresholdType_ == 2){
      if(totRawHits >= hitThresholdHigh_){
          return true;
      }
  } else if(hitThresholdType_ == 0){
      if(totRawHits <= hitThresholdLow_){
          return true;
      }
  } else if(hitThresholdType_ == 1){
      if(hitThresholdLow_ > hitThresholdHigh_) log_fatal("hitThresholdLow > hitThresholdHigh: please reset!");
      if(totRawHits >= hitThresholdLow_ && totRawHits <= hitThresholdHigh_){
          return true;
      }
  }else{
    log_error("HitThresholdType is not set correctly: Choose 0, 1, or 2");
  }
  return false;
}
