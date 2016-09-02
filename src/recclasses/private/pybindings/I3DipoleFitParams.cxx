#include "recclasses/I3DipoleFitParams.h"
#include "../recclasses/converter/I3DipoleFitParamsConverter.h"
#include "wrap.h"
#include <tableio/converter/pybindings.h>
#include <boost/python.hpp>

void register_I3DipoleFitParams()
{
  using namespace boost::python;

  wrap::I3FrameObject<I3DipoleFitParams>()
    .def_readwrite("Magnet", &I3DipoleFitParams::magnet)
    .def_readwrite("MagnetX", &I3DipoleFitParams::magnetX)
    .def_readwrite("MagnetY", &I3DipoleFitParams::magnetY)
    .def_readwrite("MagnetZ", &I3DipoleFitParams::magnetZ)
    .def_readwrite("AmpSum", &I3DipoleFitParams::ampSum)
    .def_readwrite("NHits", &I3DipoleFitParams::nHits)
    .def_readwrite("NPairs", &I3DipoleFitParams::nPairs)
    .def_readwrite("MaxAmp", &I3DipoleFitParams::maxAmp)
    ;

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3DipoleFitParamsConverter,"Dumps I3DipoleFitParams parameter objects");
}
