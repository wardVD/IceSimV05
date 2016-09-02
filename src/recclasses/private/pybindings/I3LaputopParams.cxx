/**
 *  $Id: I3LaputopParams.cxx 126571 2014-12-04 21:47:30Z hdembinski $
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

#include "recclasses/I3LaputopParams.h"
#include "recclasses/LaputopEnums.h"
#include "../recclasses/converter/I3LaputopParamsConverter.h"
#include <tableio/converter/pybindings.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Position.h>
#include <boost/python.hpp>
#include <string>

namespace boost { namespace python {
    static I3LaputopParamsPtr
    from_frame(I3Frame& frame, const std::string &name)
    {
        if (!frame.Has(name)) {
            PyErr_SetString(PyExc_KeyError, name.c_str());
            throw_error_already_set();
        }
        I3LaputopParamsConstPtr lp =
            frame.Get<I3LaputopParamsConstPtr>(name);
        if (!lp) {
            PyErr_SetString(PyExc_TypeError, name.c_str());
            throw_error_already_set();
        }
        return boost::const_pointer_cast<I3LaputopParams>(lp);
    }
}}

void register_I3LaputopParams()
{
  using namespace boost::python;
  using namespace Laputop;

  // Tell boost::python that it should create the python docstrings with
  // user-defined docstrings, python signatures, but no C++ signatures.
  docstring_options docstring_opts(true, true, false);

  enum_<Parameter::Enum>("LaputopParameter")
    .value("Log10_S125", Parameter::Log10_S125)
    .value("Beta", Parameter::Beta)
    .value("Age", Parameter::Age)
    .value("Nmu", Parameter::Nmu)
    .value("Nx", Parameter::Nx)
    .value("Ny", Parameter::Ny)
    .value("Tc", Parameter::Tc)
    .value("Xc", Parameter::Xc)
    .value("Yc", Parameter::Yc)
    .value("Theta", Parameter::Theta)
    .value("Phi", Parameter::Phi)
    // don't wrap size, not needed
    ;

  enum_<LDF::Enum>("LaputopLDF")
    .value("None", LDF::None)
    .value("DLP", LDF::DLP)
    ;

  enum_<FrontDelay::Enum>("LaputopFrontDelay")
    .value("None", FrontDelay::None)
    .value("GaussParabola", FrontDelay::GaussParabola)
    ;

  enum_<Energy::Enum>("LaputopEnergy")
    .value("None", Energy::None)
    .value("IC73SpectrumPaper", Energy::IC73SpectrumPaper)
    .value("IC73SpectrumPaperProton", Energy::IC73SpectrumPaperProton)
    .value("IC73SpectrumPaperIron", Energy::IC73SpectrumPaperIron)
    .value("ICRC2015_H4a_E27", Energy::ICRC2015_H4a_E27)
    ;

  class_<I3LaputopParams,
         bases<I3FrameObject>,
         bases<I3LaputopParams::ParameterStorageType>,
         I3LaputopParamsPtr>("I3LaputopParams")
    .def(dataclass_suite<I3LaputopParams>())
    .def("has", &I3LaputopParams::Has,
         "Check if parameter is stored (either value or error may still be NaN).",
         args("self", "par"))
    .def("value", &I3LaputopParams::GetValue,
         "Get value of parameter.",
         args("self", "par"))
    .def("error", &I3LaputopParams::GetError,
         "Get error of parameter.",
         args("self", "par"))
    .def("cov", &I3LaputopParams::GetCovariance,
         "Get covariance of two parameters.",
         args("self", "par1", "par2"))
    .def("set_value", &I3LaputopParams::SetValue,
         "Set value of parameter.",
         args("self", "par", "value"))
    .def("set_error", &I3LaputopParams::SetError,
         "Set error of parameter.",
         args("self", "par", "error"))
    .def("set_cov", &I3LaputopParams::SetCovariance,
         "Set covariance of two parameters.",
         args("self", "par1", "par2", "covariance"))
    .def("energy", &I3LaputopParams::Energy,
         "Get reconstructed energy of shower (differs from true energy by random offset).",
         (arg("self"), arg("type") = Energy::None))
    .def("expected_signal", &I3LaputopParams::ExpectedSignal,
        "Get expected signal from LDF in VEM.",
        (arg("self"), arg("r"), arg("xi") = 0.0, arg("type") = LDF::None))
    .def("expected_signal_error", &I3LaputopParams::ExpectedSignalError,
        "Get expected error of LDF in VEM.",
        (arg("self"), arg("r"), arg("xi") = 0.0, arg("type") = LDF::None))
    .def("expected_shower_front_delay", &I3LaputopParams::ExpectedShowerFrontDelay,
        "Get expected time delay with respect to plane front.",
        (arg("self"), arg("r"), arg("xi") = 0.0, arg("type") = FrontDelay::None))
    .def("expected_shower_front_delay_error", &I3LaputopParams::ExpectedShowerFrontDelayError,
        "Get expected error of time delay with respect to plane front.",
        (arg("self"), arg("r"), arg("xi") = 0.0, arg("type") = FrontDelay::None))
    .add_property("angular_resolution", &I3LaputopParams::GetAngularResolution)
    .def_readwrite("type_ldf", &I3LaputopParams::typeLDF_)
    .def_readwrite("type_front_delay", &I3LaputopParams::typeFrontDelay_)
    .def_readwrite("type_energy", &I3LaputopParams::typeEnergy_)
    .def_readwrite("z_pos", &I3LaputopParams::zPos_)
    .def_readwrite("log_likelihood", &I3LaputopParams::logLikelihood_)
    .def_readwrite("log_likelihood_silent", &I3LaputopParams::logLikelihood_Silent_)
    .def_readwrite("chi2_ldf", &I3LaputopParams::chi2_LDF_)
    .def_readwrite("chi2_time", &I3LaputopParams::chi2_Time_)
    .def_readwrite("ndf_ldf", &I3LaputopParams::ndf_LDF_)
    .def_readwrite("ndf_time", &I3LaputopParams::ndf_Time_)
    .def_readwrite("n_mini", &I3LaputopParams::nMini_)
    .def_readwrite("user_data", &I3LaputopParams::userData_)
    .def("from_frame", &from_frame, args("frame", "key"),
         "Get an I3LaputopParams from the frame, performing "
         "any necessary format conversions behind the scenes.")
    .staticmethod("from_frame")
    ;

  register_pointer_conversions<I3LaputopParams>();

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3LaputopParamsConverter, "Dumps I3LaputopParams parameter objects");
}
