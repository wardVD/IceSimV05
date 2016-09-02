/**
 *  $Id: module.cxx 108249 2013-07-16 09:22:47Z boersma $
 *  
 *  Copyright (C) 2008
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */

#include <icetray/load_project.h>

#include "wrappers.h"

#ifdef USE_NUMPY
#include <numpy/numpyconfig.h>
#ifdef NPY_1_7_API_VERSION
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#include <numpy/ndarrayobject.h>
#endif

using namespace boost::python;

#define REGISTER_THESE_THINGS \
	(I3PhotonicsServiceCommons)(I3PhotonicsService)(I3PhotoSplineService) \
	(I3PhotonicsTableService)(I3DummyPhotonicsService)
	
#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

// Workarounds for Python 3 + numpy
#ifdef USE_NUMPY
#if PY_MAJOR_VERSION >= 3
static PyObject *hack_numpy()
{
	import_array();
	return NULL;
}
#else
static void hack_numpy()
{
	import_array();
}
#endif
#endif

BOOST_PYTHON_MODULE(photonics_service)
{
	load_project("libphotonics-service", false);
#ifdef USE_NUMPY
	hack_numpy();
#endif

	BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);

}
