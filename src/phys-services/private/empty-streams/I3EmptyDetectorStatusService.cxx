#include "phys-services/empty-streams/I3EmptyDetectorStatusService.h"

#include <dataclasses/status/I3DetectorStatus.h>

I3EmptyDetectorStatusService::~I3EmptyDetectorStatusService()
{
}

I3EmptyDetectorStatusService::I3EmptyDetectorStatusService()
{
  status_ = I3DetectorStatusPtr(new I3DetectorStatus());
  status_->startTime = I3Time(0,0);
  status_->endTime = I3Time(3000,0);

}

I3DetectorStatusConstPtr 
I3EmptyDetectorStatusService::GetDetectorStatus(I3Time time)
{
  return status_;
}
