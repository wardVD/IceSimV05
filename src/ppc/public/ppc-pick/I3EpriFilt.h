#ifndef I3EPRIFILT_H
#define I3EPRIFILT_H

#include <icetray/I3IcePick.h>

/**
 * @brief An I3IcePick which selects events with primary energy less than maxE_.
 */
class I3EpriFilt : public I3IcePick {
 public:

  I3EpriFilt(const I3Context& context);
  void Configure();
  bool SelectFrame(I3Frame& frame);

 private:
  double maxE_;

  SET_LOGGER("I3EpriFilt");
};

#endif
