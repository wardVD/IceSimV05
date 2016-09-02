#include "recclasses/I3TensorOfInertiaFitParams.h"
#include "../recclasses/converter/I3TensorOfInertiaFitParamsConverter.h"
#include <tableio/converter/pybindings.h>
#include "wrap.h"
#include <boost/python.hpp>

void register_I3TensorOfInertiaFitParams()
{
  using namespace boost::python;

  wrap::I3FrameObject<I3TensorOfInertiaFitParams>()
    .def_readwrite("mineval", &I3TensorOfInertiaFitParams::mineval)
    .def_readwrite("evalratio", &I3TensorOfInertiaFitParams::evalratio)
    .def_readwrite("eval2", &I3TensorOfInertiaFitParams::eval2)
    .def_readwrite("eval3", &I3TensorOfInertiaFitParams::eval3)
    ;

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3TensorOfInertiaFitParamsConverter,
           "Dumps parameters objects of tensor-of-inertia (eigenvalues..)");
}
