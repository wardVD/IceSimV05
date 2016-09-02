/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file pybindings/hit_multiplicity/module.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the Python bindings for the
 *        "common_variables.hit_multiplicity" python module.
 *
 *        This file is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation; either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        This program is distributed in the hope that it will be useful,
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */
#include <boost/python.hpp>

#include "icetray/load_project.h"

#include "CommonVariables/hit_multiplicity/calculator.h"

namespace hm = common_variables::hit_multiplicity;

namespace bp = boost::python;

// For a boost::python function overloading tutorial see:
//   http://www.boost.org/doc/libs/1_49_0/libs/python/doc/tutorial/doc/html/python/functions.html#python.overloading

//______________________________________________________________________________
void register_hit_multiplicityModule()
{
    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures
    bp::docstring_options docstring_opts(true, true, false);

    ////////////////////////////////////////////////////////////////////////////
    // Define classes.
    //--------------------------------------------------------------------------

    ////////////////////////////////////////////////////////////////////////////
    // Define utility functions.
    //--------------------------------------------------------------------------
    bp::def("calculate_hit_multiplicity", &hm::CalculateHitMultiplicity,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the hit multiplicity values, e.g. NHitStrings, NHitDoms,      \n"
        "NHitDomsOnePulse, and NPulses, for the given I3Geometry, and the given   \n"
        "I3RecoPulseSeriesMap.                                                    \n"
    );
}

//______________________________________________________________________________
BOOST_PYTHON_MODULE(hit_multiplicity)
{
    load_project("CommonVariables", false);
    register_hit_multiplicityModule();
}
