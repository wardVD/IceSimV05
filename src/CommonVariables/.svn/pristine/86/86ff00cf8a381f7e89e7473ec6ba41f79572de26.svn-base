/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file pybindings/track_characteristics/module.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the Python bindings for the
 *        "common_variables.track_characteristics" python module.
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

#include "CommonVariables/track_characteristics/calculator.h"

namespace tc = common_variables::track_characteristics;

namespace bp = boost::python;

//______________________________________________________________________________
void register_track_characteristicsModule()
{
    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures
    bp::docstring_options docstring_opts(true, true, false);

    ////////////////////////////////////////////////////////////////////////////
    // Define the classes.
    ////////////////////////////////////////////////////////////////////////////
    // Define the utility functions.
    //--------------------------------------------------------------------------
    bp::def("calculate_track_characteristics", &tc::CalculateTrackCharacteristics,
        (bp::arg("geometry"),
         bp::arg("pulses_map"),
         bp::arg("particle"),
         bp::arg("track_cylinder_radius")
        ),
        "Calculates the track characteristics values, e.g.                        \n"
        "avg_dom_dist_q_tot_dom, empty_hits_track_length,                         \n"
        "track_hits_separation_length, and track_hits_distribution_smoothness, for\n"
        "the given I3Geometry, the given I3RecoPulseSeriesMap, and the given      \n"
        "track, given through the I3Particle object, for pulses within the given  \n"
        "cylinder radius around the given track.                                  \n"
    );
}

//______________________________________________________________________________
BOOST_PYTHON_MODULE(track_characteristics)
{
    load_project("CommonVariables", false);
    register_track_characteristicsModule();
}
