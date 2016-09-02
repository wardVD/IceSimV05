/**
 *  $Id$
 *
 *  Copyright (C) 2008
 *  Hans Dembinski <hans.dembinski@icecube.wisc.edu>
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

#include <recclasses/I3CompactKeyList.h>
#include <boost/python.hpp>

void register_I3CompactKeyList()
{
  using namespace boost::python;

  // completely inherits interface from I3Vector<OMKey>
  class_<I3CompactKeyList
         , bases<I3Vector<OMKey> >
  >("I3CompactKeyList");
}
