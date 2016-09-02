#ifndef I3NLAUNCHFILTER_H
#define I3NLAUNCHFILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief An I3IcePick which selects events which have
 * a certain number of hit DOMs
 */
class I3NLaunchFilter : public I3IcePick
{
  SET_LOGGER("I3NLauchFilterPick")
 public:
  I3NLaunchFilter(const I3Context& context);

  bool SelectFrame(I3Frame& frame);

  void Configure();

 private:
  std::string dataReadoutName_;
  int minNlaunch_;
  int maxNlaunch_;
  std::string countWhat_;
  enum count_enum{ COUNTDOMS, COUNTSTRINGS, COUNTTANKS, COUNTSTATIONS };
  std::map <std::string, count_enum> countWhatMap_;
  std::vector<int> strings_;
};

#endif
