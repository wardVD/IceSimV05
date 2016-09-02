/**
 * copyright  (C) 2007
 * the IceCube collaboration
 * Version $Id: $
 *
 * @file I3LCCleaning.h
 * @version $Revision:$
 * @date $Date: $
 * @author: wikstrom
 */


#ifndef DOMTOOLS_I3LCCLEANING_H_INCLUDED
#define DOMTOOLS_I3LCCLEANING_H_INCLUDED

#include <icetray/I3ConditionalModule.h>
#include <icetray/OMKey.h>
/**
 * This module splits an I3DOMLaunchSeriesMap into two maps.  
 * One of which contains only HLC launches, the other of which 
 * contains only SLC launches.
 */
class I3LCCleaning : public I3ConditionalModule
{
 public:
  I3LCCleaning(const I3Context& context);

  void Configure();

  void DAQ(I3FramePtr frame);

  std::string inIceInput_;
  std::string inIceOutput_;
  std::string inIceOutputSLC_;
  
  /// this is set if inIceOutputSLC_ is not empty; default is false
  bool writeSLCOutput_;
};

#endif
