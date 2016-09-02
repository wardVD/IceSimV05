//
//   Copyright (c) 2011   Claudio Kopper
//   
//   $Id: module.cxx 142739 2016-03-02 18:43:45Z nega $
//
//   This file is part of the IceTray module g4sim-interface.
//
//   g4sim-interface is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <icetray/I3FrameObject.h>
#include <icetray/load_project.h>

using namespace boost::python;
namespace bp = boost::python;
#include <boost/preprocessor.hpp>

//
//  Add the class you are registering to this list of parenthesized typenames.
//  Don't forget to watch that the newlines are backslashed.
//  To register class Nick, add (Nick) to the list below, add
//  Nick.cxx to the list of i3_add_library out in CMakeLists.txt,
//  and create a file Nick.cxx that contains a function 
//    void register_Nick();
//  that does the boost.python registration for class Nick.
//
#define REGISTER_THESE_THINGS                       \
    (I3GENIEGenerator)(I3GENIEResultDict)

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

BOOST_PYTHON_MODULE(genie_icetray)
{
  load_project("libgenie-icetray", false);

  bp::docstring_options doc_options;
  doc_options.disable_signatures();
  BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
}

