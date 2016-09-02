#ifndef PPC_H
#define PPC_H

#include <icetray/I3Frame.h>
#include <icetray/I3ConditionalModule.h>
#include <simclasses/I3MMCTrack.h>
#include <simclasses/I3MCPE.h>
#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>


class i3ppc : public I3ConditionalModule{

 public:
  SET_LOGGER("i3ppc");
  i3ppc(const I3Context &ctx);
  ~i3ppc();
  void Configure();
  void DetectorStatus(I3FramePtr frame);
  void DAQ(I3FramePtr frame);
  void Finish();

 private:

  bool ini, verbose;
  std::map<std::pair<int, unsigned long long>, const I3MMCTrack *> i3mmctracks;
  void pparticle(I3MCTreeConstPtr, I3MCTree::const_iterator);

  bool cyl;
  bool keep_;
  bool isinside(double, double, double);
  bool isinside(double, double, double, double, double, double, double);
  int iType(const I3Particle&);

  int gpu;
  std::vector<OMKey> bad;
  OMKey fla;
  double nph;
  double wid;
  std::string mct;
  int fb, fe;
  std::deque<I3FramePtr> frames;
  void popframes(int);
  void pushframe();
  std::map<int, I3MCPESeriesMapPtr> mcpes;

  double Radius, Top, Bottom, Padding;
};

#endif //PPC_H
