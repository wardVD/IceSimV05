#include "phys-services/I3RandomService.h"
#include "neutrino-generator/legacy/I3NeutrinoPropagator.h"
#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "neutrino-generator/Steering.h"


namespace bp = boost::python;

void
register_I3NuGInteractionInfo()
{
    bp::class_<I3NuGInteractionInfo, I3NuGInteractionInfoPtr, boost::noncopyable>
        ("I3NuGInteractionInfo",
        bp::init<I3RandomServicePtr, nugen::SteeringPtr, const std::string &>())
        .def("initialize", &I3NuGInteractionInfo::Initialize)
        .def("view_interaction_info", &I3NuGInteractionInfo::ViewInteractionInfo)
        .def("get_xsec_cgs", &I3NuGInteractionInfo::GetXsecCGS)
    ;
}

void register_I3NeutrinoGenerator()
{
    bp::class_<I3NeutrinoPropagator, boost::shared_ptr<I3NeutrinoPropagator>, 
        bp::bases<I3PropagatorService>, 
        boost::noncopyable> 
        ("I3NeutrinoPropagator", bp::init<I3RandomServicePtr, 
             nugen::SteeringPtr, I3NuGInteractionInfoPtr>())
        .add_property("prop_mode", &I3NeutrinoPropagator::GetPropagationMode, &I3NeutrinoPropagator::SetPropagationMode)
        .def("propagate_in_earth", 
           &I3NeutrinoPropagator::PropagateInEarthWrapper,
           (bp::arg("particle"), bp::arg("frame")), "propagate")
    ;
}


