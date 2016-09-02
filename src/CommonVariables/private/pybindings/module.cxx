/**
 * $Id: module.cxx 93523 2012-09-26 09:16:32Z jacobi $
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file pybindings/module.cxx
 * @version $Revision: 93523 $
 * @date $Date: 2012-09-26 04:16:32 -0500 (Wed, 26 Sep 2012) $
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the Python bindings for the CommonVariables project
 *        defining a python module named "common_variables".
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

using namespace boost::python;

//______________________________________________________________________________
void register_common_variablesModule()
{

}

//______________________________________________________________________________
BOOST_PYTHON_MODULE(common_variables)
{
    load_project("CommonVariables", false);
    register_common_variablesModule();
}
