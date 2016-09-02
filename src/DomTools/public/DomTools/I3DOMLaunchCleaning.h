/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id: I3DOMLaunchCleaning.h 75718 2011-05-25 20:40:52Z jvansanten $
 *
 * @file I3DOMLaunchCleaning.h
 * @version $Revision:$
 * @date $Date: 2011-05-25 15:40:52 -0500 (Wed, 25 May 2011) $
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
