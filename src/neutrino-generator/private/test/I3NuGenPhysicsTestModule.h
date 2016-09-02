#ifndef NUGEN_PHYSICS_TEST_MODULE_H_INCLUDED
#define NUGEN_PHYSICS_TEST_MODULE_H_INCLUDED

#include<icetray/I3Module.h>
#include<dataclasses/I3Map.h>
#include<vector>
#include<string>

class I3NuGenPhysicsTestModule : public I3Module
{

 public:

  I3NuGenTestModule(const I3Context& context);

  void Configure();

  void DAQ(I3FramePtr frame);

 private:
  std::string runmode_;
  int evtno_;

};

#endif
