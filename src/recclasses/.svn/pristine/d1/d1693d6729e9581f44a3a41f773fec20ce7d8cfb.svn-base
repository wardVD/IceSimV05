/**
 *  $Id$
 *  
 *  Copyright (C) 2008
 *  Fabian Kislat  <fabian.kislat@desy.de>
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

#include "recclasses/I3TopRecoPlaneFitParams.h"
#include "../recclasses/converter/I3TopRecoPlaneFitParamsConverter.h"
#include "wrap.h"
#include <tableio/converter/pybindings.h>
#include <boost/python.hpp>

void register_I3TopRecoPlaneFitParams()
{
  using namespace boost::python;

  wrap::I3FrameObject<I3TopRecoPlaneFitParams>()
    .def_readwrite("e_t", &I3TopRecoPlaneFitParams::ET)
    .def_readwrite("delta_t", &I3TopRecoPlaneFitParams::DeltaT)
    .def_readwrite("t0", &I3TopRecoPlaneFitParams::T0)
    .def_readwrite("x0", &I3TopRecoPlaneFitParams::X0)
    .def_readwrite("y0", &I3TopRecoPlaneFitParams::Y0)
    .def_readwrite("chi2", &I3TopRecoPlaneFitParams::Chi2)
    .def_readwrite("nPulses", &I3TopRecoPlaneFitParams::NPulses)
  ;

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3TopRecoPlaneFitParamsConverter, "Dumps I3TopRecoPlaneFitParams parameter objects");
}
