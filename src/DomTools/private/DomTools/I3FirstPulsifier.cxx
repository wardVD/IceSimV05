#include "DomTools/I3FirstPulsifier.h"

#include <string>

using namespace std;

I3_MODULE(I3FirstPulsifier);

I3RecoPulseSeriesMap 
GetFirstPulseSeriesMap(const I3RecoPulseSeriesMap&, bool);

I3RecoPulseSeriesMapMask 
GetFirstPulseSeriesMapMask(const I3Frame&, const std::string &);                           

// python version using I3RecoPulseSeriesMapMask (simply set first pulse's 
// mask to true, rest to false.
// This is of course much easier, but charge integration is not possible. 
// Pulses are not sorted, but they should be already sorted coming out of
// Wavedeform.
//
// pulses = dataclasses.I3RecoPulseSeriesMapMask.from_frame(frame, key)
// mask = dataclasses.I3RecoPulseSeriesMapMask(frame, key)
// mask.set_none()
// for om in pulses.iterkeys():
//    mask.set(om, 0, True)

/* ******************************************************************** */
/* Constructor                                                          */
/* ******************************************************************** */
I3FirstPulsifier::I3FirstPulsifier(const I3Context& ctx) : I3ConditionalModule(ctx) {
    log_debug("I3FirstPulsifier %s: Entering I3FirstPulsifier::I3FirstPulsifier().", GetName().c_str());

    pulseSeriesMapName_ = "OnlinePulses";
    AddParameter("InputPulseSeriesMapName", 
            "Name of input pulse series map", 
            pulseSeriesMapName_);

    outputPulseSeriesMapName_ = "OnlinePulsesFirstPulses";
    AddParameter("OutputPulseSeriesMapName", 
            "Name of output pulse series map (or mask)", 
            outputPulseSeriesMapName_);

    keepOnlyFirstCharge_ = false;
    AddParameter("KeepOnlyFirstCharge", 
            "Do you want the first pulse to carry only the charge of the first pulse? "
            "(default is charge of all pulses)", 
            keepOnlyFirstCharge_);

    useMask_ = false;
    AddParameter("UseMask", 
            "Do you want the output to be a bit mask (I3RecoPulseSeriesMapMask) "
            "with all pulses except for the first one set to False? "
            "This implies KeepOnlyFirstCharge = True since with the mask you can't "
            "integrate the charge",
            useMask_);

    AddOutBox("OutBox");
}

/* ******************************************************************** */
/* Configure                                                            */
/* ******************************************************************** */
void I3FirstPulsifier::Configure() {
    log_debug("I3FirstPulsifier %s: Entering Configure()", GetName().c_str());

    GetParameter("InputPulseSeriesMapName", pulseSeriesMapName_);
    GetParameter("OutputPulseSeriesMapName", outputPulseSeriesMapName_);
    GetParameter("KeepOnlyFirstCharge", keepOnlyFirstCharge_);
    GetParameter("UseMask", useMask_);

}

/* ******************************************************************** */
/* Physics                                                              */
/* ******************************************************************** */
void I3FirstPulsifier::Physics(I3FramePtr frame) {
  log_debug("I3FirstPulsifier %s: Starting a new Physics frame", GetName().c_str());
  
  // Check if frame has all we need:
  if ( ! frame->Has(pulseSeriesMapName_ ) ) {
    log_warn("PulseSeries missing: %s, cannot first-pulsify!!!", pulseSeriesMapName_.c_str());
  }
  else {
    if ( useMask_ ){
      log_trace("Getting first pulse series map mask...");
      I3RecoPulseSeriesMapMask firstPulseMapMask = 
        GetFirstPulseSeriesMapMask(*frame, pulseSeriesMapName_);
      log_trace("Got first pulse series map mask.");
      I3RecoPulseSeriesMapMaskPtr firstPulseMapMaskPtr(new I3RecoPulseSeriesMapMask(firstPulseMapMask));
      frame->Put(outputPulseSeriesMapName_, firstPulseMapMaskPtr);
    }
    else {
      const I3RecoPulseSeriesMap& pulseMap =
        frame->Get<I3RecoPulseSeriesMap>(pulseSeriesMapName_);
      log_trace("Getting first pulse series map...");
      I3RecoPulseSeriesMap firstPulseMap = 
        GetFirstPulseSeriesMap(pulseMap, keepOnlyFirstCharge_);
      log_trace("Got first pulse series map.");
      I3RecoPulseSeriesMapPtr firstPulseMapPtr(new I3RecoPulseSeriesMap(firstPulseMap));
      frame->Put(outputPulseSeriesMapName_, firstPulseMapPtr);
    }
  }

  PushFrame(frame,"OutBox"); // Don't forget to push the frame back to the outbox!!!
}


bool compare(const I3RecoPulse& a, const I3RecoPulse& b) {
  return (a.GetTime() < b.GetTime());
}

I3RecoPulseSeriesMap GetFirstPulseSeriesMap(const I3RecoPulseSeriesMap& pulseMap, 
                                            bool keepOnlyFirstCharge) {
  I3RecoPulseSeriesMap firstPulseMap; // create empty PulseSeriesMap
  for (I3RecoPulseSeriesMap::const_iterator iter=pulseMap.begin(); iter!=pulseMap.end(); ++iter) {
    const I3RecoPulseSeries& pulseSeries = iter->second;
    const OMKey& omKey = iter->first;
    int nHits = pulseSeries.size();
    log_trace("GetFirstPulseSeriesMap: OM (%d,%d). Found %d hits.",
              omKey.GetString(),
              omKey.GetOM(),
              nHits);
    if (nHits > 1) { // take time of first pulse
      log_trace("GetFirstPulseSeriesMap: Reducing hits on OM (%d,%d) from %d to 1",
                omKey.GetString(),
                omKey.GetOM(),
                nHits);
      const I3RecoPulse& firstPulse = *min_element(pulseSeries.begin(), pulseSeries.end(), compare);
      // create new I3PulseSeries that has only first pulse
      I3RecoPulseSeries newPulseSeries;
      newPulseSeries.push_back(firstPulse);
      if ( !keepOnlyFirstCharge ){
        float charge = 0.;
        // calculate sum of all charges
        for (I3RecoPulseSeries::const_iterator it=pulseSeries.begin(); it!=pulseSeries.end(); ++it){
          charge += it->GetCharge();
        }
        newPulseSeries.front().SetCharge(charge);
      }
      firstPulseMap.insert(pair<OMKey, I3RecoPulseSeries>(omKey, newPulseSeries));
    }
    else { // nHits <= 1, just copy the pulse series
      firstPulseMap.insert(pair<OMKey, I3RecoPulseSeries>(omKey, pulseSeries));
    }
  }
  return firstPulseMap;
}

I3RecoPulseSeriesMapMask GetFirstPulseSeriesMapMask(const I3Frame& frame, const string &key) {
  I3RecoPulseSeriesMapMask mapMask(frame, key);
  const I3RecoPulseSeriesMap& pulseMap = frame.Get<I3RecoPulseSeriesMap>(key);
  mapMask.SetNone();
  for (I3RecoPulseSeriesMap::const_iterator it=pulseMap.begin(); it!=pulseMap.end(); ++it){
    int nHits = it->second.size(); // size of PulseSeries
    log_trace("GetFirstPulseSeriesMapMask: OM (%d,%d). Found %d hits.",
              it->first.GetString(),
              it->first.GetOM(),
              nHits);
    if (nHits > 0) { // set only first pulse to true
      mapMask.Set(it->first, 0, true);
      log_trace("GetFirstPulseSeriesMapMask: Set only the first pulse to true, rest to false.");
    }
  }
  return mapMask;
}
