#ifndef I3RECOPULSEFILTER_H
#define I3RECOPULSEFILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief An expandable I3IcePick that currently can pick 
 * on nhit/nchan or charge/nchan.  Other quantities based 
 * on an I3RecoPulseSeriesMap can easily be added.
 *
 */
class I3RecoPulseFilter : public I3IcePick
{
 public:
  I3RecoPulseFilter(const I3Context& context);

  bool SelectFrame(I3Frame& frame);

  void Configure();

 private:
  std::string frameKey_;
  float minhitperchan_;
  float maxhitperchan_;
  float minchargeperchan_;
  float maxchargeperchan_;
  
  SET_LOGGER("I3RecoPulseFilter");
};

#endif
