/**
    Copyright  (C) 2007
    The IceCube Collaboration
    @file I3InverseServices.h
    @date January 2007
    @author E. Blaufuss
*/

#ifndef I3INVERSESERVICES_H
#define I3INVERSESERVICES_H

#include <string>
#include <vector>

#include "icetray/I3IcePick.h"
#include "icetray/I3Logging.h"
#include "icetray/I3Bool.h"

/**
 * @brief An I3IcePick similar to the I3InverseFilter, except this pick checks for
 * an I3IcePick in the context instead of the frame.
 */
class I3InverseServices : public I3IcePick
{
public:
  explicit I3InverseServices (const I3Context & context):I3IcePick (context)
  {
    AddParameter ("ServiceToInvert",
                  "The icepick service in the context to invert",
                  serviceName_);
  }

  void Configure ()
  {
    GetParameter ("ServiceToInvert", serviceName_);

    service_ = GetContext().Get < I3IcePickPtr > (serviceName_);
    if (!service_)
      log_fatal("(%s) context contains nothing at slot \"%s\"",
		GetName().c_str (), serviceName_.c_str ());

    if (serviceName_.empty())
      log_fatal ("(%s) no service specified", GetName().c_str ());
  }

  bool SelectFrame (I3Frame & frame)
  {
    bool decision = service_->SelectFrame (frame);

    log_debug ("(%s) Our input service said %i, return %i",
	       GetName().c_str(),
               decision, !decision);
    return !decision;                               // NOT REJECTED
  }

private:
  std::string serviceName_;
  I3IcePickPtr service_;

  SET_LOGGER ("I3InverseServices");
};

#endif /* I3INVERSESERVICES_H */
