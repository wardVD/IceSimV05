#ifndef I3EMPTYGEOMETRYSERVICE_H
#define I3EMPTYGEOMETRYSERVICE_H

#include "interfaces/I3GeometryService.h"

class I3EmptyGeometryService : public I3GeometryService
{
 public:
  I3EmptyGeometryService();

  virtual ~I3EmptyGeometryService();
  
  I3GeometryConstPtr GetGeometry(I3Time time);

 private:

  I3GeometryPtr geometry_;
};

#endif
