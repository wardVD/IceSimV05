using namespace boost::python;
namespace bp = boost:: python;

#include <recclasses/I3CscdLlhFitParams.h>
#include <tableio/converter/pybindings.h>
#include "../recclasses/converter/I3CscdLlhFitParamsConverter.h"

void register_I3CscdLlhFitParams()
{
  class_<I3CscdLlhFitParams, bases<I3FrameObject>, I3CscdLlhFitParamsPtr >("I3CscdLlhFitParams")
    .add_property("HitCount", &I3CscdLlhFitParams::GetHitCount, &I3CscdLlhFitParams::SetHitCount)
    .add_property("HitOmCount", &I3CscdLlhFitParams::GetHitOmCount, &I3CscdLlhFitParams::SetHitOmCount)
    .add_property("UnhitOmCount", &I3CscdLlhFitParams::GetUnhitOmCount, &I3CscdLlhFitParams::SetUnhitOmCount)
    .add_property("Status", &I3CscdLlhFitParams::GetStatus, &I3CscdLlhFitParams::SetStatus)
    .add_property("ErrT", &I3CscdLlhFitParams::GetErrT, &I3CscdLlhFitParams::SetErrT)
    .add_property("ErrX", &I3CscdLlhFitParams::GetErrX, &I3CscdLlhFitParams::SetErrX)
    .add_property("ErrY", &I3CscdLlhFitParams::GetErrY, &I3CscdLlhFitParams::SetErrY)
    .add_property("ErrZ", &I3CscdLlhFitParams::GetErrZ, &I3CscdLlhFitParams::SetErrZ)
    .add_property("ErrTheta", &I3CscdLlhFitParams::GetErrTheta, &I3CscdLlhFitParams::SetErrTheta)
    .add_property("ErrPhi", &I3CscdLlhFitParams::GetErrPhi, &I3CscdLlhFitParams::SetErrPhi)
    .add_property("ErrEnergy", &I3CscdLlhFitParams::GetErrEnergy, &I3CscdLlhFitParams::SetErrEnergy)
    .add_property("NegLlh", &I3CscdLlhFitParams::GetNegLlh, &I3CscdLlhFitParams::SetNegLlh)
    .add_property("ReducedLlh", &I3CscdLlhFitParams::GetReducedLlh, &I3CscdLlhFitParams::SetReducedLlh)
    ;
 
  register_pointer_conversions<I3CscdLlhFitParams>();

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3CscdLlhFitParamsConverter, "Dumps cscd-llh parameter objects");
}


