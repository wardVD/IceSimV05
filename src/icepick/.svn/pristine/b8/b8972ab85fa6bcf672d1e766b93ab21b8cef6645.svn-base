/**
    Copyright  (C) 2006
    The IceCube Collaboration
    $Id$
    @version 00-00-01
    @date $Date$
    @author Seon-Hee Seo <shseo@phys.psu.edu>
*/

#ifndef I3PICKRAWNHITEVENTFILTER_H
#define I3PICKRAWNHITEVENTFILTER_H

#include "icetray/I3IcePick.h"
#include <icetray/I3FrameObject.h>
#include "dataclasses/Utility.h"

/**
 * @brief A complex filter to filter on the number of launches
 *  choosing Icetop/InIce/both, high/low/both thresholds, and
 *  setting the readout name(s) and thresholds
 */

class I3PickRawNHitEventFilter : public I3IcePick
{
 public:
  I3PickRawNHitEventFilter(const I3Context& context);

  bool SelectFrame(I3Frame& frame);

  void Configure();

private:
  int domLaunchSource_;
  int hitThresholdType_;
  int hitThresholdHigh_;
  int hitThresholdLow_;

  std::string iniceDOMLaunchSeriesMapName_;
  std::string icetopDOMLaunchSeriesMapName_;
  
};

#endif
