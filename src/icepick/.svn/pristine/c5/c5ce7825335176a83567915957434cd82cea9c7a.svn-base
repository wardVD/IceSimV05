#ifndef I3ZENITHFILTER_H
#define I3ZENITHFILTER_H

#include <icetray/I3IcePick.h>

/**
 * @brief An I3IcePick which selects events which have
 * a particle with zenith angle greater than some parameter.
 */
class I3ZenithFilter : public I3IcePick
{
 public:

  I3ZenithFilter(const I3Context& context);
  void Configure();
  bool SelectFrame(I3Frame& frame);

 private:
  std::string particleKey_;
  double minZenith_;
  double maxZenith_;

  SET_LOGGER("I3ZenithFilter");
};

#endif
