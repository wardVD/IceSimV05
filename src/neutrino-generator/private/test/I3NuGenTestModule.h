#ifndef NUGEN_TEST_MODULE_H_INCLUDED
#define NUGEN_TEST_MODULE_H_INCLUDED

#include<icetray/I3Module.h>
#include<dataclasses/I3Map.h>
#include<vector>
#include<string>

class I3NuGenTestModule : public I3Module
{

 public:

  I3NuGenTestModule(const I3Context& context);

  void Configure();

  void DAQ(I3FramePtr frame);

 private:

  void CheckPrimaryInfo(I3MapStringDouble &wmap);
  void CheckInteractionInfo(I3MapStringDouble &wmap);
  void CheckInteractionWeights(I3MapStringDouble &wmap);

  std::vector<double> nunubarratio_;
  std::string runmode_;
  int evtno_;

};

#endif
