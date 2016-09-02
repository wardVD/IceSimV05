/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3FrameObjectFilter.h 129109 2015-02-15 17:32:49Z tschmidt $
 *
 * @file 
 * @version $Revision: 129109 $
 * @date $Date: 2015-02-15 11:32:49 -0600 (Sun, 15 Feb 2015) $
 * @author kjmeagher
 */
#ifndef I3FRAMEOBJECTFILTER_H
#define I3FRAMEOBJECTFILTER_H

// forward declarations

class I3Context;
class I3Frame;

// header files

#include <string>

#include <icetray/I3IcePick.h>
#include <icetray/I3Logging.h>

// definitions


/**
 * @brief An I3IcePick which selects frames that include an object
 * with a given name or an object with a name that starts with a given string.
 */
class I3FrameObjectFilter : public I3IcePick
{
 public:

  I3FrameObjectFilter(const I3Context& context);
  void Configure();
  bool SelectFrame(I3Frame& frame);

 private:
  std::string frame_object_key_;
  std::string frame_object_key_starts_with_;
  bool display_warning_;

  SET_LOGGER("I3FrameObjectFilter");
};

#endif

