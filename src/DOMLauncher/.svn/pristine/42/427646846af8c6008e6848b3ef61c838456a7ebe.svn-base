#include "phys-services/I3RandomService.h"
#include "DOMLauncher/I3InIceDOM.h"
#include <boost/python.hpp>

using namespace std;
namespace bp=boost::python;
void register_I3InIceDOM()
{
  bp::scope i3inicedom_object_scope =
    bp::class_<I3InIceDOM, boost::shared_ptr<I3InIceDOM>, bp::bases<I3DOM> >
    ("I3InIceDOM","DocString")
    .def(bp::init<I3RandomServicePtr, const OMKey&>())
    .def("configure", &I3InIceDOM::Configure)
    .def("create_lc_links",&I3InIceDOM::CreateLCLinks)
    ;
}
