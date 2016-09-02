#include "phys-services/empty-streams/I3EmptyCalibrationService.h"

#include <dataclasses/calibration/I3Calibration.h>

I3EmptyCalibrationService::~I3EmptyCalibrationService()
{
}

I3EmptyCalibrationService::I3EmptyCalibrationService()
{
  calibration_ = I3CalibrationPtr(new I3Calibration());
  calibration_->startTime = I3Time(0,0);
  calibration_->endTime = I3Time(3000,0);

}

I3CalibrationConstPtr I3EmptyCalibrationService::GetCalibration(I3Time time)
{
  return calibration_;
}
