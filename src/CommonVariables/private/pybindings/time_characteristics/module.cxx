/*
 * @author Anna Pollmann <anna.pollmann@uni-wuppertal.de>
 */

#include <boost/python.hpp>

#include "icetray/load_project.h"

#include "CommonVariables/time_characteristics/calculator.h"

namespace tc = common_variables::time_characteristics;

namespace bp = boost::python;


//______________________________________________________________________________
void register_time_characteristicsModule()
{
    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures
    bp::docstring_options docstring_opts(true, true, false);

    ////////////////////////////////////////////////////////////////////////////
    // Define the utility functions.
    //--------------------------------------------------------------------------
    bp::def("calculate_time_characteristics_values", &tc::CalculateTimeCharacteristics,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the time_characteristics values, e.g.                        \n"
        "event_time_length, for\n"
        "the given I3Geometry and the given I3RecoPulseSeriesMap \n"
    );
}

//______________________________________________________________________________
BOOST_PYTHON_MODULE(time_characteristics)
{
    load_project("CommonVariables", false);
    register_time_characteristicsModule();
}
