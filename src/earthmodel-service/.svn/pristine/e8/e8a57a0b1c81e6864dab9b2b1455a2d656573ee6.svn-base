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

#include <earthmodel-service/EarthModelCalculator.h>

namespace bp = boost::python;
using namespace earthmodel;

void register_EarthModelCalculator()
{

  // map the EarthModelCalculator namespace to a sub-module
  // make "from earthmodel-service import <whatever>" work
  bp::object EarthModelCalculatorModule(bp::handle<>(bp::borrowed(PyImport_AddModule("earthmodel-service.EarthModelCalculator"))));

  // make "from earthmodel-service import EarthModelCalculator" work
  bp::scope().attr("EarthModelCalculator") = EarthModelCalculatorModule;

  // set the current scope to the new sub-module
  bp::scope EarthModelCalculator_scope = EarthModelCalculatorModule;

  // export stuff in the EarthModelCalculator namespace

  bp::enum_<EarthModelCalculator::LeptonRangeOption>("lepton_range_option")
    .value("default", EarthModelCalculator::DEFAULT)
    .value("legacy", EarthModelCalculator::LEGACY)
    .value("nusim", EarthModelCalculator::NUSIM)
    .export_values()
    ;

  bp::def("impact_parameter", EarthModelCalculator::GetImpactParameter,
      "EarthModelCalculator::GetImpactParameter(const I3Position &p0, const I3Direction &d, double &t, I3Position &p)");

  bp::def("intersections_with_sphere", EarthModelCalculator::GetIntersectionsWithSphere,
      "EarthModelCalculator::GetIntersectionsWithSphere(const I3Position &impact_pos_CE, const I3Direction &dirCE, double r, I3Position &startPosCE, I3Position &endPosCE)");

  bp::def("dists_to_intersections_with_sphere", EarthModelCalculator::GetDistsToIntersectionsWithSphere,
      "EarthModelCalculator::GetIntersectionsWithSphere(const I3Position &impact_pos_CE, const I3Direction &dirCE, double r, double &startdist, double &enddist)");


  bp::def("lepton_range", EarthModelCalculator::GetLeptonRange,
      "EarthModelCalculator::GetLeptonRange(double particle_energy, bool istau, LeptonRangeOption option, double scale)");

  bp::def("column_depth_cgs_to_mwe", EarthModelCalculator::ColumnDepthCGStoMWE,
      "EarthModelCalculator::ColumnDepthCGStoMWE(double cdep_CGS)");

  bp::def("mwe_to_column_depth_cgs", EarthModelCalculator::MWEtoColumnDepthCGS,
      "EarthModelCalculator::MWEtoColumnDepthCGS(double cdep_MWE)");

}
