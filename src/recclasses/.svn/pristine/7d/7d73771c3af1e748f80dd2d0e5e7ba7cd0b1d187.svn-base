#include "recclasses/I3HitMultiplicityValues.h"
#include "wrap.h"
#include <boost/python.hpp>

void register_I3HitMultiplicityValues()
{
    using namespace boost::python;

    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures
    docstring_options docstring_opts(true, true, false);

    wrap::I3FrameObject<I3HitMultiplicityValues>("I3HitMultiplicityValues",
        "The I3HitMultiplicityValues class is an I3FrameObject class which stores \n"
        "the calculared hit multiplicity.                                         \n"
        )
        .def(init<uint32_t, uint32_t, uint32_t, uint64_t>(
            (arg("n_hit_strings"),
             arg("n_hit_doms"),
             arg("n_hit_doms_one_pulse"),
             arg("n_pulses")
            )
        ))
        .add_property("n_hit_strings",
            &I3HitMultiplicityValues::GetNHitStrings,
            &I3HitMultiplicityValues::SetNHitStrings,
            "The total number of hit strings."
        )
        .add_property("n_hit_doms",
            &I3HitMultiplicityValues::GetNHitDoms,
            &I3HitMultiplicityValues::SetNHitDoms,
            "The total number of hit DOMs."
        )
        .add_property("n_hit_doms_one_pulse",
            &I3HitMultiplicityValues::GetNHitDomsOnePulse,
            &I3HitMultiplicityValues::SetNHitDomsOnePulse,
            "The total number of DOMs having exactly one pulse."
        )
        .add_property("n_pulses",
            &I3HitMultiplicityValues::GetNPulses,
            &I3HitMultiplicityValues::SetNPulses,
            "The total number of pulses."
        )
    ;
}
