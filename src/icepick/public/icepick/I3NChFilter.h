/**
 * copyright (C) 2007
 * the IceCube collaboration
 * $Id:$
 *
 * @file I3NChFilter.h
 * @version $Revision$
 * @author Anna Franckowiak
 * @date Oct 22 2007
 */

#ifndef I3NCHFILTER_H_INCLUDED
#define I3NCHFILTER_H_INCLUDED

class I3Context;
class I3Frame;


#include <string>

#include <icetray/I3IcePick.h>
#include <icetray/I3Logging.h>


/**
 * @brief An I3IcePick to select events with a minimum number of hit channels.
 * Events with less hit channels will be rejected.
 * Note that in general events with small nch have less energy than high nch events.
 */
class I3NChFilter : public I3IcePick
{
 public:
  explicit I3NChFilter(const I3Context& context);
  virtual ~I3NChFilter();
  void Configure();
  bool SelectFrame(I3Frame& frame);

 private:
  std::string cutsKey_;
  int nch_;
  
  
  // private copy constructor and assignment
  I3NChFilter(const I3NChFilter&);
  I3NChFilter& operator=(const I3NChFilter&);

 
  SET_LOGGER("I3NChFilter");
};

#endif // I3NCHFILTER_H_INCLUDED
