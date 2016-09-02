#ifndef I3EMPTYCALIBRATIONSERVICE_H
#define I3EMPTYCALIBRATIONSERVICE_H

#include "interfaces/I3CalibrationService.h"

class I3EmptyCalibrationService : public I3CalibrationService
{
 public:
  I3EmptyCalibrationService();

  virtual ~I3EmptyCalibrationService();
  
  I3CalibrationConstPtr GetCalibration(I3Time time);

 private:

  I3CalibrationPtr calibration_;
};

#endif
