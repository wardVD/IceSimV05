#ifndef NUGEN_TEST_MODULE_H_INCLUDED
#define NUGEN_TEST_MODULE_H_INCLUDED

#include<icetray/I3Module.h>

class NuGenTestModule : public I3Module
{

 private:

  std::string flavorString_;

  unsigned nInIceParticles_;

 public:

  NuGenTestModule(const I3Context& context);

  void Configure();

  void DAQ(I3FramePtr frame);

  void Finish();

};

#endif
