
#include "DOMLauncher/InterpolatedSPETemplate.h"
#include <boost/python.hpp>


#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/status/I3DetectorStatus.h"

//using namespace boost::python;
namespace bp=boost::python;
void register_InterpolatedSPETemplate()
{
  {
    bp::scope interpolated_spe_template_scope =
    bp::class_<InterpolatedSPETemplate, boost::shared_ptr<InterpolatedSPETemplate> >
      ("InterpolatedSPETemplate","DocString",
       bp::init<const I3DOMCalibration::DroopedSPETemplate>
       (bp::args("DroppedSPETemplate"), "__init__ docstring"))
      .def("tabulate",&InterpolatedSPETemplate::Tabulate)
      .def("__call__",&InterpolatedSPETemplate::operator())
      .def("tabulation_error",&InterpolatedSPETemplate::TabulationError)
      .def("set_template",&InterpolatedSPETemplate::SetTemplate);
  }
}
