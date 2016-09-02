#include <recclasses/I3CLastFitParams.h>
#include <tableio/converter/pybindings.h>
#include "../recclasses/converter/I3CLastFitParamsConverter.h"
#include "wrap.h"

void register_I3CLastFitParams()
{
  using namespace boost::python;

  wrap::I3FrameObject<I3CLastFitParams>()
    .def_readwrite("mineval", &I3CLastFitParams::mineval)
    .def_readwrite("evalratio", &I3CLastFitParams::evalratio)
    .def_readwrite("eval2", &I3CLastFitParams::eval2)
    .def_readwrite("eval3", &I3CLastFitParams::eval3)
    ;

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3CLastFitParamsConverter,
                 "Dumps I3CLastFitParams parameter objects");
}
