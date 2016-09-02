/**
 *
 *  Copyright  (C) 2005
 *  The IceCube collaboration
 *  $Id: $
 *  @file I3NuEInjector.cxx
 *  @version $Revision:  $
 *  @date    $Date:      $
 *  @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *  modified by K.Hoshina <hoshina@icecube.wisc.edu>
 *  K.Hoshina changed the class to a service
 *
 *  @brief I3NuEInjector class implementation file, 
 *  the injector class for neutrino-generator program 
 */
////////////////////////////////////////////////////
#include "neutrino-generator/legacy/I3NuGInjector.h"
#include "neutrino-generator/legacy/StaticParams.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/utils/Calculator.h"
#include "neutrino-generator/utils/Defaults.h"
#include "neutrino-generator/sources/SourceUtils.h"
#include "earthmodel-service/EarthModelCalculator.h"
#include "dataclasses/I3Constants.h"
#include "icetray/I3SingleServiceFactory.h"

using namespace std;
using namespace nugen;
using namespace earthmodel;

//__________________________________________________
I3NuGInjector::I3NuGInjector(const I3Context &c)
 :I3ServiceBase(c),
  nu_flavor_("NuMu"),
  gamma_(1),
  energy_min_log_(1),
  energy_max_log_(9),
  zenith_min_(0), 
  zenith_max_(180.*I3Units::deg),
  azimuth_min_(0),
  azimuth_max_(360.*I3Units::deg),
  simulate_pointsource_(false),
  pointsource_name_(""),
  point_zenith_(45*I3Units::deg),
  point_zenith_sigma_(0*I3Units::deg),
  point_azimuth_(45*I3Units::deg),
  point_azimuth_sigma_(0*I3Units::deg),

  // zenith weighted generation parameter
  zenith_weight_param_(1.0),
  zenith_sampling_string_(Defaults::angleSamplingModeString),
  zenith_sampling_(Defaults::angleSamplingMode),
  steeringName_("")
{
  AddParameter("RandomService", "Pointer to random service", random_);
  AddParameter("SteeringName", "Name of SteeringService ", steeringName_);
  AddParameter("NuFlavor", "injection nu", nu_flavor_);
  AddParameter("GammaIndex", "injection gamma index", gamma_);
  AddParameter("EnergyMinLog", "injection energy min log10", energy_min_log_);
  AddParameter("EnergyMaxLog", "injection energy max log10", energy_max_log_);
  AddParameter("ZenithMin", "injection zenith min", zenith_min_);
  AddParameter("ZenithMax", "injection zenith max", zenith_max_);
  AddParameter("AzimuthMin", "injection azimuth min", azimuth_min_);
  AddParameter("AzimuthMax", "injection azimuth max", azimuth_max_);

  //
  // point source setting:
  //
  // if you want to simulate point source, set it true
  //
  AddParameter("SimulatePointSource", "do you want to simulate point source?", simulate_pointsource_);

  // then select how you define direction.
  //
  // If you have a pointsource stored in your frame already, 
  // specify the name of the pointosource.

  AddParameter("PointSourceDirectionName","Name of I3Direction that stores point source dir",   pointsource_name_);

  // or, you may specify given zenith and azimuth with their 
  // gaussian width

  AddParameter("PointSourceZenith", "point source injection zenith", point_zenith_);
  AddParameter("PointSourceZenithSigma", "point source zenith sigma", point_zenith_sigma_);
  AddParameter("PointSourceAzimuth", "point source injection azimuth ", point_azimuth_);
  AddParameter("PointSourceAzimuthSigma", "point source azimuth sigma", point_azimuth_sigma_);

  // end of point source setting

  AddParameter("ZenithWeightParam", 
               "option for zenith-weighted generation. "
               "default 1.0 gives a flat uniform weight. "
               "set 0.1 <= alpha <= 1.9. With a larger alpha "
               "you get more vertically upgoing events.",
               zenith_weight_param_);

  AddParameter("AngleSamplingMode", 
               "do you want to sample zenith in cos(zen) or zenith? ",
               zenith_sampling_string_);

  log_trace("I3NuGInjector constructed");
}  

//__________________________________________________
I3NuGInjector::I3NuGInjector(I3RandomServicePtr random,
                             SteeringPtr steer,
                             const string &name)
 :I3ServiceBase(name),
  nu_flavor_("NuMu"),
  gamma_(1),
  energy_min_log_(1),
  energy_max_log_(9),
  zenith_min_(0), 
  zenith_max_(180.*I3Units::deg),
  azimuth_min_(0),
  azimuth_max_(360.*I3Units::deg),
  simulate_pointsource_(false),
  pointsource_name_(""),
  point_zenith_(45*I3Units::deg),
  point_zenith_sigma_(0*I3Units::deg),
  point_azimuth_(45*I3Units::deg),
  point_azimuth_sigma_(0*I3Units::deg),
  zenith_weight_param_(1.0),
  zenith_sampling_string_(Defaults::angleSamplingModeString),
  zenith_sampling_(Defaults::angleSamplingMode),
  random_(random), steeringName_("dummy"),
  steer_(steer)
{
}

//__________________________________________________
I3NuGInjector::~I3NuGInjector()
{
   log_debug("I3NuGInjector deconstructed");
}

//__________________________________________________
void I3NuGInjector::Configure()
{

  GetParameter("RandomService", random_);
  if (!random_) {
     random_ = context_.Get<I3RandomServicePtr>();
     if (!random_) {
       log_fatal("No random service installed.");
     }
  }
  GetParameter("SteeringName", steeringName_);
  if (steeringName_ != "") {
     if (context_.Has(steeringName_)) {
        steer_ = context_.Get<SteeringPtr>(steeringName_);
     } else {
        log_error("You have to add I3NuGSteeringService to tray "
                  "before I3NuGInteractionInfo");
     }
  } else {
     log_error("You have to set SteeringName!");
  }

  GetParameter("NuFlavor",  nu_flavor_);
  GetParameter("GammaIndex", gamma_);
  GetParameter("EnergyMinLog", energy_min_log_);
  GetParameter("EnergyMaxLog", energy_max_log_);
  GetParameter("ZenithMin", zenith_min_);
  GetParameter("ZenithMax", zenith_max_);
  GetParameter("AzimuthMin", azimuth_min_);
  GetParameter("AzimuthMax", azimuth_max_);
  GetParameter("SimulatePointSource", simulate_pointsource_);
  GetParameter("PointSourceDirectionName",pointsource_name_);
  GetParameter("PointSourceZenith", point_zenith_);
  GetParameter("PointSourceZenithSigma", point_zenith_sigma_);
  GetParameter("PointSourceAzimuth", point_azimuth_);
  GetParameter("PointSourceAzimuthSigma", point_azimuth_sigma_);
  GetParameter("ZenithWeightParam", zenith_weight_param_);
  GetParameter("AngleSamplingMode", zenith_sampling_string_);

  zenith_sampling_ = ToAngleSamplingMode(zenith_sampling_string_);

}

//__________________________________________________
bool I3NuGInjector::InjectInitialNeutrino(I3FramePtr frame,
                                          I3MapStringDoublePtr wmap)
{

  if (simulate_pointsource_ && pointsource_name_ != "") {
      I3DirectionConstPtr pointSourceDirection = 
	frame->Get<I3DirectionConstPtr>(pointsource_name_);
      if (!pointSourceDirection)
          log_fatal("frame does not contain a direction object named \"%s\"", 
		    pointsource_name_.c_str());
      
      SetPointSourceZenith(pointSourceDirection->GetZenith());
      SetPointSourceAzimuth(pointSourceDirection->GetAzimuth());
  }
  
  //instanciate neutrino
  ParticlePtr neutrino_ptr(new Particle(I3Particle::Null, I3Particle::unknown, steer_));

  // set the new primary neutrino to the 
  injected_neutrino_ptr_ = neutrino_ptr;

  bool success = true;

  //primary information is determined in this function
  //the other is of default value
  //

  //SetType();
  success = DecideFlavor(neutrino_ptr);
  if (!success) return false;

  //SetEnergy();
  success = DecideEnergy(neutrino_ptr);
  if (!success) return false;

  //SetDir();
  success = DecideDirection(neutrino_ptr);
  if (!success) return false;

  // DecidePosition function filles I3Particle members of
  // neu_ptr->SetPos(EntrancePos);//entrance position to the earth
  // neu_ptr->SetLength((length_min+length_max)*I3Units::m);//exit from the earth
  // and Particle
  // neu_ptr->SetIntersectionToDetectionVolume(IntersectPosition);
  // neu_ptr->SetExitFromEarth(ExitPos);
  // neu_ptr->SetEntranceToEarth(EntrancePos);
  success = DecidePositions(neutrino_ptr);
  if (!success) return false;

  //neutrino_ptr->SetID(0);
  //I'm the parent or don't have parent thus set to zero
  //neutrino_ptr->SetParentID(0);
  //I'm the primary
  //neutrino_ptr->SetPrimaryID(0);
  neutrino_ptr->SetTime(0.0*I3Units::ns);
  neutrino_ptr->SetSpeed(I3Constants::c);
  neutrino_ptr->SetFitStatus(I3Particle::NotSet);

  // fill primary generation information to wmap
  SourceUtils::FillPrimaryInfo(neutrino_ptr, wmap);

  if (simulate_pointsource_) {
     SourceUtils::FillPointPrimaryGenInfo(gamma_,
                                  energy_min_log_,
                                  energy_max_log_,
                                  point_zenith_,
                                  point_zenith_sigma_,
                                  point_azimuth_,
                                  point_azimuth_sigma_,
                                  wmap);
  } else {
     SourceUtils::FillDiffusePrimaryGenInfo(gamma_,
                                  energy_min_log_,
                                  energy_max_log_,
                                  zenith_min_,
                                  zenith_max_,
                                  azimuth_min_,
                                  azimuth_max_,
                                  wmap);
  }

  return true;
}

/////////////////////////////////////////////////////  
bool I3NuGInjector:: DecideFlavor(ParticlePtr neu_ptr){
  log_trace("==decide flavor==");
 const string initial_neutrino_flavor = nu_flavor_;
 I3Particle::ParticleType neutrinotype(I3Particle::unknown);
 int flavor(0);

 random_->Uniform(-1,1)>0 ? flavor=1 : flavor=-1;

 if ("NuMu" == initial_neutrino_flavor){
   if(flavor==1)  neutrinotype = I3Particle::NuMu; 
   if(flavor==-1) neutrinotype = I3Particle::NuMuBar; 
 } else if("NuE" == initial_neutrino_flavor){
   if(flavor==1)  neutrinotype = I3Particle::NuE; 
   if(flavor==-1) neutrinotype = I3Particle::NuEBar; 
 } else if("NuTau" == initial_neutrino_flavor){
   if(flavor==1)  neutrinotype = I3Particle::NuTau; 
   if(flavor==-1) neutrinotype = I3Particle::NuTauBar; 
 } else log_fatal("This initial neutrino flavor %s doesn't exist", initial_neutrino_flavor.c_str());
 
 neu_ptr->SetType(neutrinotype);
 log_trace("flavor set to %d=%s", neu_ptr->GetType(), neu_ptr->GetTypeString().c_str());

  return true;
}

/////////////////////////////////////////////////////  
bool I3NuGInjector:: DecideEnergy(ParticlePtr neu_ptr){
    log_trace("==decide energy==");
  const double gamma    = gamma_;
  const double eminLog  = energy_min_log_;
  const double emaxLog  = energy_max_log_;

  double EnergyLog = 0.0;

  log_trace("injected with power law index %f, between log10(minE)=%f and log10(maxE)=%f", gamma, eminLog, emaxLog);

  if(eminLog==emaxLog){ 
    EnergyLog = eminLog;
  }else{
    
    if(gamma==1.0){
      EnergyLog = random_->Uniform( eminLog, emaxLog );
    }else if (gamma>0){
      double rand = random_->Uniform(0., 1.);
      double energyP = (1-rand)*pow(pow(10,eminLog),1-gamma)+rand*pow(pow(10,emaxLog),1-gamma);
      double energy  =  pow(energyP,1/(1-gamma));
      EnergyLog = log10( energy );
    }else{
      double energy = random_->Uniform(pow(10,eminLog), pow(10,emaxLog));
      EnergyLog = log10( energy );
      log_error("Negative power is not supported, we will get constant energy %f instead", energy);
    }
    
  }
  
  log_trace("injected with log E=%.10f", EnergyLog);
  
  neu_ptr->SetEnergy(pow(10, EnergyLog)*I3Units::GeV);

  return true;

}

//_________________________________________________________________
bool I3NuGInjector:: DecideDirection(ParticlePtr neu_ptr)
{
  //
  // now we have two options :
  // case 1: random direction
  // case 2: point source (defined position)
  //
  if (!simulate_pointsource_) {
      // case 1: random direction
      // be sure both zenith and azimuth parameters are given in I3Units

      log_trace("==decide direction==");
      double Zenith=0.0, Azimuth=0.0, cosZenith=0.0;
      double maxcoszen = cos(zenith_min_);
      double mincoszen = cos(zenith_max_);
      double weight = 1.0;

      // get zenith

      if (zenith_min_ == zenith_max_) {
        Zenith = zenith_min_;
      } else {
        if (zenith_sampling_ == COS) {
           Calculator::CalcSimpleSlopeWeight(
                    zenith_weight_param_,
                    mincoszen, maxcoszen, random_,  
                    cosZenith, weight);
           Zenith = acos(cosZenith); 

        } else if (zenith_sampling_ == ANG) {
           Calculator::CalcSimpleSlopeWeight(
                    zenith_weight_param_,
                    zenith_min_, zenith_max_, random_,  
                    Zenith, weight);

        } else {
           log_error("invalid angle sampling.");
        }

      }

      // get azimuth

      if (azimuth_min_ == azimuth_max_) {
        Azimuth = azimuth_min_;
      } else {
        Azimuth = random_->Uniform( azimuth_min_ , azimuth_max_);
      }

      neu_ptr->SetDir(Zenith, Azimuth);

      // multiply weight 
      // (this is not weight during propagation, but
      //  anyway it will be convoluted into final weight..)
   
      neu_ptr->GetInteractionInfoRef().SetDirectionWeight(weight);

      I3Direction direction(neu_ptr->GetDir());
      log_trace("injected to (zenith, azimuth)=(%f,%f), (theta,phi) in Direction =(%f,%f)[rad]",
                 Zenith, Azimuth, direction.CalcTheta(),direction.CalcPhi()); 

  } else if (simulate_pointsource_) {

      // case 2: point source (defined position)

      log_trace("producing a point source");
      // must be in I3Units
      double Zenith=0.0, Azimuth=0.0;
      const double zenith = point_zenith_;
      const double zenithSigma = point_zenith_sigma_;
      const double azimuth = point_azimuth_;
      const double azimuthSigma = point_azimuth_sigma_; 
      double x=random_->Gaus(0, zenithSigma);
      double y=random_->Gaus(0, azimuthSigma);
      I3DirectionPtr primary(new I3Direction(-x,-y,hypot(x,y)-1)); 
      primary->RotateY(zenith);
      primary->RotateZ(azimuth);
      if(zenithSigma == 0){
          Zenith = zenith;
      }else{
          Zenith = primary->GetZenith();
      }
      if(azimuthSigma == 0){
          Azimuth = azimuth;
      }else{
          Azimuth = primary->GetAzimuth();
      }

      neu_ptr->SetDir(Zenith, Azimuth);

      I3Direction direction(neu_ptr->GetDir());
      log_trace("injected to (zenith, azimuth)=(%f,%f), (theta,phi) in Direction =(%f,%f)[rad]", 
                 Zenith, Azimuth, direction.CalcTheta(),direction.CalcPhi()); 
  }

  return true;

}

//_________________________________________________________________
void I3NuGInjector:: GetStartPosDCCandidate(ParticlePtr neu_ptr,
                                            I3Position &intersectPosDC, 
                                            I3Direction &nuDirDC)
{

// get candidate of start position (entrance of earth)
// in detector coordinate
// June 6 2014 : cleaned up the code by using util function.
//
  nuDirDC = neu_ptr->GetDir();

  // Select an impact position. This function sets InjectionArea
  // parameter to neu_ptr also.
  intersectPosDC = nugen::SourceUtils::SelectRandomPos(neu_ptr,random_,steer_);
  log_trace("got intersect pos DC(%f,%f,%f)", intersectPosDC.GetX(),
                                              intersectPosDC.GetY(),
                                              intersectPosDC.GetZ());
}

//_________________________________________________________________
bool I3NuGInjector:: DecidePositions(ParticlePtr neu_ptr)
{
//
// You must call following functions beforehand,
// otherwise this function doesn't work correctly! 
// - DecideFlavor(neutrino_ptr);
// - DecideEnergy(neutrino_ptr);
// - DecideDirection(neutrino_ptr);
//

  log_trace("==decide positions==");

  // get first candidate of StartPos
  // calculate intersection position of a track
  // to detection cylinder (get in position)
  
  I3Position intersectPosDC;
  I3Direction nuDirDC;
  GetStartPosDCCandidate(neu_ptr, intersectPosDC, nuDirDC);

  // convert them to EarthCenter coordinate
  EarthModelServicePtr earth = steer_->GetEarthModelService();
  I3Direction nuDirCE(earth->GetEarthCoordDirFromDetCoordDir(nuDirDC)); 
  I3Position intersectPosCE(earth->GetEarthCoordPosFromDetCoordPos(intersectPosDC));  
  log_trace("Neutrino intersection to volume position CE (%f,%f,%f) [m]", 
	    intersectPosCE.GetX(), intersectPosCE.GetY(), intersectPosCE.GetZ());
  log_trace("Neutrino intersection radius to volume position CE (%f), WorldRadius %f [m]", 
            intersectPosCE.Magnitude(), steer_->GetWorldRadius());


  // check if the obtained track pass through the earth_radius.
  // CAUTION (Oct. 1. 2012) K.Hoshina:
  // This verdict has been done with geometrical earth radius (surface of ice),
  // however, now users can define their own earth radius.
  // If your injection volume is large and may cross over ice surface boundary, 
  // you have to define a larger earth radius such that your injection volume never
  // cross the boundary, in any angle.
  // To define your earth radius, use your own crust file with EarthModel service.
  //

  // get most closest position of a track to the 
  // Earth Center.
  //double t; 
  //I3Position impact_pos_CE;
  //double t;
  //double impact = EarthModelCalculator::GetImpactParameter(intersectPosCE, nuDirCE, t, impact_pos_CE);

  //
  // Get start and end position in earth center coordinate
  //
  I3Position entranceCE, exitCE;
  int success = EarthModelCalculator::GetIntersectionsWithSphere(intersectPosCE, nuDirCE,
                 steer_->GetWorldRadius(), entranceCE, exitCE);
  
  if (!success) {
     log_error("Your track doesn't path through the defined WorldRadius. "
               "It may happen when you set a large injection volume so "
               "that your detection volume cross the boundary of earth radius."
               "Your WorldRadius must contain whole detection volume in any angle. "
               "Set appropriate WorldRadius to Steering service. ");
     exit(EXIT_FAILURE);
     return false;
  }

  //Now we get entrance and exit position of neutrino 
  //to the Earth 
  
  // set the entrance position to the particle
  I3Position entranceDC(earth->GetDetCoordPosFromEarthCoordPos(entranceCE));
  I3Position entrancePosDC = entranceDC;
  neu_ptr->SetPos(entrancePosDC);

  // set the exit position to the particle
  I3Position exitDC(earth->GetDetCoordPosFromEarthCoordPos(exitCE));

  //initial neutrino's length is the distance between Earth entrance to the exit from earth
  double dist = (exitCE - entranceCE).Magnitude();
  neu_ptr->SetLength(dist);

  return true;

}

typedef I3SingleServiceFactory<I3NuGInjector> I3NuGInjectorFactory;
I3_SERVICE_FACTORY( I3NuGInjectorFactory );

