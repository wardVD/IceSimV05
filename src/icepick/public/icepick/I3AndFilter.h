#ifndef I3ANDFILTER_H
#define I3ANDFILTER_H

#include <string>
#include <vector>

#include "icetray/I3IcePick.h"
#include "icetray/I3Logging.h"
#include "icetray/I3Bool.h"

/**
 * @brief An I3IcePick which rejects an event if it was rejected by any
 * one of a configurable list of previously run IcePick decisions.
 */
class I3AndFilter:public I3IcePick
{
public:
  explicit I3AndFilter (const I3Context & context):I3IcePick (context)
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
            if (decision->value) // SO FAR SO GOOD
              {
                log_debug ("(%s) Filter %s wants to keep the event.",
                           GetName().c_str (), idec->c_str ());
              }
            else                // REJECTED
              {
                log_debug ("(%s) Filter %s does NOT want to keep the event.",
                           GetName().c_str (), idec->c_str ());
                return false;   // REJECTED
              }
          }
        else                    // REJECTED
          {
            log_warn ("(%s) Filter \"%s\" NOT AVAILABLE in current frame: "
                      "rejecting event!",
                      GetName().c_str (), idec->c_str ());
            return false;      // REJECTED
          }
      }

    log_debug ("(%s) None of the %zd input filters rejected this frame."
               "Keeping event.",
               GetName().c_str (), inputDecisions_.size ());
    return true;                // NOT REJECTED
  }

private:
  std::vector < std::string > inputDecisions_;

  SET_LOGGER ("I3AndFilter");
};

#endif /* I3ANDFILTER_H */
