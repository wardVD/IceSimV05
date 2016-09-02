#include "recclasses/I3TimeCharacteristicsValues.h"
#include "wrap.h"
#include <boost/python.hpp>


void register_I3TimeCharacteristicsValues()
{
    using namespace boost::python;

    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures
    docstring_options docstring_opts(true, true, false);

    ////////////////////////////////////////////////////////////////////////////
    // Define the classes.
    //--------------------------------------------------------------------------
    // I3TimeCharacteristicsValues
    wrap::I3FrameObject<I3TimeCharacteristicsValues>("I3TimeCharacteristicsValues",
        "The I3TimeCharacteristicsValues class is an I3FrameObject class which\n"
        "stores the calculared values."
        )
        .def(bp::init<double, int, double, int>(
            (bp::arg("timelength_fwhm"),
            bp::arg("timelength_last_first"),
            bp::arg("timelength_maxgap"),
            bp::arg("zpattern")
            ),
            "Constructs an I3TimeCharacteristicsValues object with initial values\n"
            "for ``event_time_length``."
        ))
        .add_property("timelength_fwhm",
            &I3TimeCharacteristicsValues::GetTimeLengthFwhm,
            &I3TimeCharacteristicsValues::SetTimeLengthFwhm,
            "FWHM of a gaussian fit on all first pulses per hit"
        )
        .add_property("timelength_last_first",
            &I3TimeCharacteristicsValues::GetTimeLengthLastFirst,
            &I3TimeCharacteristicsValues::SetTimeLengthLastFirst,
            "The first pulse time at the last hit dom minus the last pulse time at the first hit dom"
        )
        .add_property("timelength_maxgap",
            &I3TimeCharacteristicsValues::GetTimeLenghtMaxgap,
            &I3TimeCharacteristicsValues::SetTimeLenghtMaxgap,
            "The maximum gap in the time distributions of all first pulses times"
        )
        .add_property("zpattern",
            &I3TimeCharacteristicsValues::GetZPattern,
            &I3TimeCharacteristicsValues::SetZPattern,
            "All First pulses per Dom have been time ordered. Then look at the z values. "
            "If z > lastZ pattern++ else if z < lastZ patter--"
        )
    ;
}
