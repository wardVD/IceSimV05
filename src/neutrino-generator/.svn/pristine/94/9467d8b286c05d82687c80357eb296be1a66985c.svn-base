//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
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

#include <neutrino-generator/utils/EnumTypeDefs.h>
#include <neutrino-generator/Steering.h>
#include <neutrino-generator/legacy/I3NuGVEvent.h>
#include <earthmodel-service/EarthModelService.h>
#include <string>
#include <vector>
#include <boost/make_shared.hpp>

namespace bp = boost::python;

static boost::shared_ptr<nugen::Steering>
make_default()
{
   return boost::make_shared<nugen::Steering>(boost::make_shared<earthmodel::EarthModelService>());
}

void register_Steering()
{
  bp::class_<nugen::Steering, boost::shared_ptr<nugen::Steering>, boost::noncopyable >
         ("Steering", bp::init<boost::shared_ptr<earthmodel::EarthModelService>, 
          nugen::SimMode, nugen::VTXGenMode, nugen::InjectionMode>())
     .def(bp::init<boost::shared_ptr<earthmodel::EarthModelService> >())
     .def(bp::init<const I3Context &>())
     .def("__init__", bp::make_constructor(make_default))

     #define PROPS (NGen) (SimMode) (VTXGenMode) (InjectionMode) (DetectionSurface) (CylinderParams)
        BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, nugen::Steering, PROPS)
     #undef PROPS
     .add_property("do_muon_range_extension", &nugen::Steering::DoMuonRangeExtension, &nugen::Steering::SetDoMuonRangeExtension)
     .add_property("use_simple_scatter_form", &nugen::Steering::UseSimpleScatterForm, &nugen::Steering::SetUseSimpleScatterForm)
     .def("print_setting",
          &nugen::Steering::PrintSetting,"print stored parameters")

     ;

}
