#ifndef I3_UNFILTER_H
#define I3_UNFILTER_H
#include "icetray/I3IcePick.h"

/**
 * @brief A dumb filter which just selects all the events
 */
class I3UnFilter : public I3IcePick
{
 public:
  I3UnFilter(const I3Context& context) : I3IcePick(context){}
  bool SelectFrame(I3Frame&){return true;}
};
#endif
