/**
 * copyright (C) 2007
 * the IceCube collaboration
 * $Id:$
 *
 * @file I3DOMLaunchTriggerTypeFilter.h
 * @version $Revision$
 * @author Anna Franckowiak
 * @date Oct 22 2007
 */

#ifndef I3DOMLAUNCHITRIGGERTYPEFILTER_H_INCLUDED
#define I3DOMLAUNCHITRIGGERTYPEFILTER_H_INCLUDED

class I3Context;
class I3Frame;


#include <string>
#include <vector>

#include <icetray/I3IcePick.h>
#include <icetray/I3Logging.h>


/**
 * @brief An I3IcePick which selects events which do not include
 * DOM launches of certain trigger types.
 * 
 * \note
 * The I3 DAQ raw data decoder splits DOM launches of different trigger types
 * into different DOM launch series maps. This filter selects events by means
 * of the non-existence or emptiness of a set of these maps. 
 */
class I3DOMLaunchTriggerTypeFilter : public I3IcePick
{
 public:
  explicit I3DOMLaunchTriggerTypeFilter(const I3Context& context);
  virtual ~I3DOMLaunchTriggerTypeFilter();
  void Configure();
  bool SelectFrame(I3Frame& frame);

 private:   
  std::vector<std::string> ids_;

  // private copy constructor and assignment
  I3DOMLaunchTriggerTypeFilter(const I3DOMLaunchTriggerTypeFilter&);
  I3DOMLaunchTriggerTypeFilter& operator=(const I3DOMLaunchTriggerTypeFilter&);

 
  SET_LOGGER("I3DOMLaunchTriggerTypeFilter");
};

#endif // I3PHYSICSTRIGGERTYPEFILTER_H_INCLUDED
