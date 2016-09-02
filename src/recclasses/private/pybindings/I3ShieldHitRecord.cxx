#include "recclasses/I3ShieldHitRecord.h"
#include "../recclasses/converter/I3ShieldHitRecordConverter.h"
#include <tableio/converter/I3VectorConverter.h>
#include <tableio/converter/pybindings.h>
#include "wrap.h"
#include <boost/python.hpp>

void register_I3ShieldHitRecord()
{
  using namespace boost::python;
  
  wrap::I3FrameObject<I3ShieldHitRecord>()
    .add_property("time_residual", &I3ShieldHitRecord::GetTimeResidual, &I3ShieldHitRecord::SetTimeResidual)
    .add_property("distance", &I3ShieldHitRecord::GetDistance, &I3ShieldHitRecord::SetDistance)
    .add_property("DOMkey", &I3ShieldHitRecord::GetDOMkey, &I3ShieldHitRecord::SetDOMkey)
  ;
  
  wrap::I3Vector<I3ShieldHitRecord>("I3VectorShieldHitRecord");
  
  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3ShieldHitRecordConverter, "Converts an I3ShieldHitRecord");
  I3CONVERTER_EXPORT_DEFAULT(I3VectorConverter<I3ShieldHitRecordConverter>, "Converts a vector of I3ShieldHitRecords");
}
