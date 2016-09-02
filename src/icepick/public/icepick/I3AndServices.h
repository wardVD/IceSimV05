/**
    Copyright  (C) 2007
    The IceCube Collaboration
    @file I3OrServices.h
    @date January 2007
    @author Phil Roth <proth@icecube.umd.edu>
*/

#ifndef I3ANDSERVICES_H
#define I3ANDSERVICES_H

#include <string>
#include <vector>

#include "icetray/I3IcePick.h"
#include "icetray/I3Logging.h"
#include "icetray/I3Bool.h"

/**
 * @brief An I3IcePick similar to the I3AndFilter, except this pick checks for
 * other I3IcePicks in the context instead of I3Bools in the frame.
 */
class I3AndServices : public I3IcePick
{
public:
  explicit I3AndServices (const I3Context & context):I3IcePick (context)
  {
    AddParameter ("ServicesToAnd",
                  "A list of services in the context to AND together",
                  serviceNames_);
  }

  void Configure ()
  {
    GetParameter ("ServicesToAnd", serviceNames_);
    for (std::vector < std::string >::const_iterator iserv = serviceNames_.begin ();
         iserv != serviceNames_.end (); ++iserv)
      {
        I3IcePickPtr service = GetContext().Get < I3IcePickPtr > (*iserv);
        if (!service)
          log_fatal("(%s) context contains nothing at slot \"%s\"",
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
        if (services_[i]->SelectFrameInterface (frame)) // SO FAR SO GOOD
          {
            log_debug ("(%s) Service %s accepts this event.",
                       GetName().c_str (), serviceNames_[i].c_str ());
          }
        else                                            // REJECTED
          {
            log_debug ("(%s) Service %s does NOT accept this event.",
                       GetName().c_str (), serviceNames_[i].c_str ());
            return false;                               // REJECTED
          }
      }

    log_debug ("(%s) None of the %zd input services rejected this frame."
               "Keeping event.",
               GetName().c_str (), services_.size ());
    return true;                               // NOT REJECTED
  }

private:
  std::vector < std::string > serviceNames_;
  std::vector < I3IcePickPtr > services_;

  SET_LOGGER ("I3AndServices");
};

#endif /* I3ANDSERVICES_H */
