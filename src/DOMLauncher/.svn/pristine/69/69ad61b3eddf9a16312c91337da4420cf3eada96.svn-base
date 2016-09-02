/**
 * class: I3InIceDOMObject
 *
 * @version $Id: $
 *
 * @date: $Date: $
 *
 * @author Samuel Flis <samuel.d.flis@gmail.com>
 *
 * (c) 2011,2012 IceCube Collaboration
 */

#include <cfloat>
#include <queue>
#include <boost/foreach.hpp>

#include "phys-services/I3RandomService.h"
#include "dataclasses/I3DOMFunctions.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/geometry/I3Geometry.h"

#include "I3IceTopDOM.h"

double const I3IceTopDOM::cableCorrectionIceTop = 1650*I3Units::ns;//[ns]

I3IceTopDOM::I3IceTopDOM(){}

I3IceTopDOM::I3IceTopDOM(I3RandomServicePtr rng, const OMKey& om):
  I3DOM(rng, om) {}
  
I3IceTopDOM::I3IceTopDOM(boost::shared_ptr<I3RandomService> rng, 
                         const OMKey& om, 
                         double &globlaTime,
                         domlauncherutils::PulseTemplateMap &speTemplateMap):
                         I3DOM(rng, om, globlaTime, speTemplateMap) {}
  
bool I3IceTopDOM::Configure(const I3DOMCalibration& cal,
                            const I3DOMStatus& stat){

  double threshold(NAN);
  cableCorrection_ = I3IceTopDOM::cableCorrectionIceTop;

  bool success(true);
  if(stat.trigMode == I3DOMStatus::MPE)
    threshold = OldmpeThreshold( stat );
  else
    if(stat.trigMode == I3DOMStatus::SPE)
      threshold = OldspeThreshold( stat );
    else{
      log_error("Unknown trigger mode for DOM %s", domId_.str().c_str());
      success = false;
    }

  success &= I3DOM::Configure(cal, stat, threshold);

  if(stat.domGainType == I3DOMStatus::High &&
     stat.trigMode != I3DOMStatus::MPE){
    log_error("DOM %s is misconfigured.", domId_.str().c_str());
    success = false;
  }

  if(stat.domGainType == I3DOMStatus::Low &&
     stat.trigMode != I3DOMStatus::SPE){
    log_error("DOM %s is misconfigured.", domId_.str().c_str());
    success = false;
  }

  return success;
}

void I3IceTopDOM::MakeCoarseChargeStamp(const dlud::DiscCross& discrx,
                                        int chip,
                                        I3DOMLaunch &domLaunch){

   uint chargeSum = 0;//in unit of counts
   int channel = 2;
   if(discrx.type == dlud::HLC || discrx.type == dlud::CPU_REQUESTED){

      while(domLaunch.GetRawATWD(channel).empty() && channel>0) channel--;
      std::vector<int> &rawWaveForm = domLaunch.GetRawATWD(channel);

      for(uint i = 0; i<rawWaveForm.size(); i++){
         chargeSum += rawWaveForm[i];
      }

   }
   else if(discrx.type == dlud::SLC){
     short nDigitizations(1);
     channel = 0;
     std::vector<int> RawATWDOutput;
     if(ATWDDigitization(discrx, 0, chip, RawATWDOutput)){//digitizing channel 0
       nDigitizations++;
       channel++;
       if(ATWDDigitization(discrx, 1, chip, RawATWDOutput)){//digitizing channel 1
         nDigitizations++;
         channel++;
         ATWDDigitization(discrx, 2, chip, RawATWDOutput);//digitizing channel 2
       }
     }
     for(uint i = 0; i<RawATWDOutput.size(); i++){
       chargeSum += RawATWDOutput[i];
     }
   }

   domLaunch.SetChargeStampHighestSample(0);//NOTE: this is an ugly fix. Somehow
   //this attribute is not initialized to 0 and IT SLC launches gets a random
   //value for this attribute. It should always be 0 for IT Launches since they
   //don't have a charge stamp but a raw ATWD charge stamp.

   domLaunch.SetRawATWDChargeStamp(chargeSum);
   domLaunch.SetWhichATWDChargeStamp(channel);
}

void I3IceTopDOM::CreateLCLinks(const I3DOMMap& domMap){

  domNeighbors_.clear();

  // Each IceTop DOM listen only to one other. High gain DOMs listen to each other and
  // the low gain DOMs listen to the high gain which sits in the neighboring tank.
  // Since the DOMs sit in two tanks at the same height at the surface up and down
  // isn't well defined and the name of the pointer is thus arbitrary.
  std::vector<OMKey> neighbor_keys;
  if(domStat_.txMode == I3DOMStatus::UpAndDown){
    // High gain DOMs talk to both DOMs in the other tank
    // Low gain DOMs listen but don't speak
    if(domId_.GetOM() <= 62){
      // add boths DOMs in the other tank
      neighbor_keys.push_back( OMKey(domId_.GetString(), 63 ) );
      neighbor_keys.push_back( OMKey(domId_.GetString(), 64 ) );
    }else{
      // add boths DOMs in the other tank
      neighbor_keys.push_back( OMKey(domId_.GetString(), 61 ) );
      neighbor_keys.push_back( OMKey(domId_.GetString(), 62 ) );
    }
  }

  BOOST_FOREACH(const OMKey& neighbor_key, neighbor_keys){
    if( domMap.find(neighbor_key) != domMap.end() ){
      I3DOMMap::const_iterator iter = domMap.find(neighbor_key);
      I3IceTopDOMPtr obj = boost::dynamic_pointer_cast<I3IceTopDOM>(iter->second);
      if(obj)
        domNeighbors_.push_back(obj.get());
      else
        log_fatal("Only IceTop neighbors for now.");
    }
  }

}
