#ifndef I3SOFTWARETRIGGERFLAGSFILTER_H
#define I3SOFTWARETRIGGERFLAGSFILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief An I3IcePick which selects events that have
 * certain F2k triggers set
 */
class I3SoftwareTriggerFlagsFilter : public I3IcePick
{
 public:

  I3SoftwareTriggerFlagsFilter(const I3Context& context) : I3IcePick(context)
    {
        triggerflagsname_ = "F2kSoftwareTriggerFlags";
        AddParameter("TriggerFlagsName","Name of the F2k Software trigger flags in the frame(default='F2kSoftwareTriggerFlags')",triggerflagsname_);
    
        triggers_to_filter_.clear();
        AddParameter("TriggersToFilter","Vector of Trigger names that are to be kept in the stream (default='')",triggers_to_filter_);
    }

  void Configure() {
    GetParameter("TriggerFlagsName",triggerflagsname_);
    GetParameter("TriggersToFilter",triggers_to_filter_);
  }

  bool SelectFrame(I3Frame& frame) {
    if(!frame.Has(triggerflagsname_))
    {
        log_warn("Frame didn't have trigger flags %s set, keeping it!",triggerflagsname_.c_str());
        return true; //true now means that event is kept!
    }
    else
    {
        const I3Vector<std::string>& triggerflags_ = frame.Get<I3Vector<std::string> > (triggerflagsname_);
        for (stditer_ = triggers_to_filter_.begin(); stditer_ < triggers_to_filter_.end(); stditer_++)
        {
            for (I3iter_ = triggerflags_.begin(); I3iter_ < triggerflags_.end(); I3iter_++)
            {
                if (*stditer_ == *I3iter_)
                {
                    log_trace("Keeping this event");
                    return true;
                }
            }
        }
        log_trace("Event discarded as none of the specified software trigger flags was present");
        return false;
    }
  }

 private:

  std::vector<std::string> triggers_to_filter_;
  std::string triggerflagsname_;

  std::vector<std::string>::const_iterator stditer_;
  I3Vector<std::string>::const_iterator I3iter_;
  SET_LOGGER("I3SoftwareTriggerFlagsFilter");
};

#endif
