/**
 * Copyright (C) 2014
 * Martin Wolf <martin.wolf@fysik.su.se>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @version $Id$
 * @file STTools/private/pybindings/OMKeyLink.cxx
 * @date $Date$
 * @brief This file contains the Python bindings for the OMKeyLink class.
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

#include "recclasses/OMKeyLink.h"
#include "wrap.h"
#include <icetray/OMKey.h>
#include <boost/python.hpp>


void register_OMKeyLink()
{
  using namespace boost::python;

  // Tell boost::python that it should create the python docstrings with
  // user-defined docstrings, python signatures, but no C++ signatures.
  docstring_options docstring_opts(true, true, false);

  wrap::I3FrameObject<OMKeyLink>()
    .def(init<OMKey, OMKey>(
      (arg("self"), arg("omkey1"), arg("omkey2")),
      "Creates a new OMKeyLink object."
    ))
    .add_property("om_key_1",
      make_function( (OMKey& (OMKeyLink::*)()) &OMKeyLink::GetOMKey1, return_internal_reference<1>() ),
      &OMKeyLink::SetOMKey1)
    .add_property("om_key_2",
      make_function( (OMKey& (OMKeyLink::*)()) &OMKeyLink::GetOMKey2, return_internal_reference<1>() ),
      &OMKeyLink::SetOMKey2)
  ;

  wrap::I3Vector<OMKeyLink>();
}
