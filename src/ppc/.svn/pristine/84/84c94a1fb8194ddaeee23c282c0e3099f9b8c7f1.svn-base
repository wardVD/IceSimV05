#ifndef ADJEFF_H
#define ADJEFF_H

#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <simclasses/I3MCPE.h>

class AdjEff : public I3Module{

 public:
  SET_LOGGER("AdjEff");
  AdjEff(const I3Context& ctx);
  void Configure();
  void DAQ(I3FramePtr frame);

 private:

  double eff;
  std::vector<OMKey> set;
  std::map<OMKey, int> oms;
};

#endif //ADJEFF_H
