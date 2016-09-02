#ifndef READOUT_WINDOW_UTIL_H
#define READOUT_WINDOW_UTIL_H
/**
 * class: ReadoutWindowUtil
 *
 * Version $Id: $
 *
 * date: $Date: $
 *
 * @author toale
 *
 * (c) 2006 IceCube Collaboration
 */

#include "dataclasses/physics/I3Trigger.h"
#include "dataclasses/TriggerKey.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/status/I3TriggerStatus.h"

/**
 * @brief Utility class to provide methods to retrieve readout windows
 *        for various trigger conditions.  It's more complicate than you
 *        may think.
 */
class ReadoutWindowUtil
{

 public:

  ReadoutWindowUtil(const I3DetectorStatus& detectorStatus);
  ~ReadoutWindowUtil();

  std::pair<double,double> GetInIceReadoutWindow(const I3Trigger& trigger);
  std::pair<double,double> GetIceTopReadoutWindow(const I3Trigger& trigger);

  std::pair<double,double> GetReadoutWindow(I3TriggerStatus::Subdetector subdetector, 
					    const I3Trigger& trigger);

  double GetEarliestReadoutTime(const I3Trigger& trigger);
  double GetLatestReadoutTime(const I3Trigger& trigger);

 private:

  std::map<TriggerKey, I3TriggerStatus> triggerStatus_;

};

#endif // READOUT_WINDOW_UTIL_H
