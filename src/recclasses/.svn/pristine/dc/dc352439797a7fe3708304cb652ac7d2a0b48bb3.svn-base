/**
 *  $Id$
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
#include "recclasses/I3LineFitParams.h"
#include "../recclasses/converter/I3LineFitParamsConverter.h"
#include "wrap.h"
#include <tableio/converter/pybindings.h>
#include <boost/python.hpp>

void register_I3LineFitParams()
{
  using namespace boost::python;

  wrap::I3FrameObject<I3LineFitParams>()
    .def_readwrite("LFVel", &I3LineFitParams::LFVel)
    .def_readwrite("LFVelX", &I3LineFitParams::LFVelX)
    .def_readwrite("LFVelY", &I3LineFitParams::LFVelY)
    .def_readwrite("LFVelZ", &I3LineFitParams::LFVelZ)
    .def_readwrite("NHits", &I3LineFitParams::nHits)
    ;

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3LineFitParamsConverter,
			     "Dumps I3LineFitParams parameter objects");
}
