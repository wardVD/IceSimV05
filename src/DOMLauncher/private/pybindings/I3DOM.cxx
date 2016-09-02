#include "DOMLauncher/I3DOM.h"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

namespace bp=boost::python;
void register_I3DOM()
{

  std::vector<dlud::DiscCross>& (I3DOM::*get_triggers)() = &I3DOM::GetTriggers;
  std::vector<I3DOMLaunch>& (I3DOM::*get_domlaunches)() = &I3DOM::GetDOMLaunches;
  std::vector<I3DOM* >& (I3DOM::*get_neighbors)() = &I3DOM::GetNeighbors;

  bp::class_<I3DOM, boost::shared_ptr<I3DOM>, boost::noncopyable>("I3DOM", bp::no_init)
    .def("configure",&I3DOM::Configure)
    .def("reset",&I3DOM::Reset)
    .def("discriminator",&I3DOM::Discriminator)
    .def("trigger_launch",&I3DOM::TriggerLaunch)
    .def("add_trigger",&I3DOM::AddTrigger)
    .def("get_omkey",&I3DOM::GetOMKey, bp::return_internal_reference<>())
    .def("get_domlaunches",get_domlaunches, bp::return_internal_reference<>())
    .def("get_triggers",get_triggers, bp::return_internal_reference<>())
    .def("get_neighbors",get_neighbors, bp::return_internal_reference<>())
    .def("use_drooped_template",&I3DOM::SetUseDroopedTemplate)
    .def_readonly("discriminator_threshold_fraction", &I3DOM::GetDiscriminatorThresholdFraction)
    .def_readonly("discriminator_threshold", &I3DOM::GetDiscriminatorThreshold)
    .add_property("rapcal_time_shift",&I3DOM::GetRapCalTimeShift, &I3DOM::SetRapCalTimeShift)
    .add_static_property("merge_pulses",&I3DOM::GetMergePulses,
                         &I3DOM::SetMergePulses)
    .add_static_property("use_tabulation",&I3DOM::SetUseTabulation,
                         &I3DOM::GetUseTabulation)
    .add_static_property("use_drooped_template",&I3DOM::SetUseDroopedTemplate,
                         &I3DOM::GetUseDroopedTemplate)
    ;

  bp::class_<std::vector<I3DOM* > >("I3DOMVector")
    .def(bp::vector_indexing_suite<std::vector<I3DOM* > >())
    ;

  bp::class_<I3DOMMap>("I3DOMMap")
    .def(bp::map_indexing_suite<I3DOMMap>())
    ;

}
