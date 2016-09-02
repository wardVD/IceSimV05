/**
 * Copyright (C) 2014
 * Martin Wolf <martin.wolf@fysik.su.se>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @version $Id$
 * @file STTools/private/pybindings/OMKeyLinkSet.cxx
 * @date $Date$
 * @brief This file contains the Python bindings for the OMKeyLinkSet class.
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
#include "recclasses/OMKeyLinkSet.h"
#include "wrap.h"

#include <vector>
#include <boost/python.hpp>

void register_OMKeyLinkSet()
{
  using namespace boost::python;

  // Tell boost::python that it should create the python docstrings with
  // user-defined docstrings, python signatures, but no C++ signatures.
  docstring_options docstring_opts(true, true, false);

  wrap::I3FrameObject<OMKeyLinkSet>()
    .def(init<OMKeySet, OMKeySet>(
      (arg("self"), arg("omkeyset1"), arg("omkeyset2")),
      "Creates a new OMKeyLinkSet object based on the two given OMKeySet \n"
      "objects."
    ))
    .add_property("omkey_set_1",
        make_function( (OMKeySet& (OMKeyLinkSet::*)()) &OMKeyLinkSet::GetOMKeySet1,
                           return_internal_reference<1>() ),
        &OMKeyLinkSet::SetOMKeySet1)
    .add_property("omkey_set_2",
        make_function( (OMKeySet& (OMKeyLinkSet::*)()) &OMKeyLinkSet::GetOMKeySet2,
                           return_internal_reference<1>() ),
        &OMKeyLinkSet::SetOMKeySet2)
    .def("contains", &OMKeyLinkSet::Contains,
         (arg("omkeylink")),
         "Checks if the given OMKeyLink object is represented by this OMKey \n"
         "link set."
    )
  ;

  wrap::I3Vector<OMKeyLinkSet>();
}
