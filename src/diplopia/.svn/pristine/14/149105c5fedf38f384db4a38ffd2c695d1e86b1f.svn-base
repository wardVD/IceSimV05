//
//   Copyright (c) 2004, 2005, 2006, 2007, 2008   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
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

#include <dataclasses/I3Map.h>
#include <icetray/python/std_map_indexing_suite.hpp>

using namespace boost::python;

typedef I3Map<std::string,std::string> I3MapStringString;
I3_POINTER_TYPEDEFS(I3MapStringString);

void register_I3MapStringString()
{
  class_<I3MapStringString, bases<I3FrameObject>, I3MapStringStringPtr>("I3MapStringString")
    .def(std_map_indexing_suite<I3MapStringString>())
    ;
  register_pointer_conversions<I3MapStringString>();

#ifdef NDEBUG
  //
  //  TDS: big problems here.  the duplicate registration of pair<string,int>
  //  with the boost.python conversion registry is a warn when compiled 'release',
  //  and triggers an ugly assert in registry.cpp when compiled 'debug',
  //  due to the fact that we link against different versions of libboost_python[-d].so
  //
  class_<std::map<std::string,std::string> >("std_map_string_string",no_init)
    .def(std_map_indexing_suite<std::map<std::string,std::string> >())
    ;
#endif
}
