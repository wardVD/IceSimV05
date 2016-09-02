/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file EarthModelService.cxx
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Kotoyo Hoshina <kotoyo.hoshina@icecube.wisc.edu>
 * 
 *   @brief EarthModelService manages density profile
 *   of the Earth.
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include "icetray/I3Units.h"
#include "dataclasses/I3Constants.h"
#include "earthmodel-service/EarthModelService.h"
#include "earthmodel-service/ParticleConstants.h"
#include "icetray/I3SingleServiceFactory.h"

using namespace std;
using namespace earthmodel;

//
// These PREM parameters are solid 
// 
static const double PREM_EARTH_RADIUS = 6371000; // [m]
static const int    CHAR_BUF_SIZE = 8196; // 

// unit conversion
static const double M_TO_CM = (I3Units::m / I3Units::cm); 
static const double CM_TO_M = (I3Units::cm / I3Units::m); 

//__________________________________________________________
EarthModelService::EarthModelService(const I3Context &c) 
  : I3ServiceBase(c),
    fPath_(""),
    fMohoBoundary_(-1),
    fRockIceBoundary_(-1),
    fIceAirBoundary_(-1),
    fAtmoRadius_(-1),
    fDetDepth_(1948.0 * I3Units::m),
    fIceCapTypeString_("SimpleIceCap"),
    fIceCapSimpleAngle_(20.0*I3Units::degree),
    fIceCapSimpleRadius_(-1), // will be recaluclated if needed 
    fIceCapSimpleZshift_(0) // must be zero, will be recalculated if needed
{
   log_debug("Constructing EarthModelService");

   AddParameter("PathToDataFileDir",
                "Path to the directory containing the "
                "data files",
                fPath_);

   AddParameter("EarthModels", 
                "vector of basenames of EarthModel param file. "
                "ex. PREM_mmc.dat -> PREM_mmc "
                "suffix of the param file must be .dat ",
                fEarthModelStrings_);

   AddParameter("MaterialModels", 
                "vector of basenames of MaterialRatio param file. "
                "ex. Standard.dat -> Standard"
                "suffix of the param file must be .dat ",
                fMatRatioStrings_);

   AddParameter("IceCapType", 
                "option for choosing the shape of IceCap. "
                "If 'SimpleIceCap' is selected, it simulates spherical "
                "icecap around the SouthPole. "
                "Default is 'IceSheet', which simulates the Earth "
                "totally wrapped with a sheet of 2.8km ice.",
                fIceCapTypeString_);

   AddParameter("IceCapSimpleAngle", 
                "Valid only when IceCapType is 'SimpleIceCap'. "
                "Default: 20degree from the South Pole. ",
                fIceCapSimpleAngle_);

   AddParameter("DetectorDepth", "Depth of origin of IceCube "
                "coordinate, measured from ice surface",  
                fDetDepth_);

}

//__________________________________________________________
EarthModelService::EarthModelService(
          const std::string &name,
          const std::string &tablepath,
          const std::vector<std::string>& earthmodels,
          const std::vector<std::string>& materialmodels,
          const std::string &icecapname,
          double icecapangle,
          double detectordepth)
    : I3ServiceBase(name), 
      fPath_(tablepath), 
      fEarthModelStrings_(earthmodels),
      fMatRatioStrings_(materialmodels),
      fMohoBoundary_(-1),
      fRockIceBoundary_(-1),
      fIceAirBoundary_(-1),
      fAtmoRadius_(-1),
      fDetDepth_(detectordepth),
      fIceCapTypeString_(icecapname),
      fIceCapSimpleAngle_(icecapangle),
      fIceCapSimpleRadius_(-1), // will be recaluclated if needed 
      fIceCapSimpleZshift_(0) // must be zero, will be recalculated if needed
{
   Init();
}

//__________________________________________________________
EarthModelService::~EarthModelService()
{
}

//___________________________________________________________________
void EarthModelService::Init()
{
   if (fPath_ == "") {
      string path(getenv("I3_BUILD"));
      if (path == "") {
         path = "./";
      } 
      path += "/earthmodel-service/resources/earthparams/";
      fPath_ = path;
   }

   if (fEarthModelStrings_.size() < 1) {
      // set default PREM+mmc rock
      fEarthModelStrings_.push_back("PREM_mmc");
   }

   if (fMatRatioStrings_.size() < 1) {
      // set default materials
      fMatRatioStrings_.push_back("Standard");
   }

   //-----------------------------------------
   // Set models. By calling these functions
   // default values set above may be modified
   // depends on which crust model you choose.
   //
   SetEarthModel(fEarthModelStrings_);
   SetMaterialModel(fMatRatioStrings_);

   //-----------------------------------------
   // following setting must be done after 
   // SetEarthModel is called

   // if ice type is SimpleIceCap, recalculate iceparams and z-shift etc.
   SetIceCapTypeString(fIceCapTypeString_);
   SetIceCapSimpleAngle(fIceCapSimpleAngle_);

   SetDetectorDepth(fDetDepth_);
   SetDetectorXY(0., 0.);

   log_info("%s", PrintEarthParams().c_str()); 

}

//___________________________________________________________________
void EarthModelService::Configure()
{
   log_debug("Configuring I3EarthModelServiceFactory");

   GetParameter("PathToDataFileDir", fPath_);
   GetParameter("EarthModels", fEarthModelStrings_);
   GetParameter("MaterialModels", fMatRatioStrings_);
   GetParameter("IceCapType", fIceCapTypeString_);
   GetParameter("IceCapSimpleAngle", fIceCapSimpleAngle_);
   GetParameter("DetectorDepth", fDetDepth_);

   Init();

}

//__________________________________________________________
string EarthModelService::GetMediumTypeString(EarthModelService::MediumType t) 
{
   string stype("None");
   if (t == INNERCORE) stype = "INNERCORE";
   else if (t == OUTERCORE) stype = "OUTERCORE";
   else if (t == MANTLE) stype = "MANTLE";
   else if (t == ROCK) stype = "ROCK";
   else if (t == ICE) stype = "ICE";
   else if (t == AIR) stype = "AIR";
   else if (t == VACUUM) stype = "VACUUM";
   return stype;
}

//__________________________________________________________
EarthModelService::MediumType
EarthModelService::ConvertMediumTypeString(const string &s) 
{
   MediumType t = VACUUM;
   if (s == "INNERCORE") t = INNERCORE;
   else if (s == "OUTERCORE") t = OUTERCORE;
   else if (s == "MANTLE") t = MANTLE;
   else if (s == "ROCK") t = ROCK;
   else if (s == "ICE") t = ICE;
   else if (s == "AIR") t = AIR;
   return t;
}

//__________________________________________________________
const double EarthModelService::GetBoundary(const string &s) const
{
   EarthParamMap::const_iterator i;   
   for (i=fEarthParams_.begin(); i!= fEarthParams_.end(); ++i) {
      if (i->second.fBoundaryName_ == s) return i->first; 
   }
   for (i=fIceParams_.begin(); i!= fIceParams_.end(); ++i) {
      if (i->second.fBoundaryName_ == s) return i->first;
   }
   log_error("couldn't find a boundary %s", s.c_str());
   return -1;
}

//__________________________________________________________
const double EarthModelService::RadiusToCosZen(double r) const
{
   if (r == 0) return -1;

   double l = fDetPos_.Magnitude();
   if (r > l) return 1;

   double h = sqrt((l+r)*(l-r)); // avoid round off error
   return -1*(h/l);
}

//__________________________________________________________
string EarthModelService::PrintEarthParams() const
{
// this is used for debugging.   

   ostringstream os;
   os << "****************************************" << endl;
   os << "*Print EarthParams                      " << endl;
   os << "*---------------------------------------" << endl;

   for (EarthParamMap::const_iterator i=fEarthParams_.begin(); i!= fEarthParams_.end(); ++i) {
      if(i->second.fMediumType_==AIR){ //print ice before the atmosphere

         os << "*** icecap params for zshift " << fIceCapSimpleZshift_ << " m"<< endl;
         for (EarthParamMap::const_iterator i=fIceParams_.begin(); i!= fIceParams_.end(); ++i) {
            os << "*** " << i->second.fBoundaryName_ << "\t r = "
            << i->first << "\t density = "
            << i->second.GetDensity(i->first) << "\t coszen = "
            << RadiusToCosZen(i->first) << endl;
         }
         os << "*** icecap params end " << endl;
      }
      os << "*" << i->second.fBoundaryName_ << "\t r = " 
           << i->first << "\t density = " 
           << i->second.GetDensity(i->first) << "\t coszen = " 
           << RadiusToCosZen(i->first) << endl;
   }
   os << "*MohoBoundary      " << fMohoBoundary_ << endl;
   os << "*RockIceBoundary   " << fRockIceBoundary_ << endl;
   os << "*IceAirBoundary    " << fIceAirBoundary_ << endl;
   os << "*AtmoRadius        " << fAtmoRadius_ << endl;
   os << "*DetectorDepth     " << fDetDepth_ << endl;
   os << "*IceCapType        " << fIceCapTypeString_ << endl;
   os << "*IceCapSimpleAngle " << fIceCapSimpleAngle_/I3Units::deg<< endl;
   os << "****************************************" << endl;

   return os.str();
}

//_________________________________________________________
const I3Position EarthModelService::GetEarthCoordPosFromDetCoordPos(
                                const I3Position &p) const
{
   return p + fDetPos_;
}

//_________________________________________________________
const I3Position EarthModelService::GetDetCoordPosFromEarthCoordPos(
                                const I3Position &p) const
{
   I3Position pI3 = p - fDetPos_;
   return pI3;
}

//_________________________________________________________
const I3Direction EarthModelService::GetEarthCoordDirFromDetCoordDir(
                                const I3Direction &p) const
{
   // currently no conversion is applied.
   return p;
}

//_________________________________________________________
const I3Direction EarthModelService::GetDetCoordDirFromEarthCoordDir(
                                const I3Direction &p) const
{
   // currently no conversion is applied.
   return p;
}

//______________________________________________________________________
const EarthModelService::EarthParam&
EarthModelService::GetEarthParam(const I3Position & p_CE) const 
{
//
// argument must be in Earth Center coordinate
//

   double r = p_CE.Magnitude();

   // find an EarthParam
   EarthParamMap::const_iterator iter = fEarthParams_.lower_bound(r);

   if (iter == fEarthParams_.end()) {
      --iter;
      log_error("radius %f is not defined in EarthParam, largest radius is %f", r, iter->first);
   }

   const EarthParam& ep = iter->second;
   
   if (r < fRockIceBoundary_) //below all ice everything is simple
      return ep;
   
   if (fIceCapType_ != EarthModelService::SIMPLEICECAP) {
      //check whether there is an ice layer which more tightly contains this point
      EarthParamMap::const_iterator iceIter = fIceParams_.lower_bound(r);
      if(iceIter!=fIceParams_.end() && iceIter->second.fUpperRadius_<ep.fUpperRadius_)
         return iceIter->second;
      return ep;
   }

   // -----------------------------------------------
   //  With ice simple cap, above bed rock
   // -----------------------------------------------
   //compute the radius with respect to the icecap center
   const double rIce=(p_CE-I3Position(0,0,fIceCapSimpleZshift_)).Magnitude();
   
   //find out whether there is any ice layer which we are inside
   EarthParamMap::const_iterator iceIter = fIceParams_.lower_bound(rIce);
   if (iceIter==fIceParams_.end()) {
      return ep; //there wasn't
   }
   return iceIter->second; //there was
}

//______________________________________________________________________
const EarthModelService::MediumType EarthModelService::GetMedium(
                            const I3Position & p_CE) const 
{
   EarthParam ep = EarthModelService::GetEarthParam(p_CE);
   return ep.fMediumType_;
}

//__________________________________________________________
double EarthModelService::GetEarthDensityInCGS(const EarthParam& ep, const I3Position &p_CE)
{
   double r = (p_CE-I3Position(0,0,ep.fZOffset_)).Magnitude();
   return ep.GetDensity(r);
}

//__________________________________________________________
const double EarthModelService::GetEarthDensityInCGS(const I3Position &p_CE) const
{
// 
// CAUTION !
// This function returns density in g/cm3!!
//
   const EarthParam& ep = EarthModelService::GetEarthParam(p_CE);
   return GetEarthDensityInCGS(ep, p_CE);
}

//______________________________________________________________________
const std::map<int, double> &
EarthModelService::GetMatRatioMap(MediumType med) const
{
   MatRatioMap::const_iterator iter = fMatRatioMap_.find(med);
   if (iter == fMatRatioMap_.end()) {
      log_error("material %d is not set. check material type or "
                "your material file.", med);
   }
   return iter->second;
}

//______________________________________________________________________
double EarthModelService::GetMatRatio(MediumType med, int id) const
{
   const std::map<int, double> &map = GetMatRatioMap(med);
   std::map<int, double>::const_iterator iter = map.find(id);
   if (iter == map.end()) {
      log_error("material id %d is not set. check material type or "
                "your material file.", id);
   }
   return iter->second;
}

//______________________________________________________________________
const std::map<int, double> &
EarthModelService::GetPNERatioMap(MediumType med) const
{
   MatRatioMap::const_iterator iter = fPNERatioMap_.find(med);
   if (iter == fPNERatioMap_.end()) {
      log_error("material %d is not set. check material type or "
                "your material file.", med);
   }
   return iter->second;
}

//______________________________________________________________________
double EarthModelService::GetPNERatio(MediumType med, int id) const
{
   const std::map<int, double> &map = GetPNERatioMap(med);
   std::map<int, double>::const_iterator iter = map.find(id);
   if (iter == map.end()) {
      log_error("material id %d is not set. check material type or "
                "your material file.", id);
   }
   return iter->second;
}

//__________________________________________________________
const double EarthModelService::GetPREM(double r) const
{
//
// this function is no longer used, but keep it
// for debug purpose
//

   double dens = -1.;
   double x = r/PREM_EARTH_RADIUS; // 6371km : Earth Radius in PREM model

   if (r < 1221500) { // inner core
      dens = 13.0885 - 8.8381*x*x;

   } else if (r < 3480000) { // outer core
      dens = 12.5815 - x*(1.2638 + x*(3.6426 + x*5.5281));

   } else if (r < 5701000) { // lower mantle 
      dens = 7.9565 - x*(6.4761 - x*(5.5283 - x*3.0807));

   } else if (r < 5771000) { // lower transition zone
      dens = 5.3197 - 1.4836*x;

   } else if (r < 5971000) { // middle transition zone
      dens = 11.2494 - 8.0298*x;

   } else if (r < 6151000) { // upper transition zone
      dens = 7.1089 - 3.8045*x;  

   } else { // low velocity zone and LID
      dens = 2.691 + 0.6924*x; 
   }

   if (dens >= 0) return dens;     // g/cm3
   else log_error("(up to moho) R=%f, invalid earth density %f g/cm. "
                 "Did you set correct density?"
                  , r, dens);

   exit(EXIT_FAILURE); 
   return -1; // failer return 

}

//________________________________________________________________
const double EarthModelService::GetColumnDepthInCGS(
                 const  I3Position &from_posI3,
                 const  I3Position &to_posI3) const
{
  I3Position from_pos=GetEarthCoordPosFromDetCoordPos(from_posI3);
  I3Position to_pos=GetEarthCoordPosFromDetCoordPos(to_posI3); 

  return IntegrateDensityInCGS(from_pos, to_pos, PATH);
}

//________________________________________________________________
const double EarthModelService::FlatDepthCalculator(
                 const  I3Position &from_posCE,
                 const  I3Position &to_posCE,
                 double density,
                 IntegType intg_type) const
{
   double distToNext = (to_posCE - from_posCE).Magnitude();
   double depth = distToNext * M_TO_CM * density;

   if (intg_type == PATH || intg_type == RADIUS) return depth;

   // for SPHERE and CIRCLE options we just ignore the direction
   // of the track.
   double lastr_cm = from_posCE.Magnitude()*I3Units::m/I3Units::cm;
   double curr_cm = to_posCE.Magnitude()*I3Units::m/I3Units::cm;
   double pi = I3Constants::pi;

   if (intg_type == SPHERE) {
      double vol1 = 4./3.* pi * curr_cm * curr_cm * curr_cm * density;
      double vol2 = 4./3.* pi * lastr_cm * lastr_cm * lastr_cm * density;
      if (vol1 < vol2) std::swap(vol1, vol2);
      depth = (vol1 - vol2);

   } else if (intg_type == CIRCLE) {
      double sec1 = pi * curr_cm * curr_cm * density;
      double sec2 = pi * lastr_cm * lastr_cm * density;
      if (sec1 < sec2) std::swap(sec1, sec2);
      depth = (sec1 - sec2);

   } else {
      log_error("wrong intg type %d", intg_type);
   }

   return depth;
}

//________________________________________________________________
const double EarthModelService::IntegrateDensityInCGS(
                 const  I3Position &from_posCE,
                 const  I3Position &to_posCE,
                 IntegType intg_type) const
{

   I3Position pos(from_posCE);
   I3Position to_CE(to_posCE);

   if (intg_type != PATH) {
      // use radial information only
      pos = I3Position(0, 0, from_posCE.Magnitude());
      to_CE= I3Position(0, 0, to_posCE.Magnitude());
   }
  
   I3Direction dirCE(to_CE - pos);
   double len = (to_CE - pos).Magnitude();
   
   const double tinyStepSize=.01 * I3Units::m;
   
   double x=0; //distance traversed so far
   double depth=0; //column depth accumulated so far
   bool willLeaveAtmosphere=false;

   // calculate distance to next boundary
   double distToNext=DistanceToNextBoundaryCrossing(pos,dirCE,willLeaveAtmosphere);

   // get current medium and density
   EarthParam curMedium=GetEarthParam(pos);
   double density=GetEarthDensityInCGS(curMedium,pos); //the current density == d(depth)/dx

   log_trace_stream(" Now at " << pos << " having traversed " << x << " meters with " << depth << " gm/cm^2");
   log_trace_stream("  Current layer is " << curMedium.fBoundaryName_ << " with current density " << density);
   log_trace_stream("  Distance to next layer is " << distToNext << " meters");
   
   unsigned int iterations=0;
   //unsigned int maxIterations=1e6; //if we try to take more steps than this something is horribly wrong
   unsigned int maxIterations=1000; //if we try to take more steps than this something is horribly wrong

   I3Position lastpos(pos);
   bool approachingGoal=false;

   while(true){
      if(++iterations>maxIterations)
         log_fatal("Exceeded iteration count limit");
      
      //if the density is constant, take a shortcut
      if(curMedium.fParams_.size()==1){

         log_trace_stream(" Evaluating constant density material");

         density = GetEarthDensityInCGS(pos); //the current density == d(depth)/dx [g/cm3]
         approachingGoal = false;
         if(x+distToNext >= len){
            log_trace_stream("  Linearly interpolating remaining distance");
            distToNext = len-x;
            approachingGoal = true;
         }

         log_trace_stream("  Adding remaining column depth and advancing to next layer");
         lastpos = pos;
         x += distToNext;
         pos += distToNext * dirCE;

         depth += FlatDepthCalculator(lastpos, pos, density, intg_type); 

         log_trace("   depth is updated to %g", depth);

         if (approachingGoal) {
            log_trace(" this is final step, exit loop");
            // leave the loop immediately (and return depth)
            break;
         }

         if(willLeaveAtmosphere){ //we have now done so; there's nothing more to integrate
            log_trace_stream(" Stopping at atmosphere boundary");

            // leave the loop immediately (and return depth)
            break;
         }

         std::string oldLayerName=curMedium.fBoundaryName_;
         //make sure that we actually get into the next layer
         while(curMedium.fBoundaryName_== oldLayerName){
            log_trace_stream("   Taking tiny step toward next layer");

            lastpos = pos;
            pos += tinyStepSize * dirCE;
            depth += FlatDepthCalculator(lastpos, pos, density, intg_type); 
            log_trace("   depth updated to %e, tinyStepSize is %e, boundary name %s",  depth, tinyStepSize, oldLayerName.c_str());
            x+=tinyStepSize;
            curMedium=GetEarthParam(pos);
            if(++iterations>maxIterations)
               log_fatal("Exceeded iteration count limit");
         };
         density=GetEarthDensityInCGS(curMedium,pos);
         distToNext=DistanceToNextBoundaryCrossing(pos,dirCE,willLeaveAtmosphere);
         assert(distToNext>0);
         
         log_trace_stream(" Now at " << pos << " having traversed " << x << " meters with " << depth << " gm/cm^2");
         log_trace_stream("  Current layer is " << curMedium.fBoundaryName_ << " with current density " << density);
         log_trace_stream("  Distance to next layer is " << distToNext << " meters");
         
         continue;
      }
      
      //otherwise do the integral approximately
      double h=distToNext;
      approachingGoal=false;
      if(x+h>len){
         log_trace_stream("   Reducing step size to avoid overshooting target distance");
         h=len-x;
         approachingGoal=true;
      }
      log_trace_stream("  Integrating density over next " << h << " meters");
      densityPathIntegrand integrand(curMedium,pos,dirCE, intg_type);
      double dDepth = M_TO_CM * Integration::rombergIntegrate(integrand,0,h,1e-6);
      log_trace_stream("   Integrated column depth is " << dDepth << " g/cm^2");
      
      x+=h;
      depth+=dDepth;
      pos+=distToNext*dirCE;
      
      if(willLeaveAtmosphere){ //we have now done so; there's nothing more to integrate
         log_trace_stream(" Stopping at atmosphere boundary");
         break;
      }
      if(approachingGoal) //done
         break;
      
      std::string oldLayerName=curMedium.fBoundaryName_;
      //make sure that we actually get into the next layer
      while(curMedium.fBoundaryName_==oldLayerName){
         log_trace_stream("   Taking tiny step toward next layer");

         lastpos = pos;
         pos += tinyStepSize * dirCE;
         // because this is so tiny step I assume it's flat density
         depth += FlatDepthCalculator(lastpos, pos, density, intg_type); 
         log_trace("   depth updated to %e, tinyStepSize is %e, boundary name %s",  depth, tinyStepSize, oldLayerName.c_str());
         x+=tinyStepSize;
         curMedium=GetEarthParam(pos);
         if(++iterations>maxIterations)
            log_fatal("Exceeded iteration count limit");
      };
      density=GetEarthDensityInCGS(curMedium,pos);
      distToNext=DistanceToNextBoundaryCrossing(pos,dirCE,willLeaveAtmosphere);
      assert(distToNext>0);
      
      log_trace_stream(" Now at " << pos << " having traversed " << x << " meters with " << depth << " gm/cm^2");
      log_trace_stream("  Current layer is " << curMedium.fBoundaryName_ << " with current density " << density);
      log_trace_stream("  Distance to next layer is " << distToNext << " meters");

   } // while loop end
   
   log_trace_stream(" Len error: " << len-x);
   return(depth);
}

//________________________________________________________________
bool EarthModelService::CheckIntersectionWithLayer(
                       const I3Position& pos,
                       const I3Direction& dir,
                       EarthParamMap::const_iterator ep,
                       EarthParamMap::const_iterator& closestBoundary,
                       double& closestDist,
                       bool& willExit) const
{
   //
   // the second.fZOffset_ is zero for:
   //  - all locks and air
   //  - all ice except for SimpleIceCap model 
   //
   I3Position shiftedPos = pos-I3Position(0,0,ep->second.fZOffset_);
   double r = shiftedPos.Magnitude();
   bool inside = (r<=ep->second.fUpperRadius_);
   log_trace_stream("    Testing intersection with " << ep->second.fBoundaryName_ << " starting from relative radius " << r << "; point is " << (inside?"":"not ") << "inside");
   
   I3Position entryPoint, exitPoint;
   int isect;
   
   double dist;

   if (inside) {
      // your earthparam object ep contains outer layer.
      // try to find an exit point to the outer layer
      // this must exist

      isect=EarthModelCalculator::GetIntersectionsWithSphere(shiftedPos, dir, ep->second.fUpperRadius_, entryPoint, exitPoint);
      if(isect) //only the exit point is relevant if we're inside the layer
         dist=(shiftedPos-exitPoint).Magnitude();

   } else {
      // your earthparam object ep contains inner layer.
      // try to find an entering point to the inner layer, if exist

      isect=EarthModelCalculator::GetIntersectionsWithSphere(shiftedPos, dir, ep->second.fUpperRadius_, entryPoint, exitPoint);

      if(isect){
         //we are interested in the entry point, but only if it is closer than the exit point
         dist=(shiftedPos-entryPoint).Magnitude();
         double dist2=(shiftedPos-exitPoint).Magnitude();
         log_trace_stream("     entryDist=" << dist << " exitDist=" << dist2);
         if(dist2<dist) //exit is closer than entry; we are headed away
            isect=0; //ignore this layer by treating it as if there were no intersection
      }
   }
   
   if(isect>=1){
      // update the given crosestDist
      log_trace_stream("     dist=" << dist);
      if(dist<closestDist){
         closestDist=dist;
         closestBoundary=ep;
         willExit=inside;
      }
   }
   
   return(!inside);
}

//________________________________________________________________
double EarthModelService::DistanceToNextBoundaryCrossing(
                       const I3Position& pos,
                       const I3Direction& dir,
                       bool& exitsAtmosphere) const
{

   EarthParamMap::const_iterator closestBoundary1, closestBoundary2;

   // the closestDist will be updated if the CheckIntersectionWithLayer
   // function finds a closer distance to layer.
   double closestDist=1e100;
   bool willExit=false, ignoreIce=false;

   //
   // check rock part first 
   //
   EarthParamMap::const_iterator it = fEarthParams_.lower_bound(pos.Magnitude());
   if(it!=fEarthParams_.end()){ //check the containing layer
      CheckIntersectionWithLayer(pos,dir,it,closestBoundary1,closestDist,willExit);
      
      //if we're not inside an ice, air, or vacuum layer, there can be no ice further down
      ignoreIce=!(it->second.fMediumType_==ICE || it->second.fMediumType_==AIR || it->second.fMediumType_==VACUUM);
      if(ignoreIce)
         log_trace_stream("    Inside medium type " << it->second.fMediumType_ << ", ignoring ice");
      
      if(it!=fEarthParams_.begin()){ //check the next layer inwards
         it--;
         CheckIntersectionWithLayer(pos,dir,it,closestBoundary1,closestDist,willExit);
      }
   }
   else if(fEarthParams_.size()>0){ //no containing layer, check outermost
      it=fEarthParams_.end();
      it--;
      CheckIntersectionWithLayer(pos,dir,it,closestBoundary1,closestDist,willExit);
   }
   double dist1=closestDist;
   
   //
   // check ice part next
   //
   if(!ignoreIce){
      it = fIceParams_.lower_bound((pos-I3Position(0,0,fIceCapSimpleZshift_)).Magnitude());
      if(it!=fIceParams_.end()){ //check the containing layer
         CheckIntersectionWithLayer(pos,dir,it,closestBoundary2,closestDist,willExit);
         if(it!=fIceParams_.begin()){ //check the next layer inwards
            it--;
            CheckIntersectionWithLayer(pos,dir,it,closestBoundary2,closestDist,willExit);
         }
      }
      else if(fIceParams_.size()>0){ //no containing layer, check outermost
         it=fIceParams_.end();
         it--;
         CheckIntersectionWithLayer(pos,dir,it,closestBoundary2,closestDist,willExit);
      }
   }
   
   //
   // test outermost layer of the Earth (should be air)
   //
   it=fEarthParams_.end();
   it--;
   exitsAtmosphere=(willExit && closestDist==dist1 && closestBoundary1==it);
   return(closestDist);
}

//________________________________________________________________
double EarthModelService::DistanceForColumnDepthToPoint(
                 const I3Position& to_posI3,
                 const I3Direction& dirI3,
                 double cDepth) const
{
   //the integration problem gives the same result when run in reverse
   return(DistanceForColumnDepthFromPoint(to_posI3,-dirI3,cDepth));
}

//________________________________________________________________
double EarthModelService::DistanceForColumnDepthFromPoint(
                 const I3Position& from_posI3,
                 const I3Direction& dirI3,
                 double cDepth) const
{
   log_trace_stream("DistanceForColumnDepthFromPoint from_posI3=" << from_posI3 << " dirI3=" << dirI3 << " cDepth=" << cDepth);
   const double precision=0.1; //the precision with which the target column depth must be matched
   
   if(cDepth<0)
      log_fatal_stream("Column depth must be positive (value was " << cDepth << ')');
   
   //convert to Earth centered coordinates
   I3Position endCE = GetEarthCoordPosFromDetCoordPos(from_posI3);
   I3Direction dirCE = GetEarthCoordDirFromDetCoordDir(dirI3);
   if(endCE.Magnitude()>fAtmoRadius_)
      log_fatal_stream("Starting point " << from_posI3 << " is outside the atmosphere");
   log_trace_stream("Starting point radius is " << endCE.Magnitude());
   
   I3Position pos=endCE; //current position (earth centered)
   double x=0; //distance traversed so far
   double depth=0; //column depth accumulated so far
   EarthParam curMedium=GetEarthParam(pos);
   double density=GetEarthDensityInCGS(curMedium,pos); //the current density == d(depth)/dx
   bool willLeaveAtmosphere=false;
   double distToNext=DistanceToNextBoundaryCrossing(pos,dirCE,willLeaveAtmosphere);
   log_trace_stream(" Now at " << pos << " having traversed " << x << " meters with " << depth << " gm/cm^2");
   log_trace_stream("  Current layer is " << curMedium.fBoundaryName_ << " with current density " << density);
   log_trace_stream("  Distance to next layer is " << distToNext << " meters");
   
   const double tinyStepSize=.01*I3Units::meter;
   double h=distToNext/10; //simple tuning suggests that this is a good starting step size
   
   double densNew, depthNew, depthErr, hNew;
   const double errTol=1e-3;
   bool approachingTarget=false;
   
   unsigned int iterations=0;
   unsigned int maxIterations=1e6; //if we try to take more steps than this something is horribly wrong

   while(true){
      if(++iterations>maxIterations)
         log_fatal("Exceeded iteration count limit");
      
      //if the density is constant, take a shortcut
      if(curMedium.fParams_.size()==1){
         log_trace_stream(" Skipping across constant density material");
         double totalDepthRemaining = distToNext * M_TO_CM * density;
         log_trace_stream("  Column depth remaining in this layer is " << totalDepthRemaining << " gm/cm^2");
         //finish by linear interpolation if there is enough depth in this layer
         if(depth+totalDepthRemaining > cDepth){
            log_trace_stream("  Linearly interpolating remaining distance");
            double dx = (cDepth-depth)/(density) * CM_TO_M;
            x += dx;
            depth += dx * M_TO_CM * density;
            break;
         }
         log_trace_stream("  Adding remaining column depth and advancing to next layer");
         //otherwise add on the full depth from this layer and jump to the next layer
         depth+=totalDepthRemaining;
         x+=distToNext;
         pos+=distToNext*dirCE;
         
         log_trace_stream("  Position is now " << pos << " with radius " << pos.Magnitude());
         
         if(willLeaveAtmosphere){ //we have now done so; there's nothing more to integrate
            log_debug("Integration would leave atmosphere, aborting before full column depth reached");
            break;
         }
         std::string oldLayerName=curMedium.fBoundaryName_;
         //make sure that we actually get into the next layer
         while(curMedium.fBoundaryName_==oldLayerName){
            log_trace_stream("   Taking tiny step toward next layer");
            depth += tinyStepSize * M_TO_CM * density;
            x+=tinyStepSize;
            pos+=tinyStepSize*dirCE;
            curMedium=GetEarthParam(pos);
            if(++iterations>maxIterations)
               log_fatal("Exceeded iteration count limit");
         };
         density=GetEarthDensityInCGS(curMedium,pos);
         distToNext=DistanceToNextBoundaryCrossing(pos,dirCE,willLeaveAtmosphere);
         assert(distToNext>0);
         h=distToNext/5;//defaultStep; //reset step size
         //h=defaultStep;
         
         log_trace_stream(" Now at " << pos << " having traversed " << x << " meters with " << depth << " gm/cm^2");
         log_trace_stream("  Current layer is " << curMedium.fBoundaryName_ << " with current density " << density);
         log_trace_stream("  Distance to next layer is " << distToNext << " meters");
         
         continue;
      }
      
      //take a step
      bool stepped=false, reducedStepSize=false, steppingToNext=false;
      log_trace_stream(" Attempting to step");
      while(!stepped){
         //if the proposed next step would go outside of the current layer, clip it
         if(h>distToNext){
            log_trace_stream(" Decreasing step size to avoid entering next layer");
            if((distToNext-tinyStepSize)<distToNext
               && GetEarthParam(pos+(distToNext-tinyStepSize)*dirCE).fBoundaryName_==curMedium.fBoundaryName_){ //try to get very close without crossing
               log_trace_stream("   Will attempt to reach layer boundary");
               h=distToNext-tinyStepSize;
               steppingToNext=true;
            }
            else if(distToNext>10)
               h=distToNext-10;
            else
               h*=.999;
         }
         
         log_trace_stream("  Trial step size: " << h << " m");
         //------
         //actually do a Runge-Kutta step
         {
            //const static double c2=.2;
            const static double c3=.3, c4=.8, c5=8./9,
            a71=35./384, /*a72=0,*/ a73=500./1113, a74=125./192, a75=-2187./6784, a76=11./84,
            e1=71./57600, /*e2=0,*/ e3=-71./16695, e4=71./1920, e5=-17253./339200, e6=22./525, e7=-1./40;
            
            double k3, k4, k5, k6;
            //k2=GetEarthDensityInCGS(curMedium, pos + c2*h*dirCE); //unused
            k3=GetEarthDensityInCGS(curMedium, pos + c3*h*dirCE);
            k4=GetEarthDensityInCGS(curMedium, pos + c4*h*dirCE);
            k5=GetEarthDensityInCGS(curMedium, pos + c5*h*dirCE);
            //double xph=x+h;
            k6=GetEarthDensityInCGS(curMedium, pos + h*dirCE);
            depthNew=depth + h * M_TO_CM *(a71*density + a73*k3 + a74*k4 + a75*k5 + a76*k6);
            densNew=k6;
            depthErr=h * M_TO_CM * (e1*density + e3*k3 + e4*k4 + e5*k5 + e6*k6 + e7*densNew);
         }
         //------
         log_trace_stream("  Step results: depth=" << depthNew << " err=" << depthErr);
         depthErr/=errTol;
         depthErr=std::abs(depthErr);
         
         const double SAFETY=0.9;
         const double growthPower=-0.2;
         //const double shrinkPower=-0.25;
         const double growLimit=1.89e-4; //=(5.0/SAFETY)**(1/growthPower)
         const double shrinkLimit=6561.0; //=(.1/SAFETY)**(1/shrinkPower)
         
         //check whether step was acceptable
         if(depthErr>1){
            //need to decrease step size
            log_trace_stream("  Decreasing step size to control error");
            if(depthErr < shrinkLimit)
               h=SAFETY*h/sqrt(sqrt(depthErr)); // 1/sqrt(sqrt(depthErr)) == depthErr**shrinkPower
			else
               h*=0.1;
            reducedStepSize=true;
            approachingTarget=false;
         }
         else if(depthNew>cDepth+precision){
            //need to decrease step size
            log_trace_stream("  Decreasing step size to avoid overshooting target depth");
            //attempt to linearly interpolate what our step size would be
            //if the interpolation underflows of fails to recommend a change, though,
            //just halve the step size to avoid an infinite loop
            double factor=(cDepth-depth)/(depthNew+precision-depth);
            if(factor==0 || factor>1.0)
               h/=2;
            else
               h*=factor;
            reducedStepSize=true;
            approachingTarget=true;
         }
         else{ //step looks good, recommend an increase for next step
            log_trace_stream("  Accepting step");
            if(approachingTarget){
               //attempt to linearly extrapolate what our step size would be
               //if the extrapolation underflows, though, just halve the step size
               double factor=(cDepth+precision-depthNew)/(depthNew-depth);
               log_trace_stream("   Extrapolating distance to endpoint, factor=" << factor);
               if(factor==0) //in case of underflow, just halve the step size
                  hNew=h/2;
               else{
                  hNew=h*factor;
                  //if approachingTarget is set we know that we need to stay in the current layer
                  if(hNew>distToNext-h)
                     hNew=SAFETY*(distToNext-h);
               }
            }
            else if(depthErr > growLimit)
               hNew=h*SAFETY*pow(depthErr,growthPower);
            else
               hNew=h*5;
            stepped=true;
         }
         if(++iterations>maxIterations)
            log_fatal("Exceeded iteration count limit");
      }
      
      x+=h;
      distToNext-=h;
      depth=depthNew;
      pos+=h*dirCE;
      h=hNew;
      
      if(willLeaveAtmosphere){ //we have now done so; there's nothing more to integrate
         log_warn("Integration would leave atmosphere, aborting before full column depth reached");
         break;
      }
      //check if we are transitioning to the next layer
      if(steppingToNext && !reducedStepSize){
         std::string oldLayerName=curMedium.fBoundaryName_;
         //make sure that we actually get into the next layer
         while(curMedium.fBoundaryName_==oldLayerName){
            log_trace_stream("   Taking tiny step toward next layer");
            depth += tinyStepSize * M_TO_CM * density;
            x+=tinyStepSize;
            pos+=tinyStepSize*dirCE;
            curMedium=GetEarthParam(pos);
            if(++iterations>maxIterations)
               log_fatal("Exceeded iteration count limit");
         };
         density=GetEarthDensityInCGS(curMedium, pos);
         distToNext=DistanceToNextBoundaryCrossing(pos,dirCE,willLeaveAtmosphere);
         assert(distToNext>0);
         h=distToNext/10;//defaultStep; //reset step size
         //h=defaultStep;
      }
      else
         density=densNew;
      
      log_trace_stream(" Now at " << pos << " having traversed " << x << " meters with " << depth << " gm/cm^2");
      log_trace_stream("  Current layer is " << curMedium.fBoundaryName_ << " with current density " << density);
      log_trace_stream("  Distance to next layer is " << distToNext << " meters");
      
      //if close enough to the target depth, stop
      if(std::abs(cDepth-depth)<precision)
         break;
   }
   log_trace_stream(" CDepth error: " << cDepth-depth);
   return(x);
}


//________________________________________________________________
const double EarthModelService::GetLeptonRangeInMeterFrom(
                 const  I3Position &posI3,
                 const  I3Direction &dirI3,
                 double energy,
                 bool   isTau,
                 EarthModelCalculator::LeptonRangeOption option,  
                 double scale) const
{
   return GetLeptonRangeInMeter(energy, posI3, dirI3, isTau, false,
                                option, scale);

}

//________________________________________________________________
const double EarthModelService::GetLeptonRangeInMeterTo(
                 const  I3Position &posI3,
                 const  I3Direction &dirI3,
                 double energy,
                 bool   isTau,
                 EarthModelCalculator::LeptonRangeOption option,  
                 double scale) const
{
   return GetLeptonRangeInMeter(energy, posI3, dirI3, isTau, true,
                                option, scale);

}

//________________________________________________________________
const double EarthModelService::GetLeptonRangeInMeter(
                 double energy,
                 const  I3Position &posI3,
                 const  I3Direction &dirI3,
                 bool   isTau,
                 bool   isReverse,
                 EarthModelCalculator::LeptonRangeOption option,  
                 double scale) const
{

  // get range in m.w.e, 
  double range = EarthModelCalculator::GetLeptonRange(energy, isTau, 
                                                    option, scale);

  log_debug("GetLeptonRangeInMeter is called with energy %g,"
            "endpos(%g,%g,%g), dir(%g,%g,%g), range=%g", 
            energy, posI3.GetX(), posI3.GetY(), posI3.GetZ(), 
            dirI3.GetX(), dirI3.GetY(), dirI3.GetZ(), range);
  
  // this function is faster than old MeterMWEConverter.
  // however, input value must be column depth [g/cm2] instead of 
  // [m.w.e]
  double cdep = EarthModelCalculator::MWEtoColumnDepthCGS(range);

  double length = -1;

  if (isReverse) {
     length = DistanceForColumnDepthToPoint(posI3, dirI3, cdep);
  } else {
     length = DistanceForColumnDepthFromPoint(posI3, dirI3, cdep);
  } 
  log_debug("... and length is obtained %g,", length);

  // -------------------------------------------
  // track length is also limited by muon's lifetime
  // This limit is applied to avoid crazy range for muon
  // in Air medium and speed up the while loop.
  //
  // speed of light : c
  // muon lifetime @ lest frame : t0
  // gamma factor : E/Mmu 
  // beta  factor : sqrt(1 - 1/(gamma^2))
  // muon lifetime @ lab frame : t = gamma*t0
  // muon path length @ lab frame :
  //    mean_len_lab = vt = c*beta*t = c*beta*gamma*t0 = c*sqrt(gamma^2 - 1)*t0
  //
  // 99.99% of muons will decay within the max_len_lab distance
  // exp (- max_len_lab / mean_len_lab) = 0.0001;
  // max_len_lab = - log(1e-4) * mean_len_lab
  //             = 9.21034 * mean_len_lab
  //

  double lightspeed = I3Constants::c / (I3Units::m/I3Units::s);

  double gamma = (energy/I3Units::GeV) / (ParticleConstants::m_mu/I3Units::GeV);
  double lifetime = 2.197034e-6;  // muon mean life
  double mean_len_lab = lightspeed * sqrt(gamma*gamma - 1)*lifetime;
  double max_len_lab = 9.21034 * mean_len_lab;

  if (isTau) {
     double gamma_tau = (energy/I3Units::GeV) / (ParticleConstants::m_tau/I3Units::GeV);
     double lifetime_tau = 290.6e-15; // tau mean life
     double mean_len_lab_tau = lightspeed * sqrt(gamma_tau*gamma_tau - 1)*lifetime_tau;
     double max_len_lab_tau = 9.21034 * mean_len_lab_tau;
     max_len_lab += max_len_lab_tau;
  }
  //cerr << "max_len_lab = " << max_len_lab << " [m]" << endl;

  if (length > max_len_lab) length = max_len_lab;

  log_debug("... then final length is obtained %g,", length);

  return length;

}

//__________________________________________________________
const double 
EarthModelService::GetDistanceFromEarthEntranceToDetector(double zenrad) const
{
//
// returns track path length from Earth-entrance point of track
// to the detector center
//
   if (zenrad > I3Constants::pi) log_error("zenrad exceeds pi!");
   if (zenrad < 0) log_error("zenrad is negative!");
   if (fIceCapType_ == NOICE && fDetPos_.GetZ() > fRockIceBoundary_) {
      log_notice("Detector Center is outside of Ice/Rock. return 0.");
      return 0.;
   }

   // first, find radius and center.

   double radius = -1;
   switch (fIceCapType_) {
      case EarthModelService::NOICE :
         radius = fRockIceBoundary_;
         break;
      case EarthModelService::ICESHEET :
         radius = fIceAirBoundary_;
         break;
      case EarthModelService::SIMPLEICECAP :
         radius = fRockIceBoundary_;
         break;
      default :
         log_error("IceCapType %d is not supported", 
                 int(fIceCapType_));
   }

   // tollerance 0.5 degree
   if (zenrad < 0.5 *I3Units::deg) {
      return fDetDepth_;
   }
   if (zenrad > I3Constants::pi - 0.5 *I3Units::deg) {
      return fDetPos_.GetZ() + radius;
   }

   // distance from earth center to detector center
   double det_z = fDetPos_.GetZ();
   std::vector<double> results = GetDistanceFromSphereSurfaceToDetector(zenrad, 
                                           radius, 
                                           det_z);
   double length1 = -1;
   if (results.size()) {
      // take larger value.
      length1 = results[0];
   }

   if (fIceCapType_ == NOICE || fIceCapType_ == ICESHEET) {
      // return now
      if (length1 < 0) {
         log_error("no intersection to the Earth! check setups.");
      }
      return length1;
   } 

   // case SIMPLEICECAP 
   // try to calculate one more distance in 
   // ice sphere coordinate.

   radius = fIceCapSimpleRadius_;
   double det_z2 = det_z - fIceCapSimpleZshift_;
   std::vector<double> results2 = GetDistanceFromSphereSurfaceToDetector(zenrad, 
                                           radius, 
                                           det_z2);
   double length2 = -1;
   if (results2.size()) {
      // take larger value.
      length2 = results2[0];
   }

   if (length1 < 0 && length2 <0) {
      log_error("no intersection with any sphere!");
      return -1;

   } 

   // both spheres has valid answers. 
   // check entrans points.
   I3Direction dirDC(zenrad, 0.);
   I3Position pos1DC = -length1 * dirDC;
   I3Position pos2DC = -length2 * dirDC;
   
   I3Position pos1CE = GetEarthCoordPosFromDetCoordPos(pos1DC);
   double r1 = pos1CE.Magnitude();
   I3Position pos2CE = GetEarthCoordPosFromDetCoordPos(pos2DC);
   double r2 = pos2CE.Magnitude();

   double length = 0;
   std::string sphere("Earth");
   if (r1 > r2) {
      // then take length 1
      length = length1;
   } else {
      length = length2;
      sphere = "IceCap";
   }

   log_debug("length1(Earth) = %f, p1theta %f[deg], p1.Mag %f,"
             " length2(IceCap) = %f, p2theta %f[deg], p2.Mag %f " 
             " and selected length %f(%s) ", 
             length1, pos1CE.GetTheta()/I3Units::degree, r1,
             length2, pos2CE.GetTheta()/I3Units::degree, r2,
             length, sphere.c_str());
  
   return length;

}

//__________________________________________________________
const std::vector<double>  
EarthModelService::GetDistanceFromSphereSurfaceToDetector(
                             double zenrad,
                             double radius,
                             double det_z) const
{
   //
   // r = radius of the Earth
   // l = distance from Earth center to detector center
   // x = distance from Earth-entrance point to detector center
   // zenrad = zenith angle of the track (incoming angle)
   // coszen  = cos(zenrad)
   // costheta = cos(pi-zenrad) = -coszen
   // 
   // r^2 = l^2 + x^2 - 2*l*x*costheta
   // x^2 - 2*l*costheta*x + l^2 - r^2 = 0
   // a = 1 
   // b = -l*costheta = l*coszen
   // c = l^2 - r^2; 
   // 
   // CAUTION : this function may give a wrong answer
   // if radius < det_z. Usually upgoing tracks are OK, 
   // but be careful for downgoing tracks.
   //

   double coszen = cos(zenrad);
   double r = radius; 
   double l = det_z;
   double c = (l-r)*(l+r);
   double b = l*coszen;
   double d = b*b - c;
   double sqrtd = sqrt(b*b - c);

   double x1 = (-b + sqrt(b*b - c));
   double x2 = (-b - sqrt(b*b - c));
   log_debug("zen %f[deg], zenrad %f,  coszen %f, r %f, l %f, c %f, b %f, d %f, sqrtd %f,  x1 %f, x2 %f", 
              zenrad/I3Units::degree, zenrad,  coszen, r, l, c, b, d, sqrtd,  x1, x2);

   std::vector<double> x;
   if (sqrtd > 0) {
      x.resize(2);
      if (x1 > x2) {
         x[0] = x1;
         x[1] = x2;
      } else {
         x[0] = x2;
         x[1] = x1;
      } 

   } else if (sqrtd == 0) {
      x.resize(1);
      x[0] = x1;

   } 
   return x;
}

//__________________________________________________________
void EarthModelService::SetEarthModel(const vector<string> & s)
{

   // clear params
   fEarthParams_.clear();
   fIceParams_.clear();
   fMohoBoundary_ = -1;
   fRockIceBoundary_ = -1;
   fIceAirBoundary_ = -1;
   fAtmoRadius_ = -1;

   for (unsigned int i = 0; i<s.size(); ++i) {

      string earth_model = s[i];
      if (earth_model.find(".dat") == string::npos) earth_model += ".dat";

      // 
      // check earthmodel file
      //
      string fname1 = fPath_ + "densities/" + earth_model;
      ifstream in(fname1.c_str());

      // if the earthmodel file doesn't exist, stop simulation 
      if (in.fail())
         log_fatal("failed to open %s. Set correct EarthParamsPath.", 
                    fname1.c_str());

      //
      // read the file
      //

      std::string buf;
      string label, medtype;
      double radius, param;
      int  nparams;
      bool reachedIce=false, reachedAir=false;

      while(getline(in,buf)) {
         {
            size_t pos;
            //eliminate data after first #
            if((pos=buf.find('#'))!=std::string::npos)
               buf.erase(pos);
            //trim whitespace
            const char* whitespace=" \n\r\t\v";
            if((pos=buf.find_first_not_of(whitespace))!=0)
               //if there are no non-whitespace characters pos==std::string::npos, so the entire line is erased
               buf.erase(0,pos);
            if(!buf.empty() && (pos=buf.find_last_not_of(whitespace))!=buf.size()-1)
               buf.erase(pos+1);
            if(buf.empty())
               continue;
         }
         
         // density data
         stringstream ss(buf);
         ss >> radius >> label >> medtype >> nparams;

         EarthParam ep;
         ep.fUpperRadius_ = radius;
         ep.fZOffset_ = 0;
         ep.fBoundaryName_ = label;
         ep.fMediumType_ = ConvertMediumTypeString(medtype); 
         for (int i=0; i<nparams; ++i) {
            ss >> param;
            ep.fParams_.push_back(param);
         }

         // set static params
         if (label == "moho_boundary") {
            fMohoBoundary_ = radius;
         } else if (label == "rockice_boundary") {
            fRockIceBoundary_ = radius;
         } else if (label == "iceair_boundary") {
            fIceAirBoundary_ = radius;
         } else if (label == "atmo_radius") {
            fAtmoRadius_ = radius;
         }
         
         //stop the process if layering assumptions are violated
         switch(ep.fMediumType_){
            case ICE:
               if(reachedAir)
                  log_fatal("No ice layers can be specified after an air layer has been specified");
               reachedIce=true;
               break;
            case AIR:
               reachedAir=true;
               break;
            case VACUUM:
               log_fatal("Explicit vacuum layers are not supported\n"
                         "(Note that beyond the last specified layer vacuum will be assumed implicitly)");
               break;
            default:
               if(reachedIce)
                  log_fatal("Only ice and air layers may be specified after the first ice layer");
               if(reachedAir)
                  log_fatal("Only air layers may be specified after the first air layer");
         }

         // put params in map
         // earth_radius does not describe a physical boundary, so it does not go in the map
         // (the earth_radius entry has gone though. I keep the if sentense for old tables
         // that may contain earth_radius entry.)
         if(label != "earth_radius") {
            if(ep.fMediumType_!=ICE)
               fEarthParams_[radius] = ep;
            else{
               fIceParams_[radius] = ep;
            }
         }

      } // end of the while loop

      in.close();
   }

   if (fEarthParams_.size()<1) {
      log_error("Failed to fill crust params!");
   }

   if (fMohoBoundary_ < 0) {
      log_error("Failed to fill moho_boundary! check data file");
   }
   if (fRockIceBoundary_ < 0) {
      log_error("Failed to fill rockice_boundary! check data file");
   }
   if (fIceAirBoundary_ < 0) {
      log_error("Failed to fill iceair_boundary! check data file");
   }
   if (fAtmoRadius_ < 0) {
      log_error("Failed to fill atmo_radius! check data file");
   }

   //
   // print the EarthCrust parameters
   // 
   log_debug("*** EarthModel parameters ***");
   log_debug("radius[m] \t MatType \t params");
   for (EarthParamMap::iterator it = fEarthParams_.begin();
        it != fEarthParams_.end(); ++it) {
      EarthParam ep = it->second;
      if(ep.fMediumType_==AIR){ //before the atmosphere print the ice
         for (EarthParamMap::iterator it = fIceParams_.begin();
              it != fIceParams_.end(); ++it) {
            EarthParam ep = it->second;
            log_debug("%lf \t %s", it->first,
                      GetMediumTypeString(ep.fMediumType_).c_str());
         }
      }
      log_debug("%lf \t %s", it->first,
                 GetMediumTypeString(ep.fMediumType_).c_str());
   }
   // Update detector center position.
   SetDetectorDepth(fDetDepth_);
}

//__________________________________________________________
void EarthModelService::GetAZ(int pdg, int &np, int &nn)
{          
   np = 0;
   int z = 0;
   int prefix = 0;
   int suffix = 0;

   char buf[CHAR_BUF_SIZE];
   sprintf(buf, "%d", pdg); 
   int nread = sscanf(buf, "%3d%3d%3d%1d", &prefix, &np, &z, &suffix);
   if (nread != 4) {
      log_error("Failed to convert nuclear pdg to A and Z "
                "prefix %d, A %d, Z %d, suffix %d",
                prefix, np, z, suffix);
   }
   nn = z - np;
}

//__________________________________________________________
void EarthModelService::SetMaterialModel(const vector<string>& s)
{
   // clear static params
   fMatRatioMap_.clear();

   for (unsigned int i = 0; i<s.size(); ++i) {

      string matratio = s[i]; 
      if (matratio.find(".dat") == string::npos) matratio += ".dat";

      // 
      // check earthmodel file
      //
      string fname1 = fPath_ + "materials/" + matratio;
      ifstream in(fname1.c_str());

      if (in.fail())
         log_fatal("failed to open %s. Set correct EarthParamsPath.", fname1.c_str());

      //
      // read the file
      //

      const int bufsize = CHAR_BUF_SIZE;
      char buf[bufsize];
      string medtype;
      int  matpdg, nmats;
      double weight; 
      int nread = 0;

      while(!in.eof()) {

         in.getline(buf, bufsize);
         nread = in.gcount();

         if (nread == -1) {
            log_error("getline failed");

         } else if (nread == 1 || buf[0] == ' ' || buf[0] == '#') {
            // new line, start from white space, or comment line.
            continue;

         } else {
            // material density data
            stringstream ss(buf);
            
            ss >> medtype >> nmats;

            std::map<int, double> matratio;
            for (int i=0; i<nmats; ++i) {
               in.getline(buf, bufsize);
               nread = in.gcount();
               if (nread == -1) {
                  log_error("getline failed");
               } else if (nread == 1 || buf[0] == ' ' || buf[0] == '#') {
                  // new line, start from white space, or comment line.
                  --i;
                  continue;
               } else {
                  stringstream ss2(buf);
                  ss2 >> matpdg >> weight;
                  matratio[matpdg] = weight;
               }
            }
            MediumType med = ConvertMediumTypeString(medtype); 
            fMatRatioMap_[med] = matratio;

         }

      } // end of the while loop

      in.close();
   }

   //
   // calculate P, N, E ratio
   //
   MatRatioMap::iterator it;
   for (it = fMatRatioMap_.begin();
        it != fMatRatioMap_.end(); ++it) {
      MediumType medtype = it->first; 
      map<int, double> &mats = it->second;
      double tot_np = 0;
      double tot_nn = 0;
      int np, nn;
      map<int, double>::iterator it2;
      for (it2 = mats.begin(); it2 != mats.end(); ++it2) {
         int pdg = it2->first;
         GetAZ(pdg, np, nn);
         tot_np += np;
         tot_nn += nn;
      }
      int tot_z = tot_np + tot_nn;
      double nw_proton = tot_np / tot_z;
      double nw_neutron = tot_nn / tot_z;
      map<int, double> number_weights;
      number_weights[2212] = nw_proton;  // proton 
      number_weights[11]   = nw_proton;  // electron
      number_weights[2112] = nw_neutron; // neutron
      fPNERatioMap_[medtype] = number_weights; 
   }

   // debug print for Ratio parameters
   // 
   log_debug("*** Material Weight Ratio ***");
   log_debug("MediumType \t Material PDG \t weight");
   for (it = fMatRatioMap_.begin();
        it != fMatRatioMap_.end(); ++it) {
      string medtypestr = GetMediumTypeString(it->first); 
      map<int, double> &mats = it->second;
      map<int, double>::iterator it2;
      for (it2 = mats.begin(); it2 != mats.end(); ++it2) {
         log_debug("%s \t %d \t %f", medtypestr.c_str(),
                   it2->first, it2->second);                
      }
   }
   log_debug("*** P,N,E Number Ratio ***");
   log_debug("MediumType \t PDG \t weight");
   for (it = fPNERatioMap_.begin();
        it != fPNERatioMap_.end(); ++it) {
      string medtypestr = GetMediumTypeString(it->first); 
      map<int, double> &mats = it->second;
      map<int, double>::iterator it2;
      for (it2 = mats.begin(); it2 != mats.end(); ++it2) {
         log_debug("%s \t %d \t %f", medtypestr.c_str(),
                   it2->first, it2->second);                
      }
   }
}

//__________________________________________________________
void EarthModelService::SetIceCapTypeString(string s)
{          
   fIceCapTypeString_ = s;
   if (s == "NoIce") {
      fIceCapType_ = EarthModelService::NOICE;
   } else if (s == "IceSheet") {
      fIceCapType_ = EarthModelService::ICESHEET;
   } else if (s == "SimpleIceCap") {
      fIceCapType_ = EarthModelService::SIMPLEICECAP;
   } else {
      log_error("IceCapType %s is not supported. set IceSheet.", 
                 s.c_str());
      fIceCapTypeString_ = "IceSheet";
      fIceCapType_ = EarthModelService::ICESHEET;
   }
}

//__________________________________________________________
void EarthModelService::SetIceCapSimpleAngle(double cap_angle)
{          
//
// You must set correct IceAirBoundary and RockIceBoundary in advance!
//
   if (fIceCapType_ != EarthModelService::SIMPLEICECAP) {
      log_trace("SetIceCapSimpleAngle should not be called when the ice cap type is not SIMPLEICECAP");
      fIceCapSimpleRadius_ = -1;
      fIceCapSimpleZshift_ = 0;
      return;
   }

   if (cap_angle < 0 || cap_angle > I3Constants::pi)  {
      log_error("angle is out of range ! %f ", cap_angle);              
   }

   // polar ice angle
   fIceCapSimpleAngle_ = cap_angle;

   // calculate radius
   double costheta = cos(cap_angle);
   //double sintheta = sqrt((1-costheta)*(1+costheta));

   // get largest depth of ice
   double h = fIceAirBoundary_ - fRockIceBoundary_ ;
   // h : max depth of ice at south pole
   // d + h = radius of icecap
   // r = bed_rock radius
   // r - d : z-position of center of icecap sphere
   //
   // (d + h)^2 = (r - d)^2 + r^2
   //                     - 2*r*(r-d)*costheta
   // d = (r^2*(1-costheta) - 0.5*h^2) / (h + r*(1-costheta))
   //
   double r = fRockIceBoundary_;
   double d = (r*r*(1 - costheta) - 0.5*h*h) / (h + r*(1 - costheta));
   
   fIceCapSimpleRadius_ = d + h; // radius of sphere of icecap
   fIceCapSimpleZshift_ = r - d; // z-pos of center of sphere of icecap
   
   log_info_stream("Shifted center for ice layer spheres is " << fIceCapSimpleZshift_ << " m above the earth center");

   EarthParamMap newIceParams;
   //re-center each ice surface on the new, shifted center point
   //and modify its radius so the the ice depth at the south pole
   //is unchanged
   for(EarthParamMap::iterator iter=fIceParams_.begin(), end=fIceParams_.end();
       iter!=end; iter++){
      EarthParam& ice=iter->second;
      ice.fZOffset_=fIceCapSimpleZshift_;
      ice.fUpperRadius_-= ice.fZOffset_;
      log_info_stream(" Corrected " << ice.fBoundaryName_ << " to have center (0,0," << ice.fZOffset_
                      << ") and radius " << ice.fUpperRadius_);
      newIceParams[ice.fUpperRadius_]=ice;
   }
   fIceParams_.swap(newIceParams);
   
   log_info("*** CapRadius (valid only for SimpleCap icecap model) ***");   
   log_info("Icecap Angle %f[deg], Icecap Radius %f [m], BedRock radius %f [m], Zshift %f [m] are set",
             fIceCapSimpleAngle_/I3Units::deg,
             fIceCapSimpleRadius_/I3Units::m,
             fRockIceBoundary_/I3Units::m,
             fIceCapSimpleZshift_/I3Units::m);
}

//__________________________________________________________
void EarthModelService::SetDetectorDepth(double d)
{          
   fDetDepth_ = d;
   fDetPos_.SetZ(fIceAirBoundary_ - d);
}

//__________________________________________________________
void EarthModelService::SetDetectorXY(double x, double y)
{          
   fDetPos_.SetX(x);
   fDetPos_.SetY(y);
}

bool EarthModelService::ignored_bool;

typedef I3SingleServiceFactory<EarthModelService>
I3EarthModelServiceFactory;
I3_SERVICE_FACTORY(I3EarthModelServiceFactory);

