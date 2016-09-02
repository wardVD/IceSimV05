#include "trigger-sim/utilities/ReadoutWindowUtil.h"
#include "dataclasses/TriggerKey.h"

ReadoutWindowUtil::ReadoutWindowUtil(const I3DetectorStatus& detectorStatus) 
{
  triggerStatus_ = detectorStatus.triggerStatus;
}

ReadoutWindowUtil::~ReadoutWindowUtil() {}

std::pair<double,double> ReadoutWindowUtil::GetInIceReadoutWindow(const I3Trigger& trigger) {
  return GetReadoutWindow(I3TriggerStatus::INICE, trigger);
}

std::pair<double,double> ReadoutWindowUtil::GetIceTopReadoutWindow(const I3Trigger& trigger) {
  return GetReadoutWindow(I3TriggerStatus::ICETOP, trigger);
}

double ReadoutWindowUtil::GetEarliestReadoutTime(const I3Trigger& trigger) {

  std::pair<double,double> iniceReadout = GetReadoutWindow(I3TriggerStatus::INICE, trigger);
  std::pair<double,double> icetopReadout = GetReadoutWindow(I3TriggerStatus::ICETOP, trigger);

  return std::min(iniceReadout.first, icetopReadout.first);
  
}

double ReadoutWindowUtil::GetLatestReadoutTime(const I3Trigger& trigger) {

  std::pair<double,double> iniceReadout = GetReadoutWindow(I3TriggerStatus::INICE, trigger);
  std::pair<double,double> icetopReadout = GetReadoutWindow(I3TriggerStatus::ICETOP, trigger);

  return std::max(iniceReadout.second, icetopReadout.second);
  
}

/**
 * Return one readout window corresponding to subdetector InIce or IceTop
 *
 */
std::pair<double,double> ReadoutWindowUtil::GetReadoutWindow(I3TriggerStatus::Subdetector subdetector, const I3Trigger& trigger) {

  // The return window
  std::pair<double,double> readoutWindow(NAN,NAN);

  // Get the trigger key for this trigger
  const TriggerKey& triggerKey = trigger.GetTriggerKey();

  // Lookup the I3TriggerStatus for this TriggerKey
  std::map<TriggerKey, I3TriggerStatus>::const_iterator triggerStatusIter;
  triggerStatusIter = triggerStatus_.find(triggerKey);
  if (triggerStatusIter == triggerStatus_.end()) {
    // This TriggerKey is not in the I3TriggerStatus
    log_debug("TriggerKey not found in I3TriggerStatus.");
    return readoutWindow;
  }
  I3TriggerStatus triggerStatus = triggerStatusIter->second;
  
  // Get the I3TriggerReadoutConfig map for this I3Trigger
  const std::map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> readoutConfigMap = triggerStatus.GetReadoutSettings();

  // Lookup the I3TriggerReadoutConfig first for I3TriggerStatus::ALL
  std::map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig>::const_iterator readoutConfigIter;
  readoutConfigIter = readoutConfigMap.find(I3TriggerStatus::ALL);
  if (readoutConfigIter == readoutConfigMap.end()) {

    // Does not exist as type ALL, Now lookup the I3TriggerReadoutConfig for I3TriggerStatus::Subdetector
    readoutConfigIter = readoutConfigMap.find(subdetector);
    if (readoutConfigIter == readoutConfigMap.end()) {
      // No readouts for either I3TriggerStatus::ALL or I3TriggerStatus::Subdetector for this I3Trigger
      log_debug("No readouts for either I3TriggerStatus::ALL or I3TriggerStatus::Subdetector for this I3Trigger");
      return readoutWindow;
    }

  }
  I3TriggerReadoutConfig readoutConfig = readoutConfigIter->second;

  // Get the trigger times of this I3Trigger
  double triggerStart = trigger.GetTriggerTime();
  double triggerStop = triggerStart + trigger.GetTriggerLength();

  // Check source of this TriggerKey
  TriggerKey::SourceID triggerSource = triggerKey.GetSource();

  // Calculate the readout windows
  double readoutStart = 0;
  double readoutStop = 0;
  bool sameSubdetector = ( (triggerSource == TriggerKey::IN_ICE) && 
			   ( (subdetector == I3TriggerStatus::INICE) || 
			     (subdetector == I3TriggerStatus::ALL) ) ) ||
                         ( (triggerSource == TriggerKey::ICE_TOP) && 
			   ( (subdetector == I3TriggerStatus::ICETOP) || 
			     (subdetector == I3TriggerStatus::ALL) ) );

  if (sameSubdetector) {
    // This readout is for the same subdetector that triggered
    readoutStart = triggerStart - readoutConfig.readoutTimeMinus;
    readoutStop = triggerStop + readoutConfig.readoutTimePlus;
  } else {
    // This readout is for another subdetector
    readoutStart = triggerStart + readoutConfig.readoutTimeOffset - readoutConfig.readoutTimeMinus;
    readoutStop = triggerStart + readoutConfig.readoutTimeOffset + readoutConfig.readoutTimePlus;
  }
  readoutWindow.first = readoutStart;
  readoutWindow.second = readoutStop;

  return readoutWindow;
}

