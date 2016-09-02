/** 
 *  $Id: $
 *  @file Steering.cxx
 *  @version $Revision:  $
 *  @date    $Date:      $
 *
 *  Copyright (C) 2005
 *  the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  Aya Ishihara  <aya.ishihara@icecube.wisc.edu>
 *  modified by Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 */

#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/utils/Defaults.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"
#include "neutrino-generator/interactions/InteractionManagerBase.h"
#include "phys-services/surfaces/Cylinder.h"
#include "phys-services/surfaces/AxialCylinder.h"
#include "icetray/I3Units.h"
#include <iostream>
#include <algorithm>
#include <cfloat>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <iostream>

using namespace earthmodel;
using namespace std;

namespace nugen {

//______________________________________________________________
Steering::Steering(const I3Context &c) :
   I3ServiceBase(c), 
   numberOfEvents_(Defaults::nEvts),
   max_detection_volume_len_(Defaults::maxDetVolLen),
   simModeString_(Defaults::simModeString),
   simMode_(Defaults::simMode),
   vtxGenModeString_(Defaults::vtxGenModeString),
   vtxGenMode_(Defaults::vtxGenMode),
   vtxEThreshold_(Defaults::vtxGenEThreshold),
   benchmarkModeString_(Defaults::benchmarkModeString),
   benchmarkMode_(Defaults::benchmarkMode),
   earthname_(""),
   mctreename_(Defaults::mcTreeName),
   weightname_(Defaults::mcWeightDictName),
   
   worldRadius_(-1),

   injectionModeString_(Defaults::injectionModeString),  
   injectionMode_(Defaults::injectionMode),  
   detectionSurface_(Defaults::detectionSurface),  
   cylinderParams_(5, -DBL_MAX),  

   doMuRangeExtension_(Defaults::doMuRangeExtension),
   muRangeOpt_(Defaults::muRangeOpt),
   stepsize_(Defaults::stepSize),
   finestepsize_(Defaults::fineStepSize),
   densityTolerance_(Defaults::densityTolerance),
   global_xsec_scales_vec_(3, 1),
   psurvApproxLimit_(Defaults::psurvApproxLimit),
   useSimpleScatterForm_(Defaults::useSimpleScatterForm),
   ignoreOutgoingAngleForNC_(Defaults::ignoreOutgoingAngleForNC)
{
   AddParameter("EarthModelName", "Name of EarthModelService. ",
                earthname_);

   AddParameter("NEvents", "Number of neutrino events generated. "
                "Users must set non-zero integer value, then NuGen"
                " suspends icetray after NEvents has been generated.",
                numberOfEvents_);

   AddParameter("SimMode", 
                "Select one from Full, InEarth, FinalOnly",
                 simModeString_);

   AddParameter("InjectionMode",
                "Circle : same as legacy nugen, Surface: injects "
                "area defined MuonGun::Surface around IceCube, "
                "Dynamic : Energy dependent, Not yet implemented ",
                injectionModeString_);

   AddParameter("DetectionSurface",
                "Surface bounding the active volume where the "
                " final neutrino interaction will be forced."
                " If you set CylinderParams, the surface set via this interface will be ignored.",
                detectionSurface_);

   AddParameter("CylinderParams",
                "vector of doubles. If you set it with Surface injection mode, your DetectionSurface set above will be ignored.\n"
                "For Surface mode: p[0] radius, p[1] height, p[2] cylinder center x, p[3] cylinder center y, p[4] cylinder center z\n"
                "For Circle mode : p[0] radius, p[1] active_height_before, p[2] active_height_after",
                cylinderParams_);

   AddParameter("WorldRadius",
                "radius of NuGen simulation world: your detection "
                "volume must be within this radius from the "
                "center of the Earth. "
                "If you leave it as default the IceAirBoundary in "
                "your earthmodel-service will be set automatically. ",
                worldRadius_);

   AddParameter("DoMuonRangeExtension", "Default TRUE, don't touch it "
                              "unless you really want to switch off " 
                              "automatic extension of detection volume " 
                              "for NuMu and NuTau ",
                               doMuRangeExtension_);

   AddParameter("GlobalXsecScaleFactor", 
                 "Cross section scale factor, must be a "
                 "3-dim vector<double> for CC, NC, GR",
                 global_xsec_scales_vec_);


   AddParameter("VTXGenMode", 
                "You may choose final interaction generator from "
                "NuGen, Genie, or Hybrid.",
                 vtxGenModeString_);

   AddParameter("VTXGenEnergyThreshold", 
                "Works with VTXGenerator hybrid mode only. "
                "Energy threshold to switch NuGen and Genie.",
                 vtxEThreshold_);

   AddParameter("BenchMarkMode", 
                "*THIS OPTION IS UNDER CONSTRUCTION LEAVE IT AS DEFAUT* "
                "Off : physics run mode (default) "
                "DetEdge : inject from detector edge "
                "InDet : secondary is generated inside detector "
                "InDetPhys : vtx is inside detector ",
                 benchmarkModeString_);

   AddParameter("MaxDetectionVolumeLen",
                "Maximum length of detection volume",
                max_detection_volume_len_);

   AddParameter("MCTreeName",
                "Name of MCTree",
                mctreename_);

   AddParameter("WeightDictName",
                "Name of MCWeightDict",
                weightname_);

   AddParameter("PsurvApproxLimit", "Do Not touch the value unless"
                              "you really understand the option.", 
                               psurvApproxLimit_);

   AddParameter("UseSimpleScatterForm", "Do you use simple form"
                              " to calculate outgoing particle's angle?", 
                               useSimpleScatterForm_);

   AddParameter("IgnoreOutgoingAngleForNC", "Do you ignore " 
                              " outgoing angle for NC?", 
                               ignoreOutgoingAngleForNC_);

   // these params may be obsolate in future

   AddParameter("MuonRangeOpt", "Do Not touch the value unless"
                                "you really understand the option." 
                                "See descriptions of SetMuonRangeOpt()",
                                 muRangeOpt_);
   AddParameter("StepSize", "Do Not touch the value unless"
                              "you really understand the option.", 
                               stepsize_);
   AddParameter("FineStepSize", "Do Not touch the value unless"
                              "you really understand the option.", 
                               finestepsize_);
   AddParameter("DensityTolerance", "Do Not touch the value unless"
                              "you really understand the option.", 
                               densityTolerance_);

}

//______________________________________________________________
Steering::Steering(EarthModelServicePtr earth) :
   I3ServiceBase("NuGSteering"), 
   numberOfEvents_(Defaults::nEvts),
   max_detection_volume_len_(Defaults::maxDetVolLen),
   simModeString_(Defaults::simModeString),
   simMode_(Defaults::simMode),
   vtxGenModeString_(Defaults::vtxGenModeString),
   vtxGenMode_(Defaults::vtxGenMode),
   vtxEThreshold_(Defaults::vtxGenEThreshold),
   benchmarkModeString_(Defaults::benchmarkModeString),
   benchmarkMode_(Defaults::benchmarkMode),
   earthname_(""),
   mctreename_(Defaults::mcTreeName),
   weightname_(Defaults::mcWeightDictName),

   worldRadius_(-1),

   injectionModeString_(Defaults::injectionModeString),  
   injectionMode_(Defaults::injectionMode),  
   detectionSurface_(Defaults::detectionSurface),
   cylinderParams_(5, -DBL_MAX),  

   doMuRangeExtension_(Defaults::doMuRangeExtension),
   muRangeOpt_(Defaults::muRangeOpt),
   stepsize_(Defaults::stepSize),
   finestepsize_(Defaults::fineStepSize),
   densityTolerance_(Defaults::densityTolerance),
   global_xsec_scales_vec_(3, 1),
   psurvApproxLimit_(Defaults::psurvApproxLimit),
   useSimpleScatterForm_(Defaults::useSimpleScatterForm),
   ignoreOutgoingAngleForNC_(Defaults::ignoreOutgoingAngleForNC),
   earth_ptr_(earth)
{
   CheckParameters();
}

//______________________________________________________________
Steering::Steering(EarthModelServicePtr earth,
                   SimMode simMode,
                   VTXGenMode vtxGenMode,
                   InjectionMode injectionMode):
   I3ServiceBase("NuGSteering"), 
   numberOfEvents_(Defaults::nEvts),
   max_detection_volume_len_(Defaults::maxDetVolLen),
   simModeString_(nugen::GetSimModeString(simMode)),
   simMode_(simMode),
   vtxGenModeString_(nugen::GetVTXGenModeString(vtxGenMode)),
   vtxGenMode_(vtxGenMode),
   vtxEThreshold_(Defaults::vtxGenEThreshold),
   benchmarkModeString_(Defaults::benchmarkModeString),
   benchmarkMode_(Defaults::benchmarkMode),
   earthname_(Defaults::earthModelName),
   mctreename_(Defaults::mcTreeName),
   weightname_(Defaults::mcWeightDictName),

   worldRadius_(-1),

   injectionModeString_(nugen::GetInjectionModeString(injectionMode)),  
   injectionMode_(injectionMode),  
   detectionSurface_(Defaults::detectionSurface),
   cylinderParams_(5, -DBL_MAX),  

   doMuRangeExtension_(Defaults::doMuRangeExtension),
   muRangeOpt_(Defaults::muRangeOpt),
   stepsize_(Defaults::stepSize),
   finestepsize_(Defaults::fineStepSize),
   densityTolerance_(Defaults::densityTolerance),
   global_xsec_scales_vec_(3, 1),
   psurvApproxLimit_(Defaults::psurvApproxLimit),
   useSimpleScatterForm_(Defaults::useSimpleScatterForm),
   ignoreOutgoingAngleForNC_(Defaults::ignoreOutgoingAngleForNC),
   earth_ptr_(earth)
{
   CheckParameters();
}

//______________________________________________________________
Steering::~Steering()
{
}

//___________________________________________________________________
void Steering::Configure()
{
   log_debug("Configuring I3NuGSteeringFactory");

   GetParameter("EarthModelName", earthname_);
   GetParameter("NEvents", numberOfEvents_);
   GetParameter("SimMode", simModeString_);
   GetParameter("InjectionMode", injectionModeString_);
   GetParameter("DetectionSurface", detectionSurface_);
   GetParameter("CylinderParams", cylinderParams_);
   GetParameter("WorldRadius", worldRadius_);
   GetParameter("DoMuonRangeExtension", doMuRangeExtension_);
   GetParameter("GlobalXsecScaleFactor", global_xsec_scales_vec_);

   GetParameter("VTXGenMode", vtxGenModeString_);
   GetParameter("VTXGenEnergyThreshold", vtxEThreshold_);
   GetParameter("BenchmarkMode", benchmarkModeString_);
   GetParameter("MaxDetectionVolumeLen", max_detection_volume_len_);
   GetParameter("MCTreeName", mctreename_);
   GetParameter("WeightDictName", weightname_);
   GetParameter("PsurvApproxLimit", psurvApproxLimit_);
   GetParameter("UseSimpleScatterForm", useSimpleScatterForm_);
   GetParameter("IgnoreOutgoingAngleForNC", ignoreOutgoingAngleForNC_);
   GetParameter("MuonRangeOpt", muRangeOpt_);
   GetParameter("StepSize", stepsize_);
   GetParameter("FineStepSize", finestepsize_);
   GetParameter("DensityTolerance", densityTolerance_);

   if (earthname_ == "") {
      log_error("you have to set EarthModelName");
   } else {
      earth_ptr_ = context_.Get<EarthModelServicePtr>(earthname_); 
      if (!earth_ptr_) {
         log_error("your EarthModel %s does not exist yet."
                   "You have to add it before adding the "
                   "I3NuGSteeringService.", earthname_.c_str());
      }
   }

   CheckParameters();
   PrintSetting();
}

//______________________________________________________________
const double Steering::GetInjectionRadius() const 
{
   if (injectionMode_ != CIRCLE) return -1;
   return cylinderParams_[0];
}

//______________________________________________________________
const double Steering::GetActiveHeightBefore() const 
{
   if (injectionMode_ != CIRCLE) return -1;
   return cylinderParams_[1];
}

//______________________________________________________________
const double Steering::GetActiveHeightAfter() const 
{
   if (injectionMode_ != CIRCLE) return -1;
   return cylinderParams_[2];
}

//______________________________________________________________
const double Steering::GetCylinderRadius() const
{
   if (injectionMode_ == CIRCLE) return -1;
   I3Surfaces::CylinderPtr cyl = boost::dynamic_pointer_cast<I3Surfaces::Cylinder>(detectionSurface_);
   if (!cyl) return -1;
   double radius = cyl->GetRadius();
   return radius;
}

//______________________________________________________________
const double Steering::GetCylinderHeight() const
{
   if (injectionMode_ == CIRCLE) return -1;
   I3Surfaces::CylinderPtr cyl = boost::dynamic_pointer_cast<I3Surfaces::Cylinder>(detectionSurface_);
   if (!cyl) return -1;
   double length = cyl->GetLength();
   return length;
}

//______________________________________________________________
const I3Position Steering::GetCylinderOrigin() const
{
   if (injectionMode_ == CIRCLE) return I3Position();
   I3Surfaces::CylinderConstPtr cyl = boost::dynamic_pointer_cast<I3Surfaces::Cylinder>(detectionSurface_);
   if (!cyl) return I3Position();
   return cyl->GetCenter();
}

//______________________________________________________________
void Steering::CheckParameters() 
{

   simMode_ = ToSimMode(simModeString_);
   vtxGenMode_ = ToVTXGenMode(vtxGenModeString_);
   benchmarkMode_ = ToBenchMode(benchmarkModeString_);
   injectionMode_ = ToInjectionMode(injectionModeString_);

   if (worldRadius_ < 0) {
      // set world radius from earthmodel service
      worldRadius_ = earth_ptr_->GetIceAirBoundary();
   }

   // Create new serface for CIRCLE option, or 
   // replace cylinder for Surface and Dynamic option (if needed).
   // cylinderParams_[0] : cylinder radius or circle radius
   // cylinderParams_[1] : cylinder height or active length before 
   // cylinderParams_[2] : center pos x or active length after 
   // cylinderParams_[3] : center pos y 
   // cylinderParams_[4] : center pos z 

   if (injectionMode_ == CIRCLE) {
      // selected CIRCLE mode. Since default surface is cylinder, 
      // recreate surface here.

      double circlerad = Defaults::injectionRadius;
      double activeheightbefore = Defaults::activeHeightBefore;
      double activeheightafter = Defaults::activeHeightAfter;
      if (cylinderParams_[0] > 0)  circlerad = cylinderParams_[0];
      if (cylinderParams_[1] > 0)  activeheightbefore = cylinderParams_[1];
      if (cylinderParams_[2] > 0)  activeheightafter = cylinderParams_[2];

      // Unfortunately, AxialCylinder does not have any public function
      // to get cylinder parameters. for now, save parameters to
      // cylinderParams_...
      cylinderParams_[0] = circlerad;
      cylinderParams_[1] = activeheightbefore;
      cylinderParams_[2] = activeheightafter;

      // generate AxialCylinder, which is same as old NuGen
      // injection volume
      detectionSurface_ = boost::make_shared<I3Surfaces::AxialCylinder>(
                     activeheightbefore, activeheightafter, circlerad);
      //cerr << "Cylinder generated with active hight " << activeheightbefore << " radius " << circlerad << endl;

   } else {

      if (cylinderParams_[0] > 0 && cylinderParams_[1] > 0) {

         // This is SURFACE option and new cylinder params are set. 
         // Recreate cylinder surface.
         // If cylinderParams_[2]~[4] is not set just use
         // origin for center pos.
         cylinderParams_[2] = (cylinderParams_[2] == -DBL_MAX ? 0 : cylinderParams_[2]);
         cylinderParams_[3] = (cylinderParams_[3] == -DBL_MAX ? 0 : cylinderParams_[3]);
         cylinderParams_[4] = (cylinderParams_[4] == -DBL_MAX ? 0 : cylinderParams_[4]);
         detectionSurface_ = 
            I3Surfaces::SamplingSurfacePtr(new I3Surfaces::Cylinder(
                                        cylinderParams_[1], 
                                        cylinderParams_[0],
                                        I3Position(cylinderParams_[2],
                                                   cylinderParams_[3],
                                                   cylinderParams_[4])));
      } else {
         // just set dummy value
         cylinderParams_[0] = -DBL_MAX;
         cylinderParams_[1] = -DBL_MAX;
         cylinderParams_[2] = -DBL_MAX;
         cylinderParams_[3] = -DBL_MAX;
         cylinderParams_[4] = -DBL_MAX;
      } 
   }

   if (benchmarkMode_ != OFF) { 
      injectionMode_ = SURFACE;
      if (detectionSurface_ == NULL) {
         log_error("You chose benchmark mode, "
                 " then you have to set DetectionSurface.");
      }
   }
   global_xsec_scales_[CC] = global_xsec_scales_vec_[0];
   global_xsec_scales_[NC] = global_xsec_scales_vec_[1];
   global_xsec_scales_[GR] = global_xsec_scales_vec_[2];
}

//______________________________________________________________
void Steering::PrintSetting() 
{
   log_info("===========================================");
   log_info("NuGen Steering Setting");
   log_info("-------------------------------------------");
   log_info("NGen                 %d", numberOfEvents_);
   log_info("SimMode              %s, %d", simModeString_.c_str(), simMode_);
   log_info("VTXGenMode           %s, %d", vtxGenModeString_.c_str(), vtxGenMode_);
   log_info("VTXEnergyThreshold   %f", vtxEThreshold_);
   log_info("BenchmarkMode        %s, %d", benchmarkModeString_.c_str(), benchmarkMode_);
   log_info("MCTreeName           %s", mctreename_.c_str());
   log_info("WeightsName          %s", weightname_.c_str());
   log_info("WorldRadius          %f", worldRadius_);
   log_info("InjectionMode        %s, %d", injectionModeString_.c_str(), injectionMode_);
   log_info("--CylinderParams could be -DBL_MAX if not defined--");
   log_info("CylinderParams[0]    %5.1g", cylinderParams_[0]);
   log_info("CylinderParams[1]    %5.1g", cylinderParams_[1]);
   log_info("CylinderParams[2]    %5.1g", cylinderParams_[2]);
   log_info("CylinderParams[3]    %5.1g", cylinderParams_[3]);
   log_info("CylinderParams[4]    %5.1g", cylinderParams_[4]);
   log_info("MuRangeExtension     %d", doMuRangeExtension_);
   log_info("MuRangeOpt           %f", muRangeOpt_);
   log_info("StepSize             %f", stepsize_);
   log_info("FineStepSize         %f", finestepsize_);
   log_info("densityTolerance     %f", densityTolerance_);
   log_info("psurvApproxLimit     %f", psurvApproxLimit_);
   log_info("useSimpleScatterForm %d", useSimpleScatterForm_);
   log_info("IgnoreOutgoingAngNC  %d", ignoreOutgoingAngleForNC_);
   log_info("GlobalXsecScaleFactorCC %f", global_xsec_scales_[CC]);
   log_info("GlobalXsecScaleFactorNC %f", global_xsec_scales_[NC]);
   log_info("GlobalXsecScaleFactorGR %f", global_xsec_scales_[GR]);
   log_info("===========================================");
}

//______________________________________________________________
EarthModelServicePtr Steering::GetEarthModelService() const
{
   if (!earth_ptr_) {
      log_error("The Steering class doesn't retain valid pointer "
                "to EarthModelService. You have to set it manually "
                "or add I3EarthModelServiceFactory first, then add "
                "I3NuGSteeringFactory second, then use any "
                "NuGen module.");
   }
   return earth_ptr_;
}

//______________________________________________________________
void Steering::AddInteractionManager(VTXGenMode mode, 
                  InteractionManagerBasePtr manager) 
{
   managers_[mode] = manager;
}

//______________________________________________________________
InteractionManagerBasePtr Steering::GetInteractionManager(const Particle &p) 
{
   if (!managers_.size()) {
      log_error("at least one InteractionManager must be set.");
   }
   if (vtxGenMode_ != HYBRID) {
      if ( managers_.count(vtxGenMode_)) {
         return managers_[vtxGenMode_];
      } else {
         log_error("InteractionManager is not set for mode %s", 
                    GetVTXGenModeString(vtxGenMode_).c_str());
      }
   } 

   // check energy for hyblid.
   double energy = p.GetEnergy();
   if (energy < vtxEThreshold_) return managers_[GENIE];
   return managers_[NUGEN];
}

//______________________________________________________________
void Steering::AddParticle(std::string pname, ParticlePtr p)
{
   particles_[pname] = p;
}

//______________________________________________________________
ParticlePtr Steering::GetParticle(std::string pname) 
{
   if (!particles_.count(pname)) {
      log_error("particle %s does not exist", pname.c_str());
   }
   return particles_[pname];
}

}

typedef I3SingleServiceFactory<nugen::Steering> I3NuGSteeringFactory;
I3_SERVICE_FACTORY(I3NuGSteeringFactory);
