#ifndef I3GLOBALTRIGGERSIM_H
#define I3GLOBALTRIGGERSIM_H
/**
 * class: I3GlobalTriggerSim
 *
 * Version $Id: $
 *
 * date: $Date: $
 *
 * @author wikstrom
 * @author: Seon-Hee Seo
 *
 * (c) 2006 IceCube Collaboration
 */

#include <string>

#include <boost/optional.hpp>

#include <icetray/I3Module.h>
#include <dataclasses/I3Time.h>

class I3GlobalTriggerSim : public I3Module
{

  I3GlobalTriggerSim();
  I3GlobalTriggerSim(const I3GlobalTriggerSim&);
  I3GlobalTriggerSim& operator=(const I3GlobalTriggerSim&);

public:

  I3GlobalTriggerSim(const I3Context&);
  
  ~I3GlobalTriggerSim() {};

  void Configure();

  void DAQ(I3FramePtr frame);

  void Finish(){};

private:

  //Class parameters
  int eventID_;
  std::string i3TriggName_;
  std::string globalTriggName_;
  bool filterMode_;
  double i3ReadoutWindowBefore_;
  double i3ReadoutWindowAfter_;
  double i3ReadoutWindowOffset_;
  std::vector<std::string> domLaunchSeriesMapNames_;
  double time_increment_;
  int run_id_;

  // Internal time shift.  This is incremented by
  // time_increment_ on each frame.
  double dt_;

  boost::optional<std::pair<I3Time, I3Time> > time_range_;

  void PushIf(bool triggerCondition, I3FramePtr frame);

  SET_LOGGER("I3GlobalTriggerSim");
};

inline void I3GlobalTriggerSim::PushIf(bool triggerCondition, I3FramePtr frame){
  // if the trigger condition is fulfilled then we always push the frame
  // if the trigger condition is not fulfilled then we push the frame
  // only when filterMode_ is set to false.
  if( triggerCondition || !filterMode_)
    PushFrame(frame,"OutBox");
}

#endif //I3GLOBALTRIGGERSIM_H
