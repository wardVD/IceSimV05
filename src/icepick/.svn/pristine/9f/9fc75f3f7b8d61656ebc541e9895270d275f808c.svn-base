/**
    Copyright  (C) 2007
    The IceCube Collaboration
    @file I3OrServices.h
    @date January 2007
    @author Phil Roth <proth@icecube.umd.edu>
*/

#ifndef I3ORSERVICES_H
#define I3ORSERVICES_H

#include <string>
#include <vector>

#include "icetray/I3IcePick.h"
#include "icetray/I3Logging.h"
#include "icetray/I3Bool.h"

/**
 * @brief An I3IcePick similar to the I3OrFilter, except this pick checks for
 * other I3IcePicks in the context instead of I3Bools in the frame.
 */
class I3OrServices : public I3IcePick
{
public:
  explicit I3OrServices (const I3Context & context):I3IcePick (context)
  {
    AddParameter ("ServicesToOr",
                  "A list of services in the context to OR together",
                  serviceNames_);
  }

  void Configure ()
  {
    GetParameter ("ServicesToOr", serviceNames_);
    for (std::vector < std::string >::const_iterator iserv = serviceNames_.begin ();
         iserv != serviceNames_.end (); ++iserv)
      {
        I3IcePickPtr service = GetContext().Get < I3IcePickPtr > (*iserv);
        if (!service)
          log_fatal("(%s)  context contains nothing at slot \"%s\"",
                    GetName().c_str (), iserv->c_str ());
        services_.push_back(service);
      }
    if (services_.empty())
      log_fatal ("(%s) no service specified", GetName().c_str ());
  }

  bool SelectFrame (I3Frame & frame)
  {
    for (unsigned int i = 0; i < services_.size (); ++i)
      {
        if (services_[i]->SelectFrameInterface (frame)) // NOT REJECTED
          {
            log_debug ("(%s) Service %s wants to accept the event.",
                       GetName().c_str (), serviceNames_[i].c_str ());
            return true;                                // NOT REJECTED
          }
        else
          {
            log_debug ("(%s) Service %s does NOT want to accept the event.",
                       GetName().c_str (), serviceNames_[i].c_str ());
          }
      }

    log_debug ("(%s) None of the %zd services likes this frame."
               "Rejecting event.",
               GetName().c_str (), services_.size ());
    return false;
  }

private:
  std::vector < std::string > serviceNames_;
  std::vector < I3IcePickPtr > services_;

  SET_LOGGER ("I3OrServices");
};

#endif /* I3ORSERVICES_H */
