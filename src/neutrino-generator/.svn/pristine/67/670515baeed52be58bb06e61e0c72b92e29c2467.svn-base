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

using namespace boost::python;
using namespace std;

// dummy class just used as a namespace.
namespace {
  struct dummy { };
}

void register_TypeDefs()
{
  using namespace nugen;

  enum_<SimMode>("SimMode")
    .value("full", FULL)
    .value("inearth", INEARTH)
    .value("finalonly", FINALONLY)
    .export_values()
    ;

  enum_<VTXGenMode>("VTXGenMode")
    .value("nugen", NUGEN)
    .value("genie", GENIE)
    .value("hybrid", HYBRID)
    .export_values()
    ;

  enum_<BenchMode>("BenchMode")
    .value("off", OFF)
    .value("detedge", DETEDGE)
    .value("indet", INDET)
    .value("allbench", ALLBENCH)
    .export_values()
    ;

  enum_<InjectionMode>("InjectionMode")
    .value("circle", CIRCLE)
    .value("surface", SURFACE)
    .value("dynamic", DYNAMIC)
    .export_values()
    ;

  enum_<InteractionType>("InteractionType")
    .value("cc", CC)
    .value("nc", NC)
    .value("gr", GR)
    .value("gn", GN)
    .value("decay", DECAY)
    .value("nointeraction", NOINTERACTION)
    .value("undefined", UNDEFINED)
    .export_values()
    ;

  enum_<PropagationMode>("PropagationMode")
    .value("legacy", LEGACY)
    .value("nopropweight", NOPROPWEIGHT)
    .value("ncgrweighted", NCGRWEIGHTED)
    .value("autodetect", AUTODETECT)
    .export_values()
    ;

  enum_<AngleSamplingMode>("AngleSamplingMode")
    .value("cos", COS)
    .value("ang", ANG)
    .export_values()
    ;

  def("to_propagation_mode", &ToPropagationMode,"string to type conversion");

  class_<dummy>("TypeDefs")
  ;
}

