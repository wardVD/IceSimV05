/**
 * Copyright (C) 2014
 * Martin Wolf <martin.wolf@fysik.su.se>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @version $Id$
 * @file STTools/private/pybindings/OMKeySet.cxx
 * @date $Date$
 * @brief This file contains the Python bindings for the OMKeySet class.
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

#include "recclasses/OMKeySet.h"
#include "wrap.h"

#include <vector>
#include <string>
#include <boost/python.hpp>

void register_OMKeySet()
{
    using namespace boost::python;

    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures.
    docstring_options docstring_opts(true, true, false);

    typedef std::vector<std::string> vstring;
    wrap::I3FrameObject<OMKeySet>()
        .def(init< vstring, vstring >(
            (arg("self"), arg("strings"), arg("oms")),
          "Creates a new OMKeySet object. The `strings` and `oms` arguments  \n"
          "are lists of strings specifying the ranges of string numbers and  \n"
          "OM numbers. The string and om lists must be of the same length."
        ))
        .add_property("omkeys", &OMKeySet::GetOMKeys
            , "The I3VectorOMKey containing all (unique) OMKeys that are defined \n"
              "by this OMKeySet.")
        .add_property("is_empty", &OMKeySet::IsEmpty
            , "Checks if this OMKey set is an empty set, i.e. defines no OMKeys.")
        .def("contains", &OMKeySet::Contains
            , (arg("omkey")),
              "Checks if the given OMKey object is part of this OMKey set        \n"
              "definition.")
    ;

    wrap::I3Vector<OMKeySet>();
}
