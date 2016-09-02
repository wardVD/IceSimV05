/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file pybindings/hit_statistics/module.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the Python bindings for the
 *        "common_variables.hit_statistics" python module.
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

#include "CommonVariables/hit_statistics/calculator.h"

namespace hs = common_variables::hit_statistics;

namespace bp = boost::python;

// For a boost::python function overloading tutorial see:
//   http://www.boost.org/doc/libs/1_49_0/libs/python/doc/tutorial/doc/html/python/functions.html#python.overloading

//______________________________________________________________________________
void register_hit_statisticsModule()
{
    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures
    bp::docstring_options docstring_opts(true, true, false);

    ////////////////////////////////////////////////////////////////////////////
    // Define the classes.
    ////////////////////////////////////////////////////////////////////////////
    // Define the utility functions.
    //--------------------------------------------------------------------------
    bp::def("calculate_hit_statistics", &hs::CalculateHitStatistics,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the hit statistics values for the given I3Geometry, and the   \n"
        "given I3RecoPulseSeriesMap.                                              \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_cog", &hs::CalculateCOG,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the Center-Of-Gravity (COG) of the event given by the given   \n"
        "I3Geometry, and the given I3RecoPulseSeriesMap.                          \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_cog_z_sigma", &hs::CalculateCOGZSigma,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the sigma value of the z-component of the Center-Of-Gravity   \n"
        "(COG) of the event given by the given I3Geometry, and the given          \n"
        "I3RecoPulseSeriesMap.                                                    \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_min_pulse_time", &hs::CalculateMinPulseTime,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the minimal time of all pulses, e.g. the time of the first    \n"
        "pulse, for the given I3Geometry, and the given I3RecoPulseSeriesMap.     \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_max_pulse_time", &hs::CalculateMaxPulseTime,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the maximal time of all pulses, e.g. the time of the last     \n"
        "pulse, for the given I3Geometry, and the given I3RecoPulseSeriesMap.     \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_q_max_doms", &hs::CalculateQMaxDoms,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the maximal value of all DOM pulse charge sums for the given  \n"
        "I3Geometry, and the given I3RecoPulseSeriesMap.                          \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_q_tot_pulses", &hs::CalculateQTotPulses,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the total charge of all pulses for the given I3Geometry, and  \n"
        "the given I3RecoPulseSeriesMap.                                          \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_z_min", &hs::CalculateZMin,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the minimal OM_Z value of all hits for the given I3Geometry,  \n"
        "and the given I3RecoPulseSeriesMap.                                      \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_z_max", &hs::CalculateZMax,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the maximal OM_Z value of all hits for the given I3Geometry,  \n"
        "and the given I3RecoPulseSeriesMap.                                      \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_z_mean", &hs::CalculateZMean,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the mean of the OM_Z values for the given I3Geometry, and     \n"
        "the given I3RecoPulseSeriesMap.                                          \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_z_sigma", &hs::CalculateZSigma,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the sigma (RMSD) of the OM_Z values for the given I3Geometry, \n"
        "and the given I3RecoPulseSeriesMap.                                      \n"
    );

    //--------------------------------------------------------------------------
    bp::def("calculate_z_travel", &hs::CalculateZTravel,
        (bp::arg("geometry"),
         bp::arg("pulses_map")
        ),
        "Calculates the ZTravel cut variable for the given I3Geometry, and the    \n"
        "given I3RecoPulseSeriesMap. It is defined as:                            \n"
        "                                                                         \n"
        ".. math::                                                                \n"
        "                                                                         \n"
        "    zTravel = \\frac{1}{NHitDoms}\\sum_{i=1}^{NHitDoms}\\left( OMz_i - \\overline{OMz_{\\mathrm{first\\ quartile\\ in\\ time}}}\\right)\n"
        "                                                                         \n"
        "where :math:`\\overline{OMz_{\\mathrm{first\\ quartile\\ in\\ time}}}` is \n"
        "the arithmetic mean of the DOM's z-component of the first quartile of    \n"
        "the hit DOMs sorted ascending by time. The time of each hit DOM is taken \n"
        "from the first pulse of this DOM:                                        \n"
        "                                                                         \n"
        ".. math::                                                                \n"
        "                                                                         \n"
        "    \\overline{OMz_{\\mathrm{first\\ quartile\\ in\\ time}}} = \\frac{1}{NHitDoms/4}\\sum_{j=1}^{NHitDoms/4} OMz_j\n"
        "                                                                         \n"
        ".. note::                                                                \n"
        "                                                                         \n"
        "    ZTravel is only defined for :math:`NHitDoms >= 4`. Otherwise it      \n"
        "    returns NAN.                                                         \n"
    );
}

//______________________________________________________________________________
BOOST_PYTHON_MODULE(hit_statistics)
{
    load_project("CommonVariables", false);
    register_hit_statisticsModule();
}
