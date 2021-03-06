/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id: I3DetectorStatus.h 135122 2015-07-14 18:54:17Z jtatar $
 * @file I3DetectorStatus.h
 * @date $Date: 2015-07-14 13:54:17 -0500 (Tue, 14 Jul 2015) $
 */

#ifndef I3DETECTORSTATUS_H_INCLUDED
#define I3DETECTORSTATUS_H_INCLUDED

#include <map>

#include <icetray/I3DefaultName.h>
#include <icetray/I3FrameObject.h>
#include <dataclasses/I3Time.h>
#include <icetray/OMKey.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/Utility.h>
#include <dataclasses/status/I3DOMStatus.h>
#include <dataclasses/status/I3TriggerStatus.h>

/**
 * @brief This is the state of the aspects of the detector that people have 
 * direct control over.  Contains the "per run" settings.
 *
 * Stuff that is a 'knob' on the detector.  This is a
 * top-level object in the frame related to this 'Detector Status' information.
 * Contains: 
 * - map of per DOM configurations (also the list of active DOMs),
 * - map of per AOM configurations (including AOMs read out by the TWR DAQ),
 * - map of active icecube triggers (and their configurations),
 * - map of active amanda triggers (and their configurations) and
 * - map of active domhubs (and their settings) ... eventually.
 */
static const unsigned i3detectorstatus_version_ = 4;

class I3DetectorStatus : public I3FrameObject
{
public:
  I3Time startTime;
  I3Time endTime;

  I3DOMStatusMap domStatus;
  I3TriggerStatusMap triggerStatus;
  std::string daqConfigurationName; // is this used?

  I3DetectorStatus() {}
  
  ~I3DetectorStatus();

  bool operator==(const I3DetectorStatus& rhs)
  {
    return (startTime == rhs.startTime &&
            endTime == rhs.endTime &&
            domStatus == rhs.domStatus &&
            triggerStatus == rhs.triggerStatus &&
            daqConfigurationName == rhs.daqConfigurationName);
  }
  bool operator!=(const I3DetectorStatus& rhs)
  {
    return !operator==(rhs);
  }

private:
  friend class boost::serialization::access;  
  template <class Archive> void load(Archive & ar, unsigned version);
  template <class Archive> void save(Archive & ar, unsigned version) const;
  BOOST_SERIALIZATION_SPLIT_MEMBER();
};

BOOST_CLASS_VERSION(I3DetectorStatus, i3detectorstatus_version_);
I3_DEFAULT_NAME(I3DetectorStatus);
I3_POINTER_TYPEDEFS(I3DetectorStatus);

#endif /*I3DETECTORSTATUS_H_INCLUDED*/

