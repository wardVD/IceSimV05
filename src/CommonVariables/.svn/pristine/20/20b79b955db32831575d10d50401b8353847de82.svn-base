/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file pybindings/direct_hits/module.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the Python bindings for the
 *        "common_variables.direct_hits" python module.
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
#include "icetray/python/copy_suite.hpp"
#include "icetray/python/list_indexing_suite.hpp"
#include "icetray/python/stream_to_string.hpp"
#include "dataclasses/ostream_overloads.hpp"

#include "CommonVariables/direct_hits/calculator.h"
#include "CommonVariables/direct_hits/I3DirectHitsDefinition.h"
#include "CommonVariables/direct_hits/I3DirectHitsTimeWindow.h"

namespace dh = common_variables::direct_hits;

namespace bp = boost::python;

// For a boost::python function overloading tutorial see:
//   http://www.boost.org/doc/libs/1_49_0/libs/python/doc/tutorial/doc/html/python/functions.html#python.overloading

//______________________________________________________________________________
// Thin wrappers for dh::CalculateDirectHits overloaded functions.
//------------------------------------------------------------------------------
// Overloaded function class (ofc) 1: (const I3Geometry&, const I3RecoPulseSeriesMap&, const I3Particle&)
I3DirectHitsValuesMapPtr dh__CalculateDirectHits_ofc1(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap,
    const I3Particle&           particle
)
{ return dh::CalculateDirectHits(geometry, pulsesMap, particle); }
//------------------------------------------------------------------------------
// Overloaded function class (ofc) 2: (const I3DirectHitsDefinitionSeries&, const I3Geometry&, const I3RecoPulseSeriesMap&, const I3Particle&)
I3DirectHitsValuesMapPtr dh__CalculateDirectHits_ofc2(
    const I3DirectHitsDefinitionSeries& dhDefinitions,
    const I3Geometry&                   geometry,
    const I3RecoPulseSeriesMap&         pulsesMap,
    const I3Particle&                   particle
)
{ return dh::CalculateDirectHits(dhDefinitions, geometry, pulsesMap, particle); }
//------------------------------------------------------------------------------
// Overloaded function class (ofc) 3: (const bp::list&, const I3Geometry&, const I3RecoPulseSeriesMap&, const I3Particle&)
I3DirectHitsValuesMapPtr dh__CalculateDirectHits_ofc3(
    const bp::list&             dhDefinitions_list,
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap,
    const I3Particle&           particle
)
{
    // Convert python list into an I3DirectHitsDefinitionSeries object.
    I3DirectHitsDefinitionSeries dhDefinitions;
    uint64_t N = bp::len(dhDefinitions_list);
    for(uint64_t i=0; i<N; ++i)
        dhDefinitions.push_back(bp::extract<I3DirectHitsDefinition>(dhDefinitions_list[i]));
    return dh::CalculateDirectHits(dhDefinitions, geometry, pulsesMap, particle);
}

//______________________________________________________________________________
void register_direct_hitsModule()
{
    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures.
    bp::docstring_options docstring_opts(true, true, false);

    ////////////////////////////////////////////////////////////////////////////
    // Define classes.
    //--------------------------------------------------------------------------
    // I3DirectHitsTimeWindow
    bp::class_<I3DirectHitsTimeWindow, I3DirectHitsTimeWindowPtr>("I3DirectHitsTimeWindow",
        "The I3DirectHitsTimeWindow class describes the relative time window for the\n"
        "direct hits calculation.                                                   \n"
        "                                                                           \n"
        "The start and end time of the time window can be accessed through the class\n"
        "properties ``time_0`` and ``time_1``, respectively. In C++ these values can\n"
        "be accessed through the ``GetTime0()`` and ``GetTime1()`` class methods.   \n",
        bp::init<double, double>(
            (bp::arg("time0"),
             bp::arg("time1")
            ),
            "Constructs an I3DirectHitsTimeWindow class object with "
            "specified start and end times for that time window."
        ))
        .add_property("time_0",
            &I3DirectHitsTimeWindow::GetTime0, &I3DirectHitsTimeWindow::SetTime0,
            "The start time of the relative direct hits time window."
        )
        .add_property("time_1",
            &I3DirectHitsTimeWindow::GetTime1, &I3DirectHitsTimeWindow::SetTime1,
            "The end time of the relative direct hits time window."
        )
        .def("is_time_after_time_window", &I3DirectHitsTimeWindow::IsTimeAfterTimeWindow,
            (bp::arg("time")),
            "Checks if the given relative time is after the relative time window.   \n"
        )
        .def("is_time_before_time_window", &I3DirectHitsTimeWindow::IsTimeBeforeTimeWindow,
            (bp::arg("time")),
            "Checks if the given relative time is before the relative time window.  \n"
        )
        .def("is_time_inside_time_window", &I3DirectHitsTimeWindow::IsTimeInsideTimeWindow,
            (bp::arg("time")),
            "Checks if the given relative time is inside the relative time window.  \n"
        )
        .def("__str__", &stream_to_string<I3DirectHitsTimeWindow>)
    ;
    bp::implicitly_convertible<I3DirectHitsTimeWindowPtr, I3DirectHitsTimeWindowConstPtr>();
    //--------------------------------------------------------------------------
    // I3DirectHitsDefinition
    bp::class_<I3DirectHitsDefinition, I3DirectHitsDefinitionPtr>("I3DirectHitsDefinition",
        "The I3DirectHitsDefinition class describes a class of direct hits, e.g. the\n"
        "time window for the direct hits.                                           \n"
        "                                                                           \n"
        "The I3DirectHitsTimeWindow class can be accessed through the C++           \n"
        "``GetTimeWindow()`` method or the Python property ``time_window``.         \n"
        "                                                                           \n"
        "Each direct hits definition class has also a name, which can be accessed   \n"
        "through the C++ ``GetName()`` method or the Python property ``name``.      \n",
        bp::init<std::string, double, double>(
            (bp::arg("name"),
             bp::arg("time0"),
             bp::arg("time1")
            ),
            "Constructs an I3DirectHitsDefinition class object to describe "
            "a particular class of direct hits by their name, start, and "
            "end time of their time window."
        ))
        .add_property("name",
            &I3DirectHitsDefinition::GetName, &I3DirectHitsDefinition::SetName,
            "The name of the direct hits definition."
        )
        .add_property("time_window",
            bp::make_function(
                (I3DirectHitsTimeWindow& (I3DirectHitsDefinition::*) ()) &I3DirectHitsDefinition::GetTimeWindow,
                bp::return_internal_reference<1>()
            ),
            &I3DirectHitsDefinition::SetTimeWindow,
            "The :class:`icecube.common_variables.direct_hits.I3DirectHitsTimeWindow`\n"
            "object, defining the time window of the direct hits.                   \n"
        )
        .def("__str__", &stream_to_string<I3DirectHitsDefinition>)
    ;
    bp::implicitly_convertible<I3DirectHitsDefinitionPtr, I3DirectHitsDefinitionConstPtr>();
    //--------------------------------------------------------------------------
    // I3DirectHitsDefinitionSeries
    bp::class_<I3DirectHitsDefinitionSeries, I3DirectHitsDefinitionSeriesPtr>("I3DirectHitsDefinitionSeries",
        "The I3DirectHitsDefinitionSeries class is a vector of                    \n"
        "I3DirectHitsDefinition objects. Inside Python, it can be used like a     \n"
        "normal Python list through its \"append\" and \"extend\" methods.        \n"
    )
        .def(bp::list_indexing_suite<I3DirectHitsDefinitionSeries>())
        //.def(bp::dataclass_suite<I3DirectHitsDefinitionSeries>())
        .def(bp::copy_suite<I3DirectHitsDefinitionSeries>())
        .def("__str__", &stream_to_string<I3DirectHitsDefinitionSeries>)
        .def(freeze())
    ;
    bp::implicitly_convertible<I3DirectHitsDefinitionSeriesPtr, I3DirectHitsDefinitionSeriesConstPtr>();

    ////////////////////////////////////////////////////////////////////////////
    // Define general functions.
    //--------------------------------------------------------------------------
    bp::def("get_default_definitions", &dh::GetDefaultDefinitions,
        "Gets the I3DirectHitsDefinitionSeries object with the default direct     \n"
        "hits definitions, e.g. time windows. The following default direct hits   \n"
        "time windows are defined:                                                \n"
        "                                                                         \n"
        "- time window \"A\": [-15ns; +15ns]                                      \n"
        "- time window \"B\": [-15ns; +25ns]                                      \n"
        "- time window \"C\": [-15ns; +75ns]                                      \n"
        "- time window \"D\": [-15ns; +125ns]                                     \n"
    );

    ////////////////////////////////////////////////////////////////////////////
    // define calculate_direct_hits function with all its argument variants
    //--------------------------------------------------------------------------
    bp::def("calculate_direct_hits", &dh__CalculateDirectHits_ofc1,
        (bp::arg("geometry"),
         bp::arg("pulses_map"),
         bp::arg("particle")
        ),
        "Calculates the direct hits for the default direct hits time windows      \n"
        "(defined through the get_default_definitions() function), the given      \n"
        "I3Geometry, the given I3RecoPulseSeriesMap, and the given I3Particle.    \n"
    );
    bp::def("calculate_direct_hits", &dh__CalculateDirectHits_ofc2,
        (bp::arg("dh_definitions"),
         bp::arg("geometry"),
         bp::arg("pulses_map"),
         bp::arg("particle")
        ),
        "Calculates the direct hits for the given time windows (given through the \n"
        "I3DirectHitsDefinitionSeries object or the Python list of                \n"
        "I3DirectHitsDefinition objects), the given I3Geometry, the given         \n"
        "I3RecoPulseSeriesMap and the given I3Particle.                           \n"
    );
    bp::def("calculate_direct_hits", &dh__CalculateDirectHits_ofc3,
        (bp::arg("dh_definitions"),
         bp::arg("geometry"),
         bp::arg("pulses_map"),
         bp::arg("particle")
        ), ""
    );

    ////////////////////////////////////////////////////////////////////////////
    // Define utility functions.
    //--------------------------------------------------------------------------
    bp::def("get_direct_hits_pulse_map", &dh::GetDirectHitsPulseMap,
        (bp::arg("dh_definition"),
         bp::arg("geometry"),
         bp::arg("pulses_map"),
         bp::arg("particle"),
         bp::arg("all_pulses_of_direct_doms")=false
        ),
        "Returns an I3RecoPulseSeriesMap with the direct hit DOMs for a given     \n"
        "direct hits definition, e.g. direct hits time window, for the given      \n"
        "I3Geometry, the given I3RecoPulseSeriesMap, and the given I3Particle.    \n"
        "                                                                         \n"
        "If the *all_pulses_of_direct_doms* option (default is ``False``) is set  \n"
        "to ``True``, the I3RecoPulseSeries object of each direct hit DOM will    \n"
        "contain all pulses of the original I3RecoPulseSeries of that DOM.        \n"
        "Otherwise only the direct pulses will be included.                       \n"
    );
    //--------------------------------------------------------------------------
    bp::def("get_direct_pulses_time_residuals", &dh::GetDirectPulsesTimeResiduals,
        (bp::arg("dh_definition"),
         bp::arg("geometry"),
         bp::arg("pulses_map"),
         bp::arg("particle")
        ),
        "Returns an I3VectorDouble containing the time residuals of all direct    \n"
        "pulses for a given direct hits definition, e.g. direct hits time window, \n"
        "for the given I3Geometry, the given I3RecoPulseSeriesMap, and the given  \n"
        "I3Particle.                                                              \n"
    );
}

//______________________________________________________________________________
BOOST_PYTHON_MODULE(direct_hits)
{
    load_project("CommonVariables", false);
    register_direct_hitsModule();
}
