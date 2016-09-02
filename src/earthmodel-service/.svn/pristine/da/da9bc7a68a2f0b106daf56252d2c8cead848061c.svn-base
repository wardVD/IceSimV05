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

#include "icetray/I3ServiceBase.h"
#include <earthmodel-service/EarthModelService.h>
#include <string>
#include <vector>
#include <map>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

using namespace boost::python;

std::string get_path(const earthmodel::EarthModelService& self) {
  return self.GetPath();
}

std::string get_ice_cap_type_string(const earthmodel::EarthModelService& self) {
  return self.GetIceCapTypeString();
}

void set_path(earthmodel::EarthModelService& self, const std::string &mypath) {
  self.SetPath(mypath);
}

void set_ice_cap_type_string(earthmodel::EarthModelService& self, const std::string &mymodel) {
  self.SetIceCapTypeString(mymodel);
}

void register_EarthModelService()
{
  using namespace earthmodel;

  enum_<EarthModelService::MediumType>("MediumType")
    .value("innercore", EarthModelService::INNERCORE)
    .value("outercore", EarthModelService::OUTERCORE)
    .value("mantle", EarthModelService::MANTLE)
    .value("rock", EarthModelService::ROCK)
    .value("ice", EarthModelService::ICE)
    .value("air", EarthModelService::AIR)
    .value("vacuum", EarthModelService::VACUUM)
    .export_values()
    ;

  enum_<EarthModelService::IceCapType>("IceCapType")
    .value("noice", EarthModelService::NOICE)
    .value("icesheet", EarthModelService::ICESHEET)
    .value("simpleicecap", EarthModelService::SIMPLEICECAP)
    .export_values()
    ;

  enum_<EarthModelService::IntegType>("IntegType")
    .value("path", EarthModelService::PATH)
    .value("radius", EarthModelService::RADIUS)
    .value("circle", EarthModelService::CIRCLE)
    .value("sphere", EarthModelService::SPHERE)
    .export_values()
    ;



  // overloading functions
  const I3Position (EarthModelService::* earthpos_from_i3pos)(const I3Position&) const = &EarthModelService::GetEarthCoordPosFromDetCoordPos;

  const I3Position (EarthModelService::* i3pos_from_earthpos)(const I3Position&) const = &EarthModelService::GetDetCoordPosFromEarthCoordPos;

  const I3Direction (EarthModelService::* earthdir_from_i3dir)(const I3Direction&) const = &EarthModelService::GetEarthCoordDirFromDetCoordDir;

  const I3Direction (EarthModelService::* i3dir_from_earthdir)(const I3Direction&) const = &EarthModelService::GetDetCoordDirFromEarthCoordDir;

  const double (EarthModelService::* earth_density)(const I3Position&) const = &EarthModelService::GetEarthDensityInCGS;

  {
  // class
  class_<EarthModelService::EarthParam, boost::shared_ptr<EarthModelService::EarthParam> >
      ("EarthParam", init<>())
      .def(init<const EarthModelService::EarthParam&>())
      .def_readwrite("upper_radius",&EarthModelService::EarthParam::fUpperRadius_)
      .def_readwrite("z_offset",&EarthModelService::EarthParam::fZOffset_)
      .def_readwrite("boundary_name",&EarthModelService::EarthParam::fBoundaryName_)
      .def_readwrite("medium_type",&EarthModelService::EarthParam::fMediumType_)
      .def_readwrite("params",&EarthModelService::EarthParam::fParams_)
      .def("get_density",&EarthModelService::EarthParam::GetDensity)
      .def("print_density",&EarthModelService::EarthParam::PrintDensity)
      ;

  class_<EarthModelService, boost::shared_ptr<EarthModelService>, boost::noncopyable >
         ("EarthModelService", init< optional<const std::string &, const std::string &,
          const std::vector<std::string> &, const std::vector<std::string> &, 
          const std::string &, double, double> >())
     .def(init<const I3Context &>())
     .def("earth_coord_pos_from_det_coord_pos", earthpos_from_i3pos)
     .def("det_coord_pos_from_earth_coord_pos", i3pos_from_earthpos)
     .def("earth_coord_dir_from_det_coord_dir", earthdir_from_i3dir)
     .def("det_coord_dir_from_earth_coord_dir", i3dir_from_earthdir)
     .def("earth_density_in_cgs", earth_density)
     .def("set_detector_xy", &EarthModelService::SetDetectorXY)
     .def("distance_from_earth_entrance_to_detector", &EarthModelService::GetDistanceFromEarthEntranceToDetector)
     .def("distance_from_sphere_surface_to_detector", &EarthModelService::GetDistanceFromSphereSurfaceToDetector)
     .def("lepton_range_in_meter_from", &EarthModelService::GetLeptonRangeInMeterFrom)
     .def("lepton_range_in_meter_to", &EarthModelService::GetLeptonRangeInMeterTo)
     .def("column_depth_in_cgs", &EarthModelService::GetColumnDepthInCGS)
     .def("integrate_density_in_cgs", &EarthModelService::IntegrateDensityInCGS)
     .def("distance_for_column_depth_to_point", &EarthModelService::DistanceForColumnDepthToPoint)
     .def("distance_for_column_depth_from_point", &EarthModelService::DistanceForColumnDepthFromPoint)
     .def("print_earth_params", &EarthModelService::PrintEarthParams)
     .def("prem", &EarthModelService::GetPREM)
     .def("medium", &EarthModelService::GetMedium)
     .def("boundary", &EarthModelService::GetBoundary)
     .def("distance_to_next_boundary_crossing", &EarthModelService::DistanceToNextBoundaryCrossing)
     .def("get_earth_param",&EarthModelService::GetEarthParam,return_internal_reference<1>())
     .def("get_earth_params_list",&EarthModelService::GetEarthParamsList)

     .add_property("path", &get_path, &set_path)
     .add_property("detector_depth", &EarthModelService::GetDetectorDepth, &EarthModelService::SetDetectorDepth)
     .add_property("ice_cap_type_string", &get_ice_cap_type_string, set_ice_cap_type_string)
     .add_property("ice_cap_simple_angle", &EarthModelService::GetIceCapSimpleAngle, &EarthModelService::SetIceCapSimpleAngle)

     .add_property("moho_boundary", &EarthModelService::GetMohoBoundary)
     .add_property("rock_ice_boundary", &EarthModelService::GetRockIceBoundary)
     .add_property("ice_air_boundary", &EarthModelService::GetIceAirBoundary)
     .add_property("atmo_radius", &EarthModelService::GetAtmoRadius)
     ;
      

    //class_<std::vector<EarthModelService::EarthParam> >("std::vector<EarthModelService::EarthParam>")
    //  .def(vector_indexing_suite<std::vector<EarthModelService::EarthParam> >());

  }
}
