#include "phys-services/empty-streams/I3EmptyGeometryService.h"

#include <dataclasses/geometry/I3Geometry.h>

I3EmptyGeometryService::~I3EmptyGeometryService()
{
}

I3EmptyGeometryService::I3EmptyGeometryService()
{
  geometry_ = I3GeometryPtr(new I3Geometry());
  geometry_->startTime = I3Time(0,0);
  geometry_->endTime = I3Time(3000,0);

}

I3GeometryConstPtr I3EmptyGeometryService::GetGeometry(I3Time time)
{
  return geometry_;
}
