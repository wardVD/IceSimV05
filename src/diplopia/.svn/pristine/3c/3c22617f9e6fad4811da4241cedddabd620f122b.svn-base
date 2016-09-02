#ifndef PP_WEIGHT_TEST_H_INCLUDED
#define PP_WEIGHT_TEST_H_INCLUDED

#include <icetray/I3Module.h>

class PPCheckWeights: public I3Module
{

  std::string mapName_;
  std::string mcTreeName_;
  std::string mmcInfoName_;
  std::string weight_name_;
  unsigned int counter_;

 public:

  PPCheckWeights(const I3Context& context);

  void Configure();

  void DAQ(I3FramePtr frame);

};

#endif
