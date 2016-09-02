/** *  $Id: $ *  @file I3NuGPointSource.cxx *  @version $Revision:  $
 *  @date    $Date:      $
 *
 *  Copyright (C) 2005
 *  the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  Aya Ishihara  <aya.ishihara@icecube.wisc.edu>
 *  modified by Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 */

#include "neutrino-generator/Particle.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/sources/I3NuGPointSource.h"
#include "neutrino-generator/sources/SourceUtils.h"
#include "neutrino-generator/utils/Defaults.h"
#include "neutrino-generator/utils/Utils.h"

using namespace std;
using namespace nugen;
using namespace earthmodel;

I3_MODULE(I3NuGPointSource);

//______________________________________________________________
I3NuGPointSource::I3NuGPointSource(const I3Context& ctx) : 
  I3ConditionalModule(ctx), 
  number_of_events_(0),
  eventCounter_(0),
  steer_name_(Defaults::steerName), 
  outNuName_(Defaults::primaryNuName),
  gammaIndex_(2.0),
  energyMinLog_(2.0),
  energyMaxLog_(8.0),
  sourceName_(""),
  zenith_(45*I3Units::deg),
  azimuth_(45*I3Units::deg),
  zenithSigma_(0*I3Units::deg),
  azimuthSigma_(0*I3Units::deg),
  flavor_("NuMu"),
  nunubar_ratio_vec_(2,1)
{

  AddParameter("RandomService", "pointer to random service", random_);
  AddParameter("SteeringName", "name of steering service", steer_name_);

  AddParameter("NuFlavor", "injection flavor", flavor_);	
  AddParameter("NuNuBarRatio", "ratio of generated Nu : NuBar",nunubar_ratio_vec_);	

  AddParameter("OutNuName", "output neutrino name", outNuName_);
  AddParameter("SourceName", "name of point source", sourceName_);
  AddParameter("GammaIndex","power low index", gammaIndex_);	
  AddParameter("EnergyMinLog", "min energy log10", energyMinLog_);
  AddParameter("EnergyMaxLog", "max energy log10", energyMaxLog_);
  AddParameter("Zenith", "point source zenith", zenith_);
  AddParameter("ZenithSigma", "sigma of zenith", zenithSigma_);
  AddParameter("Azimuth", "point source azimuth", azimuth_);
  AddParameter("AzimuthSigma", "sigma of azimuth", azimuthSigma_);

  AddOutBox("OutBox");
}

//______________________________________________________________
void I3NuGPointSource::Configure()
{ 

  GetParameter("RandomService",    random_);

  GetParameter("SteeringName", steer_name_);
  steer_ptr_ = GetService<SteeringPtr>(steer_name_);
  if (!steer_ptr_) {
     log_fatal("You have to add I3NuGSteerService in advance!");
  }

  earth_ptr_ = steer_ptr_->GetEarthModelService();
  if (!earth_ptr_) {
     log_fatal("EarthModelService doesn't exist! "
               "Check name in nugen::Steer and make sure "
               "if you add EarthModelService in advance.");
  }

  GetParameter("NuFlavor", flavor_);	
  GetParameter("NuNuBarRatio", nunubar_ratio_vec_);	
  type_map_.clear();
  type_map_[Utils::GetParticleType(flavor_)] = nunubar_ratio_vec_[0];
  type_map_[Utils::GetParticleType(flavor_+"Bar")] = nunubar_ratio_vec_[1];

  GetParameter("OutNuName",    outNuName_);
  GetParameter("SourceName",   sourceName_);
  GetParameter("GammaIndex",   gammaIndex_);
  GetParameter("EnergyMinLog", energyMinLog_);
  GetParameter("EnergyMaxLog", energyMaxLog_);
  GetParameter("Zenith",       zenith_);
  GetParameter("ZenithSigma",  zenithSigma_);
  GetParameter("Azimuth",      azimuth_);
  GetParameter("AzimuthSigma", azimuthSigma_);  

  // initialize event counter etc.

  number_of_events_ = steer_ptr_->GetNGen();
  eventCounter_ = 0;
}

//___________________________________________________________________
void I3NuGPointSource::DAQ(I3FramePtr frame)
{
  if( eventCounter_ >= number_of_events_ ) {
     RequestSuspension();
     return;
  }

  if (! random_) {
     random_ = GetService<I3RandomServicePtr>();
  }

  // generate primary neutrino
  ParticlePtr nu = 
         SourceUtils::GeneratePrimaryNeutrino(type_map_, 
                                              random_, steer_ptr_);

  // set energy
  if (!SourceUtils::SetPowerLowEnergy(nu, gammaIndex_, 
                                      energyMinLog_,
                                      energyMaxLog_, random_)) {
     return;
  }

  // set direction 
  // if you have source direction in frame, overwrite zenith_ and
  // azimuth_ with frame value.

  if (sourceName_ != "") {
     I3DirectionConstPtr dir = 
	frame->Get<I3DirectionConstPtr>(sourceName_);
     if (!dir) {
          log_fatal("frame does not contain a direction object named"
                    " \"%s\"", sourceName_.c_str());
     }
     zenith_ = dir->GetZenith();
     azimuth_ = dir->GetAzimuth();
  }

  if (!SetDirection(nu)) {
     return;
  }

  // set position
  if (!SourceUtils::SetNuPositions(nu,earth_ptr_,random_,steer_ptr_)) {
     return;
  }

  // make MCWeightDict and put it to frame
  string weightname = steer_ptr_->GetWeightDictName();
  I3MapStringDoublePtr wmap = 
         Utils::PutWeightDict(frame, weightname);

  // fill primary generation information to wmap
  SourceUtils::FillPrimaryInfo(nu, wmap);
  SourceUtils::FillPointPrimaryGenInfo(gammaIndex_,
                                  energyMinLog_,
                                  energyMaxLog_,
                                  zenith_,
                                  zenithSigma_,
                                  azimuth_,
                                  azimuthSigma_,
                                  wmap);

  // put particle and its weights(par particle info) to frame
  Utils::PutParticle(frame, nu, outNuName_, wmap);

  string treename = steer_ptr_->GetMCTreeName();
  Utils::PutMCTree(frame, nu, treename);

  PushFrame(frame,"OutBox");  

  ++eventCounter_;
  return;

}

//_________________________________________________________________
bool I3NuGPointSource::SetDirection(ParticlePtr nu)
{
  log_trace("producing a point source");

  double Zenith=0.0, Azimuth=0.0;
  double x=random_->Gaus(0, zenithSigma_);
  double y=random_->Gaus(0, azimuthSigma_);
  I3DirectionPtr primary(new I3Direction(-x,-y,hypot(x,y)-1)); 
  primary->RotateY(zenith_);
  primary->RotateZ(azimuth_);

  if (zenithSigma_ == 0) {
     Zenith = zenith_;
  } else {
     Zenith = primary->GetZenith();
  }

  if (azimuthSigma_ == 0) {
     Azimuth = azimuth_;
  } else {
     Azimuth = primary->GetAzimuth();
  }

  nu->SetDir(Zenith, Azimuth);

  I3Direction direction(nu->GetDir());
  log_trace("injected to (zenith, azimuth)=(%f,%f), (theta,phi) =(%f,%f)[rad]", 
             Zenith, Azimuth, direction.CalcTheta(),direction.CalcPhi()); 

  // set direction weight.
  // for point source, it should be 1.
  nu->GetInteractionInfoRef().SetDirectionWeight(1.0);

  return true;

}

