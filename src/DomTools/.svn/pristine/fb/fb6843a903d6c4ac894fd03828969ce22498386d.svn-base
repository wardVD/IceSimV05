/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id$
 *
 * @file I3DOMLaunchCleaning.h
 * @version $Revision:$
 * @date $Date$
 */


#ifndef DOMTOOLS_I3DOMLAUNCHCLEANING_H_INCLUDED
#define DOMTOOLS_I3DOMLAUNCHCLEANING_H_INCLUDED

#include <icetray/I3ConditionalModule.h>
#include <icetray/OMKey.h>

class I3DOMLaunchCleaning : public I3ConditionalModule
{
 public:
  I3DOMLaunchCleaning(const I3Context& context);

  void Configure();

  void DAQ(I3FramePtr frame);

 private:
  std::vector<OMKey> cleanedKeys_;
  std::string cleanedKeysList_;

  std::string inIceInput_;
  std::string iceTopInput_;

  std::string inIceOutput_;
  std::string iceTopOutput_;


  bool firstLaunchCleaning_;
};

#endif
