#include "phys-services/empty-streams/I3EmptyStreamsFactory.h"

#include "phys-services/empty-streams/I3EmptyGeometryService.h"
#include "phys-services/empty-streams/I3EmptyDetectorStatusService.h"
#include "phys-services/empty-streams/I3EmptyCalibrationService.h"
#include "dataclasses/I3Time.h"

I3_SERVICE_FACTORY(I3EmptyStreamsFactory);

I3EmptyStreamsFactory::I3EmptyStreamsFactory(const I3Context& context) : 
  I3ServiceFactory(context), 
  nframes_(10),
  installCalibrations_(true),
  installStatus_(true),
  installGeometries_(true),
  eventTimeYr_(2006),
  eventTimeNs_(0),
  eventRunNumber_(0)
{
  log_trace("constructing I3EmptyStreamsFactory");
  AddParameter("NFrames","Number of event frames to spit out",nframes_);
  AddParameter("InstallCalibration",
	       "Whether or not to install the Calibration Service",
	       installCalibrations_);
  AddParameter("InstallGeometry",
	       "Whether or not to install the Geometry Service",
	       installGeometries_);
  AddParameter("InstallStatus",
	       "Whether or not to install the DetectorStatus Service",
	       installStatus_);
  AddParameter("EventTimeYear",
	       "Specify the time (year) to use for events installed",
	       eventTimeYr_);
  AddParameter("EventTimeNnanosec",
	       "Specify the time (N ns) to use for events installed",
	       eventTimeNs_);
  AddParameter("EventRunNumber",
	       "Specify the run number to use for events installed",
	       eventRunNumber_);
}

void I3EmptyStreamsFactory::Configure()
{
  log_trace("Configuring I3EmptyStreamsFactory");

  GetParameter("NFrames",nframes_);

  GetParameter("InstallCalibration",
	       installCalibrations_);
  GetParameter("InstallGeometry",
	       installGeometries_);
  GetParameter("InstallStatus",
	       installStatus_);
  GetParameter("EventTimeYear",
	       eventTimeYr_);
  GetParameter("EventTimeNNanosec",
	       eventTimeNs_);
  GetParameter("EventRunNumber",
	       eventRunNumber_);

  if(installCalibrations_)
    calibrations_ = 
      boost::shared_ptr<I3CalibrationService>(new I3EmptyCalibrationService());
  if(installStatus_)
    status_ = 
      boost::shared_ptr<I3DetectorStatusService>(new I3EmptyDetectorStatusService());
  if(installGeometries_)
    geometries_ = 
      boost::shared_ptr<I3GeometryService>(new I3EmptyGeometryService());
}

bool I3EmptyStreamsFactory::InstallService(I3Context& services)
{
  bool success = true;

  if(installCalibrations_)
    success *=
      services.Put<I3CalibrationService>(calibrations_);

  if(installStatus_)
    success *= 
      services.Put<I3DetectorStatusService>(status_);

  if(installGeometries_)
    success *= 
      services.Put<I3GeometryService>(geometries_);

  return success;

}
