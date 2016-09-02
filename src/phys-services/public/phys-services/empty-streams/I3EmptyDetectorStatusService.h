#ifndef I3EMPTYDETECTORSTATUSSERVICE_H
#define I3EMPTYDETECTORSTATUSSERVICE_H

#include "interfaces/I3DetectorStatusService.h"

class I3EmptyDetectorStatusService : public I3DetectorStatusService
{
 public:
  I3EmptyDetectorStatusService();

  virtual ~I3EmptyDetectorStatusService();

  I3DetectorStatusConstPtr GetDetectorStatus(I3Time time);

 private:

  I3DetectorStatusPtr status_;
};

#endif
