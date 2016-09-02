#include "DOMLauncher/domlauncherutils.h"
#include <boost/python.hpp>
#include <phys-services/I3RandomService.h>

#include <vector>

namespace bp = boost::python;
using namespace domlauncherutils::detail;
using namespace domlauncherutils;
void register_domlauncherutils()
{
  bp::enum_<LaunchType>("TriggerType")
    .value("HLC", HLC)
    .value("SLC", SLC)
    .value("Discriminator", Discriminator)
    .value("CPU_REQUESTED", CPU_REQUESTED)
    .export_values();

  bp::enum_<ATWDChip>("ATWDChip")
    .value("ATWDa", ATWDa)
    .value("ATWDb", ATWDb)
    .export_values();


  bp::class_<DiscCross>("DiscCross")
    .def(bp::init<OMKey,double>())
    .def_readwrite("DOM",&DiscCross::DOM)
    .def_readwrite("time",&DiscCross::time)
    .def_readwrite("type",&DiscCross::type)
    .def("__lt__", &dcCompare)
    ;

  bp::class_<WaveformEvaluator>("WaveformEvaluator")
    .def(bp::init<double, double, double>())
    .def_readwrite("SetPulses",&WaveformEvaluator::SetPulses)
    .def_readwrite("Clear",&WaveformEvaluator::Clear)
    .def_readwrite("WaveFormAmplitude",&WaveformEvaluator::WaveFormAmplitude)
    ;
    
  bp::class_<std::vector<DiscCross> >("DCStream")
    .def(bp::vector_indexing_suite<std::vector<DiscCross> >())
    ;

}
