/**
 * Copyright (C) 2014
 * Martin Wolf <martin.wolf@fysik.su.se>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @version $Id$
 * @file STTools/private/pybindings/I3STConfiguration.cxx
 * @date $Date$
 * @brief This file contains the Python bindings for the I3STConfiguration
 *        class.
 *
 *        ----------------------------------------------------------------------
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
 */
#include "recclasses/I3STConfiguration.h"
#include "wrap.h"
#include <boost/python.hpp>

void register_I3STConfiguration()
{
  using namespace boost::python;

  // Tell boost::python that it should create the python docstrings with
  // user-defined docstrings, python signatures, but no C++ signatures.
  docstring_options docstring_opts(true, true, false);

  wrap::I3FrameObject<I3STConfiguration>("I3STConfiguration",
      "The I3STConfiguration class is a base class for all ST algorithm specific\n"
      "ST configuration classes.                                                \n"
      "It holds an I3VectorOMKeyLink for a list of OM links for which the       \n"
      "particular ST configuration applies to.                                  \n")
    .def(init<const std::string&, const I3VectorOMKeyLinkSet&>(
         (arg("self"), arg("name"), arg("omKeyLinkSets")),
         "Constructs a new I3STConfiguration object with the given name and given  \n"
         "list of OMKeyLinkSet objects."
    ))
    .add_property("name",
        &I3STConfiguration::GetName,
        &I3STConfiguration::SetName,
        "The name of the ST configuration. For example, this name could be the    \n"
        "name of the \"sub-detector\" for which this ST configuration applies to. \n"
    )
    .add_property("omkey_link_sets",
        make_function( (I3VectorOMKeyLinkSet& (I3STConfiguration::*)()) &I3STConfiguration::GetOMKeyLinkSets, return_internal_reference<1>() ),
        &I3STConfiguration::SetOMKeyLinkSets,
        "The list of OMKeyLinkSets for which this ST configuration applies to.     \n"
    )
  ;

  wrap::I3Vector<I3STConfiguration>();
}
