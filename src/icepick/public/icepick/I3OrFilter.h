/**
    Copyright  (C) 2006
    The IceCube Collaboration
    @file I3OrFilter.h
    @date September 2006
    @author David Boersma <boersma@icecube.wisc.edu>
*/

#ifndef I3ORFILTER_H
#define I3ORFILTER_H

#include <string>
#include <vector>

#include "icetray/I3IcePick.h"
#include "icetray/I3Logging.h"
#include "icetray/I3Bool.h"

/**
 * @brief An I3IcePick which keeps an event if it was kept by any
 * one of a configurable list of previously run IcePick decisions.
 * 
 * It's a generalization of the I3LogicalOrFilter, which takes exactly
 * two input decision. This class takes arbitrarily many.
 */
class I3OrFilter:public I3IcePick
{
public:
  explicit I3OrFilter (const I3Context & context):I3IcePick (context)
  {
    AddParameter ("InputDecisions",
                  "The list of frame object names of previous IcePick decisions.",
                  inputDecisions_);
  }

  void Configure ()
  {
    GetParameter ("InputDecisions", inputDecisions_);
    if (inputDecisions_.empty ())
      log_fatal ("(%s) no input decision specified", GetName().c_str ());
  }

  bool SelectFrame (I3Frame & frame)
  {
    std::vector < std::string >::const_iterator idec;
    for (idec = inputDecisions_.begin ();
         idec != inputDecisions_.end (); ++idec)
      {
        I3BoolConstPtr decision = frame.Get < I3BoolConstPtr > (*idec);
        if (decision)
          {
            if (decision->value) // NOT REJECTED
              {
                log_debug ("(%s) Filter %s wants to keep the event.",
                           GetName().c_str (), idec->c_str ());
                return true;     // NOT REJECTED
              }
            else
              {
                log_debug ("(%s) Filter %s does NOT want to keep the event.",
                           GetName().c_str (), idec->c_str ());
              }
          }
        else
          {
            log_warn ("(%s) Filter \"%s\" NOT AVAILABLE in current frame.",
                      GetName().c_str (), idec->c_str ());
          }
      }

    log_debug ("(%s) None of the %zd filters likes this frame."
               "Filtering event.",
               GetName().c_str (), inputDecisions_.size ());
    return false;
  }

private:
  std::vector < std::string > inputDecisions_;

  SET_LOGGER ("I3OrFilter");
};

#endif /* I3ORFILTER_H */
