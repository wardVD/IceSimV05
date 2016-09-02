#ifndef PP_TEST_SOURCE_H_INCLUDED
#define PP_TEST_SOURCE_H_INCLUDED

#include <icetray/I3Module.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3Particle.h>
#include <simclasses/I3MMCTrack.h>

class PPTestSource : public I3Module
{

  int nPEs_;
  double first_;
  double last_;
  double emin_;
  double emax_;
  std::string icmapName_;
  std::string mcTreeName_;
  std::string mmcInfoName_;
  unsigned int counter_;

 public:

  PPTestSource(const I3Context& context);

  void Configure();

  void DAQ(I3FramePtr frame);

};

#endif
