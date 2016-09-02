#ifndef I3EMPTYSTREAMSFACTORY_H
#define I3EMPTYSTREAMSFACTORY_H

#include <icetray/I3ServiceFactory.h>

class I3CalibrationService;
class I3DetectorStatusService;
class I3GeometryService;

class I3EmptyStreamsFactory : public I3ServiceFactory
{
 public:
  I3EmptyStreamsFactory(const I3Context& context);
  
  bool InstallService(I3Context& services);

  void Configure();
 private:
  int nframes_;

  boost::shared_ptr<I3CalibrationService> calibrations_;

  // plural of status is status.  But pronounced with a long u
  // so though this looks like 'status', it's pronounced 
  // statoooos
  boost::shared_ptr<I3DetectorStatusService> status_;
  boost::shared_ptr<I3GeometryService> geometries_;

  bool installCalibrations_;
  bool installStatus_;
  bool installGeometries_;
  int32_t eventTimeYr_;
  int64_t eventTimeNs_;
  unsigned eventRunNumber_;
};

#endif
