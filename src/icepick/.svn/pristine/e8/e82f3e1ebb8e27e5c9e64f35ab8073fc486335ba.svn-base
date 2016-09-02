#ifndef I3_FIT_SUCCEEDED_FILTER_H
#define I3_FIT_SUCCEEDED_FILTER_H

#include "icetray/I3IcePick.h"
#include "dataclasses/physics/I3Particle.h"

/**
 * @brief An IcePick that keeps an event with a 
 *  certain fit that has FitStatus = OK (or NotSet).
 */
class I3FitSucceededFilter : public I3IcePick {

public:
  I3FitSucceededFilter(const I3Context& context) : I3IcePick(context) {
    fit_ = "";
    AddParameter("Fitname", "The fit to be checked.", fit_);
    keep_if_failed_ = 0;
    AddParameter("DidNotSucceed", "Keep the events where the fit did NOT succeed?", 
		 keep_if_failed_);
  }

  void Configure() {
    GetParameter("Fitname", fit_);
    GetParameter("DidNotSucceed", keep_if_failed_);
  }

  bool SelectFrame(I3Frame& frame) {

    bool goodevent = false;
    if (frame.Has(fit_)) {
      const I3Particle& thefit = frame.Get<I3Particle>(fit_);
      
      int status = thefit.GetFitStatus();
      goodevent = (status == I3Particle::OK || status == I3Particle::NotSet);
      log_trace("Status = %d... is it a good event? %d", status, goodevent);
      
    } else {
      goodevent = false;
      log_info("Fit of name %s does not exist. Classifying as a bad fit.", fit_.c_str());
    }

    if (keep_if_failed_) {
      log_debug("Reversing the boolean.");
      return !goodevent;
    }
    return goodevent;
 
  }

private:
  std::string fit_;
  bool keep_if_failed_;

  SET_LOGGER("I3FitSucceededFilter");
};

#endif
