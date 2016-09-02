/* Copyright (C) 2012
 * Samuel Flis <samuel.d.flis@gmail.com>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include <icetray/I3FrameObject.h>
#include <boost/preprocessor.hpp>
#include <icetray/load_project.h>

using namespace boost::python;

#define REGISTER_THESE_THINGS \
    (InterpolatedSPETemplate)\
    (I3DOM)\
    (I3InIceDOM)\
    (I3IceTopDOM)\
    (domlauncherutils)\
    (PMTResponseSimulator)

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();

BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

I3_PYTHON_MODULE(DOMLauncher)
{
    load_project("libDOMLauncher", false);

    BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
}
