/** 
 *    copyright  (C) 2004
 *   the icecube collaboration
 *   $Id: Particle.cxx 141406 2016-02-01 21:46:00Z david.schultz $
 *
 *   @version $Revision: 141406 $
 *   @date $Date: 2016-02-01 15:46:00 -0600 (Mon, 01 Feb 2016) $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief Particle implementation file
 *
 */
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/utils/Calculator.h"
#include "neutrino-generator/interactions/InteractionManagerBase.h"
#include "phys-services/surfaces/Cylinder.h"
#include <sstream>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

using namespace std;
using namespace earthmodel;

namespace nugen {

int Particle::global_particle_id_ = 0;

//_________________________________________________
Particle::Particle(I3Particle::ParticleShape shape, 
                   I3Particle::ParticleType type, 
                   SteeringPtr steer)
  : I3Particle(shape, type), steer_(steer),
    detectionVolume_(steer_->GetDetectionSurface())
{
  ClearFlags();
  my_particle_id_ = global_particle_id_++;

  // negative minor id means it has no parent.
  parent_id_.minorID = -1;
} 

//_________________________________________________
Particle::Particle(const I3Particle &p,
                   SteeringPtr steer)
  : I3Particle(p), steer_(steer),
    detectionVolume_(steer_->GetDetectionSurface())
{
  ClearFlags();
  my_particle_id_ = global_particle_id_++;

  // negative minor id means it has no parent.
  parent_id_.minorID = -1;
} 

//_________________________________________________
Particle::Particle()
  : I3Particle(I3Particle::Null, I3Particle::unknown), 
    detectionVolume_(new I3Surfaces::Cylinder(800, 1600)),
    isGeoCalculated_(false),
    isStepCalculated_(false)
{
  log_error("private default constructor is called");
  abort();
  my_particle_id_ = global_particle_id_++;

  // negative minor id means it has no parent.
  parent_id_.minorID = -1;
}

//_________________________________________________
Particle::Particle(const Particle &p)
  : I3Particle(p.GetI3Particle()), 
    steer_(p.steer_),
    intInfo_(p.intInfo_),
    intGeo_(p.intGeo_),
    composite_list_(p.composite_list_),
    stepmap_(p.stepmap_),
    detectionVolume_(p.detectionVolume_),
    isGeoCalculated_(p.isGeoCalculated_),
    isStepCalculated_(p.isStepCalculated_)
{
  // this function should not be used
  log_error("private copy constructor is called");
  abort();
  my_particle_id_ = global_particle_id_++;

  // negative minor id means it has no parent.
  parent_id_.minorID = -1;
}

//_________________________________________________
Particle& Particle::operator=(const Particle &p)
{
  // this function should not be used
  log_error("private operator= is called");
  abort();
  steer_ = p.steer_;
  intInfo_ = p.intInfo_;
  intGeo_ = p.intGeo_;
  composite_list_ = p.composite_list_;
  stepmap_ = p.stepmap_;
  detectionVolume_ = p.detectionVolume_;
  isGeoCalculated_ = p.isGeoCalculated_;
  isStepCalculated_ = p.isStepCalculated_;
  my_particle_id_ = p.my_particle_id_;
  parent_id_ = p.parent_id_;
  return *this;
}

//_________________________________________________
std::string Particle::PrintID()
{
   std::ostringstream os;

   os << "MajorID:" << GetMajorID() 
      << " MinorID:" << GetMinorID()
      << " UniqID:" << my_particle_id_;

   return os.str();
}

//_________________________________________________
void Particle::PrintDaughters(const std::string& indent)
{
   
   printf("%s+++%s, %lld, %d LocationType %d\n", 
          indent.c_str(), GetTypeString().c_str(), 
          (long long)GetMajorID(), GetMinorID(), GetLocationType());
   for (unsigned int i=0; i<composite_list_.size(); ++i) {
      ParticlePtr p = composite_list_[i];
      const vector<ParticlePtr>& dl = p->GetDaughters();
      if (dl.size() > 0) {
         p->PrintDaughters(indent+"   ");
      } else {
         printf("%s   +++%s, %lld, %d, LocationType %d\n", 
               indent.c_str(), p->GetTypeString().c_str(), 
               (long long)p->GetMajorID(), p->GetMinorID(),
               p->GetLocationType());
      }
   }
}

//_________________________________________________
void Particle::SetLocationType(I3Particle::LocationType l)
{
   this->I3Particle::SetLocationType(l);
   if (l != I3Particle::InIce) return;

   // if this particle is InIce, children must be InIce too
   for (unsigned int i=0; i<composite_list_.size(); ++i) {
      composite_list_[i]->Particle::SetLocationType(l);
   } 
}

//_________________________________________________
void Particle::AddDaughter(ParticlePtr p)
{
   if (GetLocationType() == I3Particle::InIce) {
      // inice particle's children must be inice
      p->SetLocationType(I3Particle::InIce);
   }
   p->SetParentID(GetID());
   composite_list_.push_back(p);
}

//_________________________________________________
I3Particle Particle::GetI3Particle() const
{
   I3Particle p(*dynamic_cast<I3Particle*>(const_cast<Particle*>(this)));
   //I3Particle p(*this);
   return p;
}

//_________________________________________________
void Particle::CopyExtraInfoFrom(const Particle& p)
{
   intInfo_ = p.GetInteractionInfoRef();
   intGeo_ = p.GetInteractionGeo();
}

//_________________________________________________
InteractionGeo& Particle::GetInteractionGeoRef()
{
   if (isGeoCalculated_) return intGeo_;
   
   // calculate interaction geometory
   InjectionMode mode = steer_->GetInjectionMode();

   if (mode == CIRCLE) {
      CalculateInteractionGeometryLegacy();
   } else {
      CalculateInteractionGeometry();
   }

   return intGeo_; 
}

//_________________________________________________
const DoubleStepMap& Particle::GetStepMapRef()
{
   if (isStepCalculated_) return stepmap_;
   
   // calculate stepping
   Stepping();
   return stepmap_; 
}

//_________________________________________________
void Particle::CheckParticle()
{

  log_trace("check particle");
  log_trace("Particle ID = %lld:%d", 
           (long long)GetMajorID(), GetMinorID());
  log_trace("Particle Flavor Type %d = %s", 
           GetType(), GetTypeString().c_str());
  log_trace("Particle log(Energy[GeV]) = %.5f, Energy = %.5f [GeV]", 
           log10(GetEnergy()/I3Units::GeV), GetEnergy()/I3Units::GeV);
  log_trace("Particle Start Position DC (%.2f,%.2f,%.2f)", 
           GetX(), GetY(), GetZ());
  log_trace("Particle Direction (zenith,azimuth)=(%.2f,%.2f)[deg]", 
           GetZenith()/I3Units::deg, GetAzimuth()/I3Units::deg);
  log_trace("Particle Start Time %.2f [ns], Speed %.2f [m/ns]", 
           GetTime()/I3Units::ns, GetSpeed()/I3Units::m*I3Units::ns);
  log_trace("Track Length %f [m]", GetLength()/I3Units::m);

  if (InteractedOrDecayed() ) {
    const I3Position EndPos = GetEndPosition();
    if (intInfo_.intType_==DECAY) {
      log_trace("Particle Decayed");
      log_trace("Particle Decay Position (%.2f,%.2f,%.2f)", 
		          EndPos.GetX(), EndPos.GetY(), EndPos.GetZ());
    } else {
      log_trace("Particle Interacted Type: %d", intInfo_.intType_);	  
      log_trace("Interaction Type in String:%s", 
		          GetInteractionTypeString(intInfo_.intType_).c_str());	  
      log_trace("Particle Interaction Position (%.2f,%.2f,%.2f)", 
		          EndPos.GetX(), EndPos.GetY(), EndPos.GetZ());
      log_trace("Interaction Crosssection      : log_10(sigma_type/cm2)=%f", 
		          log10(intInfo_.GetInteractionXsecCGS()));	  
      log_trace("Total Interaction Crosssection: log_10(sigma_total/cm2)=%f", 
		          log10(intInfo_.GetTotalXsecCGS()));	  
      log_trace("Interaction Probability : %g",
		          intInfo_.GetInteractionWeight() );	  
      log_trace("Column depth over track length: %.2f [g/cm2]",
		          intInfo_.GetInteractionColumnDepthCGS());	  
    }

  } else log_trace("Particle NOT interacted nor Decayed");

  /*
  log_trace("Entrance to the Earth in Detector Coordinate (%.2f, %.2f, %.2f)",
	    GetEntranceToEarth().GetX(),GetEntranceToEarth().GetY(),GetEntranceToEarth().GetZ());
  log_trace("Exit from the Earth in Detector Coordinate (%.2f, %.2f, %.2f)",
	    GetExitFromEarth().GetX(),GetExitFromEarth().GetY(),GetExitFromEarth().GetZ());
  log_trace("Intersection to detection volume in Detector Coordinate (%.2f, %.2f, %.2f)",
	    GetIntersectionToDetectionVolume().GetX(),
	    GetIntersectionToDetectionVolume().GetY(),
	    GetIntersectionToDetectionVolume().GetZ());
  */
}

//_________________________________________________________
const I3Position Particle::GetEndPosition()
{
  double     track_length = GetLength();
  if (std::isnan(track_length)) {
     I3Position p = GetPos();
     log_debug("Track Length not defined. pid = %d, ParticleType %s, log10(energy) = %f"
               " posDC(%f,%f,%f)",
               my_particle_id_, GetTypeString().c_str(), 
               log10(GetEnergy()), 
               p.GetX(), p.GetY(), p.GetZ());
     track_length = 0;
  }

  return GetPos() + track_length*GetDir();
}

//_________________________________________________________
bool Particle::InteractedOrDecayed()
{
  if (GetDaughters().size()>0) return true;
  return false;
}

//_________________________________________________________
const double Particle::GetTotalDaughterEnergies(){      

  vector<ParticlePtr>::const_iterator iter 
                            = GetDaughters().begin();
  
  double energy = 0;
  
  for(; iter != GetDaughters().end(); ++iter){
    energy += (*iter)->GetEnergy();
  }
  return energy;
}

//_____________________________________________________________
void Particle::CheckDaughters()
{
  log_trace("===== check daughters =====");
  int n_daughter=0;
  vector<boost::shared_ptr<Particle> >::const_iterator daughter_iter = GetDaughters().begin();
  for(; daughter_iter != GetDaughters().end(); ++daughter_iter) {
     ++n_daughter;
    
     log_trace("Daughter #%d: Energy %f GeV", n_daughter,
	            (*daughter_iter)->GetEnergy());
     (*daughter_iter)->CheckParticle();
    
  }
  log_trace("===== Exit CheckDaughters =====");
}

//______________________________________________________________
void Particle::CalculateInteractionGeometryLegacy()
{
  log_trace("CalculateInteractionGeo pid = %d", my_particle_id_);

  // if the particle is not neutrino, we shouldn't calculate
  // interactionGeometory anymore
  if (! I3Particle::IsNeutrino()) {
     isGeoCalculated_ = true;
     return;
  }

  // if interaction geom params are set already,
  // skip the function.
  if (isGeoCalculated_) return;

  ClearFlags();

  EarthModelServiceConstPtr earth_ptr = steer_->GetEarthModelService();

  //-----------------------------------------------------
  // get current geom params
  // pos must be within Earth's radius
  // (if you generate the particle with I3NuGInjector, 
  //  it should be within the Earth)
  //-----------------------------------------------------

  const I3Position posDC(GetPos());
  const I3Position posCE(earth_ptr->GetEarthCoordPosFromDetCoordPos(posDC));
  const I3Direction dirDC(GetDir());
  const I3Direction dirCE(earth_ptr->GetEarthCoordDirFromDetCoordDir(dirDC)); 
  const double ene  = GetEnergy();

  // sanity check 1
  double earthrad = steer_->GetWorldRadius();
  double tolerance = 1.0 * I3Units::m;
  if (posCE.Magnitude() - earthrad > tolerance) {
     log_warn("particle start from outside of Earth, diff = %g",
            posCE.Magnitude() - earthrad);
  }

  log_trace("CalculateInteractionGeom is called with energy %f, posDC(%f,%f,%f), dirDC(%f,%f,%f)",
             ene , posDC.GetX(), posDC.GetY(), posDC.GetZ(), dirDC.GetX(), dirDC.GetY(), dirDC.GetZ());

  double detection_length_before = steer_->GetActiveHeightBefore();
  double detection_length_after  = steer_->GetActiveHeightAfter();

  //-----------------------------------------------------
  // get closest position on a track to EarthCenter
  // may be used later.
  //-----------------------------------------------------
  double t_CE = -1;
  I3Position impact_pos_CE;
  EarthModelCalculator::GetImpactParameter(posCE, 
                                          dirCE, t_CE, impact_pos_CE);

  log_trace("length to ImpactPosCE %f, InpactPosCE(%f,%f,%f), impactparam %f", t_CE, 
            impact_pos_CE.GetX(), impact_pos_CE.GetY(), 
            impact_pos_CE.GetZ(), impact_pos_CE.Magnitude());

  //-----------------------------------------------------
  // get most closest position on a track 
  // to the IceCube center and distance to 
  // impact pos from posDE (toIP)
  //-----------------------------------------------------

  double toIP = -1;
  I3Position impact_pos_DC;
  double impact_DC = EarthModelCalculator::GetImpactParameter(
                        posDC, dirDC, toIP, impact_pos_DC);
  log_trace("length to ImpactPosDC %f, InpactPosDC(%f,%f,%f), impactparam %f", 
            toIP, 
            impact_pos_DC.GetX(), impact_pos_DC.GetY(), 
            impact_pos_DC.GetZ(), impact_DC);


  //-----------------------------------------------------
  // 1. calculate entering intersecton to detection volume.
  //-----------------------------------------------------

  // define entering position of detection volume
  // temporary.
  // this is used to calculate muon range, and 
  // will be updated later.

  I3Position enterPosDC = impact_pos_DC - detection_length_before*dirDC;

  double modDetLenBefore = detection_length_before;

  log_trace("original DetectionLengthBefore %f", modDetLenBefore);

  // muon or tau range calculation.
  // This is applicable only for numu(bar) and nutau(bar)
  // and if MuonDetectionVolumeExtension is ON.

  double lepton_range = 0;
  bool doMuRangeExt = steer_->DoMuonRangeExtension();

  if (doMuRangeExt && 
     (IsNuMu() || 
      IsNuTau() ||
      GetType() == I3Particle::NuEBar)) {

     double murange_opt = steer_->GetMuonRangeOpt();

     if (murange_opt == 0) {
        // This is not correct, but keep this option for 
        // backword compatibility,
        // to simulate NuGen 1 "Legacy Mode".
        // use lepton range [meter water equiv] as length
        lepton_range = GetMuonRange();
        intGeo_.range_mwe_ = lepton_range;

        // this is not used for range calculation
        // but we keep it for debugging purpose
        intGeo_.range_m_ = GetMuonRangeInMeterTo(enterPosDC);

     } else if (murange_opt > 0) {
        // lepton range is converted to length [m]
        // from somewhere to enterPosDC.
        // The obtained range should be sum of detection_length_before
        // and the length[m] instead of replacing 
        // the detection_length_before with the length[m],
        // since secondary muons may vanishe at the edge of detector.

        // this is not used for range calculation
        // but we keep it for debugging purpose
        intGeo_.range_mwe_ = GetMuonRange();

        intGeo_.range_m_ = GetMuonRangeInMeterTo(enterPosDC);
        log_trace("MuonRangeInMeter %f for energy %g", intGeo_.range_m_, GetEnergy());
        lepton_range = detection_length_before + 
                        intGeo_.range_m_;

     } else {
        log_error("negative MuRengeOpt is not supported") ; 
     }

     if (lepton_range > modDetLenBefore) {

        // update detection_length_before with larger value
        modDetLenBefore = lepton_range;
        log_trace("updated lepton range from parameterization is %f [m],"
                  " for lepton energy %f [GeV]", 
                        modDetLenBefore, 
                   ene/I3Units::GeV);
     } else {
        log_trace("detection_length_before %f was larger than "
                  "lepton_length %f. "
                  "use original detection_length_before.",
                   modDetLenBefore, lepton_range);
     }
  }

  log_trace("modDetLenBefore %f", modDetLenBefore);

  // update enterPosDC
  // now range_before is calculated with taking into account
  // particle position, we don't need sanity check
  enterPosDC = impact_pos_DC - dirDC*modDetLenBefore;
  I3Position enterPosCE(earth_ptr->GetEarthCoordPosFromDetCoordPos(enterPosDC));

  log_trace("enterPosDC(%f,%f,%f)", enterPosDC.GetX(), enterPosDC.GetY(), enterPosDC.GetZ());

  //-----------------------------------------------------
  // 2. calculate exit position from detection volume
  //-----------------------------------------------------
 
  I3Position endPosDC = impact_pos_DC + detection_length_after*dirDC;
  I3Position endPosCE(earth_ptr->GetEarthCoordPosFromDetCoordPos(endPosDC));
  double modDetLenAfter = (endPosDC - impact_pos_DC).Magnitude();

  log_trace("endPosDC(%f,%f,%f), modDetLenAfter = %f", 
           endPosDC.GetX(), endPosDC.GetY(), endPosDC.GetZ(),
           modDetLenAfter);

  //-----------------------------------------------------
  // 3. Check if the enter & end pos are inside the Earth(WorldRadius)
  //    Detection Volume must be within the WorldRadius
  //-----------------------------------------------------

  I3Position impact_pos_DC_CE(earth_ptr->GetEarthCoordPosFromDetCoordPos(impact_pos_DC));

  if (impact_pos_DC_CE.Magnitude() > earthrad || 
      enterPosCE.Magnitude() > earthrad ||
      endPosCE.Magnitude() > earthrad) {

     // get interactions with sphere. 
     I3Position start_CE, end_CE;
     int success = EarthModelCalculator::GetIntersectionsWithSphere(
                impact_pos_CE, dirCE,
                steer_->GetWorldRadius(),
                start_CE, end_CE);

     if (!success) {
        log_fatal("tracks must have intersections with the Earth(WorldRadius). "
                  "If you define a large injection area, "
                  "you have to adjust WorldRadius parameter too. ");
     } 

     if (impact_pos_DC_CE.Magnitude() > earthrad ) {

        // Feb.14 2014 K.Hoshina added:
        // I stopped to defrect neutrino angle for NC interaction 
        // which is exactly same behavior as old nugen,
        // because I learned it's so inefficient.
        // so the situation I described below won't happen. 
        // However, still I keep the code for future use (if needed.)
        //------------------------------------------------------------

        // It may happen if a neutrino's angle is deflected
        // significantly from its original direction.
        // Most closest position to the IceCube Center locates
        // outside of the Earth, so anyway this neutrino
        // will not be able to see from IceCube.
        // Set it as "dark" (I know this flag should be used for
        // photon propagator only, but anyway photon propagator
        // do nothing to neutrinos, so I use the flag for internal
        // use of NuGen.

        SetShape(I3Particle::Dark);

        // However, if the neutrino interact again, the tertiary 
        // neutrino may point back to IceCube again. 
        // We still have to set end point of "InEarth" propagation 
        // -- in this case enterPosDC, where we stop InEarth
        // propagation and start final interaction.
        // in this case the start_CE should be behind current
        // particle pos. Here is the order:
        // start_CE(Earth Surface) --> posCE(Inside Earth) --> 
        //    end_CE(Earth Surface) --> 
        //        impact_pos_DC_CE(Outside of the Earth)

        enterPosCE = end_CE;
        enterPosDC = earth_ptr->GetDetCoordPosFromEarthCoordPos(enterPosCE); 
        // endPosCE is dummy, because it's outside of the Earth
        endPosDC = enterPosDC + (detection_length_before
                                +detection_length_after) * dirDC;
        endPosCE = earth_ptr->GetEarthCoordPosFromDetCoordPos(endPosDC);
        // modDetLenBefore and modDetLenAfter are also dummy.
        double totalDetLen = (endPosDC - enterPosDC).Magnitude();
        modDetLenBefore = 0.5 * totalDetLen;
        modDetLenAfter = 0.5 * totalDetLen;

        log_debug("move enter position to the Earth surface enterPosCE(%f,%f,%f)",
            enterPosCE.GetX(), enterPosCE.GetY(), enterPosCE.GetZ());
        log_debug("move end position to dummy posCE(outside of the Earth)(%f,%f,%f)",
            endPosCE.GetX(), endPosCE.GetY(), endPosCE.GetZ() );
        log_debug("Dummy modDetLenBefore = %f, dummy modDetLenAfter = %f",
            modDetLenBefore, modDetLenAfter);

     } else if (enterPosCE.Magnitude() > earthrad) {

        // this case, we pulled back enterPosDC too much.
        // High energy downgoing neutrinos will have this issue.

        log_trace("move enter position to the Earth surface");
        enterPosDC = earth_ptr->GetDetCoordPosFromEarthCoordPos(start_CE); 
        enterPosCE = earth_ptr->GetEarthCoordPosFromDetCoordPos(enterPosDC);
        modDetLenBefore = (impact_pos_DC - enterPosDC).Magnitude();

     } else if (endPosCE.Magnitude() > earthrad) {

        // it may happen if you set huge injection cylinder

        log_trace("move end position to the Earth surface");
        endPosDC = earth_ptr->GetDetCoordPosFromEarthCoordPos(end_CE); 
        endPosCE = earth_ptr->GetEarthCoordPosFromDetCoordPos(endPosDC);
        modDetLenAfter = (impact_pos_DC - endPosDC).Magnitude();
     }
  }
  log_trace("modDetLenAfter %f", modDetLenAfter);

  double totlen = modDetLenBefore + modDetLenAfter;

  if (totlen > steer_->GetMaxDetVolLen()) { 
     log_error("too large detection volume %f for log10E %f and zenith %f "
               "deg (range %f m.w.e), modDetLenBefore = %f, modDetLenAfter = %f, "
               "impact_pos_DC(%f,%f,%f), impactparamCE %f, enterPosDC(%f,%f,%f), endPosDC(%f,%f,%f)", 
               totlen, log10(ene), GetDir().GetZenith()/I3Units::deg,
               GetMuonRange(), modDetLenBefore, modDetLenAfter,
               impact_pos_DC.GetX(), impact_pos_DC.GetY(), impact_pos_DC.GetZ(),
               impact_pos_DC.Magnitude(),
               enterPosDC.GetX(), enterPosDC.GetY(), enterPosDC.GetZ(),
               endPosDC.GetX(), endPosDC.GetY(), endPosDC.GetZ());
  }

  // save info

  intGeo_.impactParam_ = impact_DC; 
  intGeo_.mod_detlen_before_= modDetLenBefore;
  intGeo_.mod_detlen_after_= modDetLenAfter;
  intGeo_.closestDetVolDC_ = impact_pos_DC;
  intGeo_.enterDetVolDC_ = enterPosDC;
  intGeo_.exitDetVolDC_ = endPosDC;
  isGeoCalculated_ = true;

}

//______________________________________________________________
void Particle::CalculateInteractionGeometry()
{

  log_trace("CalculateInteractionGeo pid = %d", my_particle_id_);

  // if the particle is not neutrino, we shouldn't calculate
  // interactionGeometory anymore
  if (! I3Particle::IsNeutrino()) {
     isGeoCalculated_ = true;
     return;
  }

  // if interaction geom params are set already,
  // skip the function.
  if (isGeoCalculated_) return;

  // initialize flags
  ClearFlags();

  //-----------------------------------------------------
  // 1. calculate entering intersecton to detection cylinder.
  //-----------------------------------------------------

  // define entering position of detection volume
  // temporary.
  // this is used to calculate muon range, and 
  // will be updated later.

  I3Position pos = GetPos();
  I3Direction dir = GetDir();
  std::pair<double, double> t = detectionVolume_->GetIntersection(pos, dir);
  log_trace("t.first = %g, t.second = %g", t.first, t.second);

  if (std::isnan(t.first)) {
     log_trace("No intersection with detection cylinder");

     // if no intersection, we are not interested in the particle.
     // however, secondary particles of this particle might cross
     // the detCyl so let's set it as dark and calculate 
     // interaction geom with old function.

     SetShape(I3Particle::Dark);
     CalculateInteractionGeometryLegacy();
     return ;
  }

  //-----------------------------------------------------
  // get current geom params
  // pos must be within Earth's radius
  // (if you generate the particle with I3NuGInjector, 
  //  it should be within the Earth)
  //-----------------------------------------------------

  const I3Position posDC(pos);
  const I3Direction dirDC(dir);
  const double   ene  = GetEnergy();

  log_trace("CalculateInteractionGeom is called with energy %f, posDC(%f,%f,%f), dirDC(%f,%f,%f)",
             ene , posDC.GetX(), posDC.GetY(), posDC.GetZ(), dirDC.GetX(), dirDC.GetY(), dirDC.GetZ());

  // the enterPosDC will be updated with muon range
  // if the particle is mu or tau flavor.

  I3Position enterPosDC = posDC + t.first * dirDC;
  I3Position endPosDC = posDC + t.second * dirDC;

  // muon or tau range calculation.
  // This is applicable only for numu(bar) and nutau(bar)
  // and if MuonDetectionVolumeExtension is ON.

  double range_before = 0; 
  bool doMuRangeExt = steer_->DoMuonRangeExtension();

  if (doMuRangeExt && 
     (IsNuMu() || 
      IsNuTau() ||
      GetType() == I3Particle::NuEBar)) {

     // this function will not be used on Legacy mode,
     // so skip if parenthiesis for muonrange_opt.

     // lepton range is converted to length [m]
     // from somewhere to enterPosDC.
     intGeo_.range_m_ = GetMuonRangeInMeterTo(enterPosDC);
     log_trace("MuonRangeInMeter %f for energy %g", intGeo_.range_m_, GetEnergy());
     range_before = intGeo_.range_m_;

     // this is not used for range calculation
     // but we keep it for debugging purpose
     intGeo_.range_mwe_ = GetMuonRange();

  }

  // now range_before is calculated with taking into account
  // particle position, we don't need sanity check
  enterPosDC -= (dirDC*range_before);

  //-----------------------------------------------------
  // 2. Check if the enter & end pos are inside the Earth(WorldRadius)
  //    Detection Volume must be within the WorldRadius
  //-----------------------------------------------------

  EarthModelServiceConstPtr earth = steer_->GetEarthModelService();

  I3Position enterPosCE = earth->GetEarthCoordPosFromDetCoordPos(enterPosDC);
  I3Position endPosCE = earth->GetEarthCoordPosFromDetCoordPos(endPosDC);
  double earthrad = steer_->GetWorldRadius();

  if (enterPosCE.Magnitude() > earthrad || endPosCE.Magnitude() > earthrad) {

     I3Direction dirCE(earth->GetEarthCoordDirFromDetCoordDir(dirDC)); 
     I3Position  posCE(earth->GetEarthCoordPosFromDetCoordPos(posDC)); 
     double t_CE;
     I3Position impact_pos_CE;
     EarthModelCalculator::GetImpactParameter(
                             posCE, dirCE, t_CE, impact_pos_CE);
     
     // get interactions with sphere. 
     I3Position start_CE, end_CE;
     int success = EarthModelCalculator::GetIntersectionsWithSphere(
                impact_pos_CE, dirCE,
                earthrad,
                start_CE, end_CE);
     if (success) {
        if (enterPosCE.Magnitude() > earthrad) {
           enterPosDC = earth->GetDetCoordPosFromEarthCoordPos(start_CE); 
        }
        if (endPosCE.Magnitude() > earthrad) {
           endPosDC = earth->GetDetCoordPosFromEarthCoordPos(end_CE); 
        }
     } else {
        log_error("tracks must have intersections with the Earth(WorldRadius). "
                  "If you define a large injection area, "
                  "you have to adjust WorldRadius parameter too. ");
     }
  }
  log_trace("enterPosDC(%g,%g,%g)", enterPosDC.GetX(), enterPosDC.GetY(), enterPosDC.GetZ());

  log_trace("endPosDC(%g,%g,%g)", endPosDC.GetX(), endPosDC.GetY(), endPosDC.GetZ());

  double totlen = (endPosDC - enterPosDC).Magnitude();

  if (totlen > steer_->GetMaxDetVolLen()) { 
     log_warn("too large detection volume %f for energy %f and zenith %f deg, something is wrong!", 
               totlen, ene, GetDir().GetZenith()/I3Units::deg);
  }

  // save info

  I3Position impact_pos_DC;
  double toIP;
  double impact_DC = EarthModelCalculator::GetImpactParameter(posDC,
                                          dirDC, toIP, impact_pos_DC);
  log_trace("length to ImpactPosDC %f", toIP);
  
  intGeo_.impactParam_ = impact_DC; 
  intGeo_.mod_detlen_before_= (enterPosDC - impact_pos_DC).Magnitude();
  intGeo_.mod_detlen_after_= (endPosDC - impact_pos_DC).Magnitude();
  intGeo_.closestDetVolDC_ = impact_pos_DC;
  intGeo_.enterDetVolDC_ = enterPosDC;
  intGeo_.exitDetVolDC_ = endPosDC;
  isGeoCalculated_ = true;

}


//____________________________________________________________________________
void Particle::GetMaterial(const I3Position &posDC,
               EarthModelServiceConstPtr earth,
               EarthModelService::MediumType &med,
               double &density)
{
   I3Position posCE(earth->GetEarthCoordPosFromDetCoordPos(posDC));
   med = earth->GetMedium(posCE);
   //log_trace("got material type %s\n", earth_->GetMediumTypeString(med).c_str());

   density = earth->GetEarthDensityInCGS(posCE);
   //log_trace("density is %f\n", density);
   return;
}

//________________________________________________________________
void Particle::Stepping()
{
   // stepping function is called at Earth surface, most likely 
   // in bed rock and ice. Currently their structure is quite
   // simple sphere so that we use fast stepping function.
   // However, if we calculate final interaction within complicated structure, 
   // we need to get back to old bruteforce function...

   SteppingSimple();
   // SteppingBruteforce();

}

//________________________________________________________________
void Particle::SteppingSimple()
{
   EarthModelServiceConstPtr earth = steer_->GetEarthModelService();
   InteractionManagerBasePtr interp = steer_->GetInteractionManager(*this);
   InteractionTypeDoubleMap globfactors = steer_->GetGlobalXsecScales();
   InteractionGeo &intgeo = GetInteractionGeoRef();
   I3Position spos = intgeo.enterDetVolDC_;
   I3Position epos = intgeo.exitDetVolDC_;
   double energy = GetEnergy();
   double psurvApproxLimit = steer_->GetPsurvApproxLimit();

   log_trace("stepping is called for type %s from startp(%f,%f,%f) to endp(%f,%f,%f)",
             GetTypeString().c_str(), spos.GetX(), spos.GetY(), spos.GetZ(),
             epos.GetX(), epos.GetY(), epos.GetZ());

   I3Direction dir = GetDir();
   double totlen = (epos - spos).Magnitude();
   double len = 0;

   // prepare dummy laststep for initialization
   Step laststep;
   laststep.start_pos_ = spos;
   laststep.end_pos_ = spos;
   laststep.lowEApproxLimit_ = psurvApproxLimit;

   while (len < totlen) {

      // find next boundary 
      I3Position endposCE = earth->GetEarthCoordPosFromDetCoordPos(laststep.end_pos_);
      I3Direction dirCE = earth->GetEarthCoordDirFromDetCoordDir(dir);
      double toboundary = Calculator::DistanceToNextBoundary(earth,endposCE, dirCE);
      len += toboundary;

      if (len > totlen) {
         // this is the final step
         toboundary -= (len - totlen);
         len = totlen;
      }

      // get current step's end position and middle position
      I3Position stependpos = spos + len * dir;
      I3Position midpos = laststep.end_pos_ + 0.5*toboundary*dir;
      
      // get medium type and density at midpos 
      EarthModelService::MediumType mat;
      double density = 0; 
      GetMaterial(midpos, earth, mat, density);

      log_trace("stependpos (%f,%f,%f), len %f, mat %s",
                 stependpos.GetX(), stependpos.GetY(), stependpos.GetZ(),
                 len, earth->GetMediumTypeString(mat).c_str());

      // create a new step
      Step step;
      step.lowEApproxLimit_ = psurvApproxLimit;

      step.istep_ = laststep.istep_ + 1;
      step.total_xsec_= 0; // will be calculated
      step.total_xsec_per_gram_= 0; // will be calculated
      step.start_pos_= laststep.end_pos_;
      step.start_len_= laststep.end_len_;
      step.column_depth_in_ = laststep.column_depth_out_;
      step.psurv_in_ = laststep.psurv_out_;

      // calc len from the end of the last step
      double dlen = len - step.start_len_;
      double dcdepCGS = dlen/I3Units::cm*density; //dlen [m] to [cm]

      // get total cross section at middle position of
      // the step.
      I3Position midposCE(earth->GetEarthCoordPosFromDetCoordPos(midpos));
      DoubleDoublePair totXsec = 
                interp->GetTotalXsec(midposCE, GetType(), energy,
                                     globfactors);
      step.total_xsec_ += totXsec.first;
      step.total_xsec_per_gram_ += totXsec.second;

      // set end position, column depth
      step.end_len_ = len;
      step.end_pos_ = spos + len*dir;
      step.column_depth_out_ = step.column_depth_in_ + dcdepCGS;

      // calculate surviving probability within this step
      // TotalXsecPerGram [cm^2/g] * dcdepCGS [g/cm^2]
      // double prob = exp(- step.total_xsec_per_gram_ * dcdepCGS);
      double prob = step.CalcPsurv(step.total_xsec_per_gram_, dcdepCGS);

      log_trace("stepping : dcdepCGS = %g, totxsec_per_gram = %g, prob = %.15g", 
              dcdepCGS, step.total_xsec_per_gram_, prob);

      step.psurv_out_ = step.psurv_in_ * prob;

      // store the step to DoubleStepMap
      // key : total column depth at step end
      // from the entrance ofinteraction volume

      stepmap_[step.column_depth_out_] = step;

      // update laststep
      laststep = step;

      if (step.istep_ > 1000) {
         log_error("step %d, totlen %g, len %g, endpos(%f,%f,%f)"
                   "This is simple stepping mode, step number over 1000 " 
                   "seems something is totally wrong. Check it.",
                   step.istep_, totlen, len, 
                   step.end_pos_.GetX(), step.end_pos_.GetY(),
                   step.end_pos_.GetZ());
      }
   }

   log_debug("show all steps--- ");
   DoubleStepMap::iterator iter = stepmap_.begin();
   for (iter=stepmap_.begin(); iter != stepmap_.end(); ++iter) {
      (iter->second).Print();
   }
   log_debug("show all steps end--- ");

   // set total column dep to interaction info
   intInfo_.total_column_depthCGS_ = laststep.column_depth_out_;

   isStepCalculated_ = true;

}

//________________________________________________________________
void Particle::SteppingBruteforce()
{
   EarthModelServiceConstPtr earth = steer_->GetEarthModelService();
   InteractionManagerBasePtr interp = steer_->GetInteractionManager(*this);
   InteractionTypeDoubleMap globfactors = steer_->GetGlobalXsecScales();
   InteractionGeo &intgeo = GetInteractionGeoRef();
   I3Position spos = intgeo.enterDetVolDC_;
   I3Position epos = intgeo.exitDetVolDC_;
   double energy = GetEnergy();
   double psurvApproxLimit = steer_->GetPsurvApproxLimit();

   log_trace("stepping is called for type %s from startp(%f,%f,%f) to endp(%f,%f,%f)",
             GetTypeString().c_str(), spos.GetX(), spos.GetY(), spos.GetZ(),
             epos.GetX(), epos.GetY(), epos.GetZ());

   I3Direction dir = GetDir();
   double totlen = (epos - spos).Magnitude();

   // sample density and cross section at every 1m. It's bruteforce
   // but simple if geometory is not simple.
   // at the boundary of material, try to reduce the size of step.

   double defaultdx = steer_->GetStepSize(); // [m]
   double finedx = steer_->GetFineStepSize(); // [m]
   double dx = defaultdx;
   double len = 0;
   double density = 0; // [g/cm3]
   double dcdepCGS = 0;   // column depth within a step in [g/cm3]

   int    istep = 0;   // number of steps have been stepped

   // get medium type and density at this position
   EarthModelService::MediumType mat;
   GetMaterial(spos, earth, mat, density);

   EarthModelService::MediumType lastmat = mat;
   double  lastdensity = density;

   // dummy laststep for initialization
   Step laststep;
   laststep.start_pos_ = spos;
   laststep.end_pos_ = spos;
   laststep.lowEApproxLimit_ = psurvApproxLimit;

   // do stepping!

   while (len < totlen) {

      // step forward.
      len += dx;
      GetMaterial(spos + len*dir, earth, mat, density);
      I3Position curpos = spos + len*dir;

      log_trace("pos (%f,%f,%f), len %f, mat %s",
                 curpos.GetX(), curpos.GetY(), curpos.GetZ(),
                 len, earth->GetMediumTypeString(mat).c_str());

      // check material and adjust step size.
      if (mat != lastmat) {
         // make step finer. This routine is called until
         // a step cross over the material boundary.
         len -= dx;
         dx = finedx;
         len += dx;
         GetMaterial(spos + len*dir, earth, mat, density);
         // restore dx
         dx = defaultdx;
      }

      // then, if the material and density are same as the last
      // step, merge the step to the last one.
      bool merge_step = (istep != 0 &&
                        density == lastdensity &&
                        mat == lastmat);

      // new step
      Step step;
      step.lowEApproxLimit_ = psurvApproxLimit;

      if (merge_step) {
         log_trace("this step is merged");

         // first, remove exsisting step from stepmap.
         stepmap_.erase(laststep.column_depth_out_);

         // you can use laststep's calculation.
         step = laststep; // copy info

         // calc len from the start of the last step
         double dlen = len - step.start_len_;
         dcdepCGS = dlen/I3Units::cm*density; // [m] to [cm]

      } else {

         log_trace("Add new step");

         step.istep_ = laststep.istep_ + 1;
         step.total_xsec_= 0;
         step.total_xsec_per_gram_= 0;
         step.start_pos_= laststep.end_pos_;
         step.start_len_= laststep.end_len_;
         step.column_depth_in_ = laststep.column_depth_out_;
         step.psurv_in_ = laststep.psurv_out_;

         // calc len from the end of the last step
         double dlen = len - step.start_len_;
         dcdepCGS = dlen/I3Units::cm*density; //dlen [m] to [cm]

         // get total cross section at middle position of
         // the step.
         I3Position middle = step.start_pos_ + 0.5*dlen*dir;
         I3Position posCE(earth->GetEarthCoordPosFromDetCoordPos(middle));
         DoubleDoublePair totXsec = 
                   interp->GetTotalXsec(posCE, GetType(), energy,
                                        globfactors);

         step.total_xsec_ += totXsec.first;
         step.total_xsec_per_gram_ += totXsec.second;

      }

      // set end position, column depth
      step.end_len_ = len;
      step.end_pos_ = spos + len*dir;
      step.column_depth_out_ = step.column_depth_in_ + dcdepCGS;

      // calculate surviving probability within this step
      // TotalXsecPerGram [cm^2/g] * dcdepCGS [g/cm^2]
      // double prob = exp(- step.total_xsec_per_gram_ * dcdepCGS);
      double prob = step.CalcPsurv(step.total_xsec_per_gram_, dcdepCGS);

      log_trace("stepping : dcdepCGS = %g, totxsec_per_gram = %g, prob = %.15g", 
              dcdepCGS, step.total_xsec_per_gram_, prob);

      step.psurv_out_ = step.psurv_in_ * prob;

      // store the step to DoubleStepMap
      // key : total column depth at step end
      // from the entrance ofinteraction volume

      stepmap_[step.column_depth_out_] = step;

      // check remaining distance
      if (totlen - len < dx) {
         dx = totlen - len;
      }

      lastmat = mat;
      lastdensity = density;
      laststep = step;
      ++istep;

   }

   log_debug("show all steps--- ");
   DoubleStepMap::iterator iter = stepmap_.begin();
   for (iter=stepmap_.begin(); iter != stepmap_.end(); ++iter) {
      (iter->second).Print();
   }
   log_debug("show all steps end--- ");

   // set total column dep to interaction info
   intInfo_.total_column_depthCGS_ = laststep.column_depth_out_;

   isStepCalculated_ = true;

}

//________________________________________________________________
void Particle::MuRangeOptConverter(double murangeopt,
                        EarthModelCalculator::LeptonRangeOption &opt,
                        double &scale) const
{
  opt = EarthModelCalculator::DEFAULT;
  if (murangeopt <10) {
     opt = EarthModelCalculator::LEGACY;
  } else if (murangeopt >= 20) {
     opt = EarthModelCalculator::NUSIM;
  }

  scale = fmod(murangeopt, 10.);
}

//________________________________________________________________
const double Particle::GetMuonRange() const
{
   // Feb.14 2014
   // In discussion with C.Weaver we found there is a possibility
   // that we may need muon range extension for NuE, if Grashow Resonance
   // make W -> mu + numu  decay.

   if (IsElectron() || GetType() == I3Particle::NuE) return 0;

   double murangeopt = steer_->GetMuonRangeOpt();
   double ene = GetEnergy();

   EarthModelCalculator::LeptonRangeOption opt;
   double scale;
   MuRangeOptConverter(murangeopt, opt, scale);

   // If secondary could be tau, set tauextend true.
   // for NuEBer - W - leptons, we need to extend for tau.
   bool tauextend = true;
   if (IsMuFlavor()) tauextend = false;

   return EarthModelCalculator::GetLeptonRange(ene, tauextend,
                                               opt, scale);
}
 

//________________________________________________________________
const double Particle::GetMuonRangeInMeterTo(const I3Position &endposDC) const
{
  // NuEBar may generate track via Grashow Resonance. 
  if (IsElectron() || GetType() == I3Particle::NuE) return 0;

  EarthModelServiceConstPtr es_ptr = steer_->GetEarthModelService();

  double energy  = GetEnergy();
  I3Direction dirDC(GetDir());

  double murangeopt = steer_->GetMuonRangeOpt();
  EarthModelCalculator::LeptonRangeOption opt = EarthModelCalculator::DEFAULT;
  double scale = 1.0;
  MuRangeOptConverter(murangeopt, opt, scale);

  // If secondary could be tau, set tauextend true.
  // for NuEBer - W - leptons, we need to extend for tau.
  bool tauextend = true;
  if (IsMuFlavor()) tauextend = false;

  double distance = es_ptr->GetLeptonRangeInMeterTo(endposDC,
                              dirDC, energy, tauextend, 
                              opt, scale); 

  double maxdist = (endposDC - GetPos()).Magnitude();

  return (distance < maxdist) ? distance : maxdist;

}

//________________________________________________________________
const double Particle::GetMuonRangeInMeterFrom(const I3Position &posDC) const
{
  // following line is inactivated because NuEBar may generate track via Grashow Resonance. 
  //if (IsEFlavor()) return 0;

  EarthModelServiceConstPtr es_ptr = steer_->GetEarthModelService();

  double energy  = GetEnergy();
  I3Direction dirDC(GetDir());

  double murangeopt = steer_->GetMuonRangeOpt();
  EarthModelCalculator::LeptonRangeOption opt = EarthModelCalculator::DEFAULT;
  double scale = 1.0;
  MuRangeOptConverter(murangeopt, opt, scale);

  // If secondary could be tau, set tauextend true.
  // for NuEBer - W - leptons, we need to extend for tau.
  bool tauextend = true;
  if (IsMuFlavor()) tauextend = false;

  double distance = es_ptr->GetLeptonRangeInMeterFrom(posDC,
                              dirDC, energy, tauextend, 
                              opt, scale); 

  return distance;

}

}
