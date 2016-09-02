#ifndef I3NHITCHANNELFILTER_H
#define I3NHITCHANNELFILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief An I3IcePick which selects events which have
 * nHit above some threshold
 */
class I3NHitChannelFilter : public I3IcePick
{
 public:
  I3NHitChannelFilter(const I3Context& context);

  bool SelectFrame(I3Frame& frame);

  void Configure();

 private:
  std::string responseKey_;
  int minThreshold_;
  int maxThreshold_;
};

#endif
