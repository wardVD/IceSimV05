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

#include "recclasses/I3TopLateralFitParams.h"
#include "../recclasses/converter/I3TopLateralFitParamsConverter.h"
#include "wrap.h"
#include <tableio/converter/pybindings.h>
#include <boost/python.hpp>

void register_I3TopLateralFitParams()
{
  using namespace boost::python;

  wrap::I3FrameObject<I3TopLateralFitParams>()
    .def_readwrite("xc_err", &I3TopLateralFitParams::Xc_err)
    .def_readwrite("yc_err", &I3TopLateralFitParams::Yc_err)
    .def_readwrite("s125", &I3TopLateralFitParams::S125)
    .def_readwrite("log10_s125_err", &I3TopLateralFitParams::Log10_S125_err)
    .def_readwrite("s50", &I3TopLateralFitParams::S50)
    .def_readwrite("s70", &I3TopLateralFitParams::S70)
    .def_readwrite("s80", &I3TopLateralFitParams::S80)
    .def_readwrite("s100", &I3TopLateralFitParams::S100)
    .def_readwrite("s150", &I3TopLateralFitParams::S150)
    .def_readwrite("s180", &I3TopLateralFitParams::S180)
    .def_readwrite("s250", &I3TopLateralFitParams::S250)
    .def_readwrite("s500", &I3TopLateralFitParams::S500)
    .def_readwrite("s_int", &I3TopLateralFitParams::Sint)
    .def_readwrite("s_int_err", &I3TopLateralFitParams::Sint_err)
    .def_readwrite("e_50", &I3TopLateralFitParams::E_50)
    .def_readwrite("e_100", &I3TopLateralFitParams::E_100)
    .def_readwrite("e_125", &I3TopLateralFitParams::E_125)
    .def_readwrite("e_180", &I3TopLateralFitParams::E_180)
    .def_readwrite("e_500", &I3TopLateralFitParams::E_500)
    .def_readwrite("e_firstguess", &I3TopLateralFitParams::E_firstguess)
    .def_readwrite("e_proton", &I3TopLateralFitParams::E_proton)
    .def_readwrite("e_iron", &I3TopLateralFitParams::E_iron)
    .def_readwrite("beta", &I3TopLateralFitParams::Beta)
    .def_readwrite("beta_err", &I3TopLateralFitParams::Beta_err)
    .def_readwrite("age", &I3TopLateralFitParams::Age)
    .def_readwrite("dir_err", &I3TopLateralFitParams::Dir_err)
    .def_readwrite("llh", &I3TopLateralFitParams::Llh)
    .def_readwrite("llh_silent", &I3TopLateralFitParams::Llh_silent)
    .def_readwrite("chi2", &I3TopLateralFitParams::Chi2)
    .def_readwrite("chi2_time", &I3TopLateralFitParams::Chi2_time)
    .def_readwrite("ndf", &I3TopLateralFitParams::Ndf)
    .def_readwrite("rlogl", &I3TopLateralFitParams::RLogL)
    .def_readwrite("nmini", &I3TopLateralFitParams::NMini)
    .def_readwrite("sumVEM", &I3TopLateralFitParams::SumVEM)
    .def_readwrite("rmin", &I3TopLateralFitParams::Rmin)
    .def_readwrite("rmax", &I3TopLateralFitParams::Rmax)
    .def_readwrite("rtrans", &I3TopLateralFitParams::Rtrans)
    .def_readwrite("loudest_station", &I3TopLateralFitParams::LoudestStation)
    .def_readwrite("corr_xc_yc", &I3TopLateralFitParams::Corr_Xc_Yc)
    .def_readwrite("corr_xc_log10_s125", &I3TopLateralFitParams::Corr_Xc_Log10_S125)
    .def_readwrite("corr_xc_beta", &I3TopLateralFitParams::Corr_Xc_Beta)
    .def_readwrite("corr_yc_log10_s125", &I3TopLateralFitParams::Corr_Yc_Log10_S125)
    .def_readwrite("corr_yc_beta", &I3TopLateralFitParams::Corr_Yc_Beta)
    .def_readwrite("corr_log10_s125_beta", &I3TopLateralFitParams::Corr_Log10_S125_Beta);

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3TopLateralFitParamsConverter, "Dumps I3TopLateralFitParams parameter objects");
}
