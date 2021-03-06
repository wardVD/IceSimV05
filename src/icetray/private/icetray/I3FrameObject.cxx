/**
 *  $Id: I3FrameObject.cxx 1559 2013-02-10 14:55:26Z nwhitehorn $
 *  
 *  Copyright (C) 2007
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
#include <icetray/serialization.h>
#include <icetray/I3FrameObject.h>

I3FrameObject::~I3FrameObject() { }

template <class Archive>
void
I3FrameObject::serialize(Archive & ar, unsigned version)
{
}

I3_BASIC_SERIALIZABLE(I3FrameObject);

#if BOOST_VERSION >= 103600
BOOST_SERIALIZATION_SHARED_PTR(I3FrameObject);
#endif
