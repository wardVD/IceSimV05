#include "phys-services/I3RandomService.h"
#include "DOMLauncher/I3IceTopDOM.h"
#include <boost/python.hpp>

using namespace std;
namespace bp=boost::python;
void register_I3IceTopDOM()
{
  bp::scope i3inicedom_object_scope =
    bp::class_<I3IceTopDOM, boost::shared_ptr<I3IceTopDOM>, bp::bases<I3DOM> >
    ("I3IceTopDOM","DocString")
    .def(bp::init<I3RandomServicePtr, const OMKey&>())
    .def("configure", &I3IceTopDOM::Configure)
    .def("create_lc_links",&I3IceTopDOM::CreateLCLinks)
    ;
}
