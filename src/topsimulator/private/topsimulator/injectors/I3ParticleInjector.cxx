/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3ParticleInjector.cxx
 * @version $Rev: $
 * @date $Date: $
 * @authors Tilo Waldenmaier
 */

#include <topsimulator/interface/I3InjectorFactory.h>
#include <topsimulator/injectors/I3ParticleInjector.h>
#include <phys-services/I3GSLRandomService.h>
#include <dataclasses/I3Constants.h>


I3ParticleInjector::I3ParticleInjector(const I3Context& context): I3InjectorService(context)
{
  randomService_ = I3RandomServicePtr(); 
  
  
  numParticles_ = 1;
  AddParameter("NumParticles",
	       "Number of particles to be generated per event and per tank",
	       numParticles_);

  numEvents_ = 1;
  AddParameter("NumEvents",
	       "Number of events to be generated",
	       numEvents_);
  
  AddParameter("TankKeys",
	       "List of tank keys in the form [\"1A, 1B, 2A, 2B, ...\"]. Tanks need to exist in the GCD file!",
	       std::vector<std::string>());

  useAllTanks_ = false;
  AddParameter("AllTanks",
	       "Set this to True if all tanks of the given geometry (GCD-file) should be used.",
	       useAllTanks_);
  
  std::string typeName = "mu-";
  AddParameter("ParticleType",
	       "Type of particle. Valid types are \"mu+, mu-, e+, e-, gamma, p+, p-, n\".",
	       typeName);
  
  energyRange_.clear();
  energyRange_.push_back(1.0);
  AddParameter("EnergyRange",
	       "Energy range in [GeV]",
	       energyRange_);
  
  specIndex_ = 0.0;
  AddParameter("SpectralIndex",
	       "Spectral index of energy spectrum (O: flat)",
	       specIndex_);
  
  
  radiusRange_.clear();
  radiusRange_.push_back(0.0);
  AddParameter("RadiusRange",
	       "Radius range in [m]",
	       radiusRange_);

  phiRange_.clear();
  phiRange_.push_back(0.0);
  AddParameter("PhiRange",
	       "Phi range [0, 360] deg",
	       phiRange_);

  startHeight_ = 5.0;
  AddParameter("StartHeight",
	       "Height above tank lid at which particle track starts (if zero --> starts on tank lid)",
	       startHeight_);
  
  zenithRange_.clear();
  zenithRange_.push_back(0.0);
  AddParameter("ZenithRange",
	       "Zenith range [0, 90] deg",
	       zenithRange_);
  
  zenithPower_ = 2.0;
  AddParameter("ZenithPower",
	       "Power of the cos(zenith) (0: Isotropic, 2: atmospheric muon zenith distribution).",
	       zenithPower_);
  
  azimuthRange_.clear();
  azimuthRange_.push_back(0.0);
  AddParameter("AzimuthRange",
	       "Azimuth range [0, 360] deg",
	       azimuthRange_);
  
  
  randomServiceName_ = "";
  AddParameter("RandomServiceName",
	       "Name of the random service in the context. If empty default random service will be used.",
	       randomServiceName_);
  
}


I3ParticleInjector::~I3ParticleInjector()
{

}


void I3ParticleInjector::Configure()
{
  log_info("Configuring the I3ParticleInjector:");

  GetParameter("NumParticles", numParticles_);

  GetParameter("NumEvents", numEvents_);
  
  std::vector<std::string> listOfTanks;
  GetParameter("TankKeys", listOfTanks);
  FillTankList(listOfTanks);
  
  // Set tank iterator back to the beginning
  tankKey_iter_ = tankKeys_.begin();
  
  GetParameter("AllTanks", useAllTanks_);
  if(useAllTanks_ && !tankKeys_.empty())
  {
    tankKeys_.clear();
    log_warn("All tanks are selected. The specified tank list has no effect.");
  }


  std::string typeName;
  GetParameter("ParticleType", typeName);
  particleType_ = GetParticleType(typeName);
  if(particleType_ == I3Particle::unknown)
  {
    log_fatal("Invalid particle type \"%s\"!", typeName.c_str());
  }
  log_info("+ Particle type: %s", typeName.c_str());
  
  
  // ----------------------
  
  GetRangeParameter("EnergyRange", energyRange_, I3Units::GeV);
  GetParameter("SpectralIndex", specIndex_);
  if(energyRange_.size()>1)
  {
    log_info("+ Energy range: [%.2f, %.2f] GeV", energyRange_.at(0)/I3Units::GeV, energyRange_.at(1)/I3Units::GeV);
    log_info("+ Spectral index: %.1f", specIndex_);
  }
  else
  {
    log_info("+ Fixed energy: %.2f GeV", energyRange_.at(0)/I3Units::GeV);
  }
  
  // ----------------------
  
  GetRangeParameter("RadiusRange", radiusRange_, I3Units::m);
  if(radiusRange_.size()>1)
  {
    log_info("+ Radius range: [%.2f, %.2f] m", radiusRange_.at(0)/I3Units::m, radiusRange_.at(1)/I3Units::m);
  }
  else
  {
    log_info("+ Fixed radius: %.2f m", radiusRange_.at(0)/I3Units::m);
  }
  
  GetRangeParameter("PhiRange", phiRange_, I3Units::deg);
  if(phiRange_.size()>1)
  {
    log_info("+ Phi range: [%.2f, %.2f] deg", phiRange_.at(0)/I3Units::deg, phiRange_.at(1)/I3Units::deg);
  }
  else
  {
    log_info("+ Fixed phi: %.2f deg", phiRange_.at(0)/I3Units::deg);
  }
  
  GetParameter("StartHeight", startHeight_);
  startHeight_  *= I3Units::m;
  log_info("+ Start height: %.2f m above tank", startHeight_/I3Units::m);
	   
  
  // ----------------------
  
  GetRangeParameter("ZenithRange", zenithRange_, I3Units::deg);
  GetParameter("ZenithPower", zenithPower_);
  if(zenithRange_.size()>1)
  {
    log_info("+ Zenith range: [%.2f, %.2f] deg", zenithRange_.at(0)/I3Units::deg, zenithRange_.at(1)/I3Units::deg);
    log_info("+ Zenith power: %.1f  %s", zenithPower_, (zenithPower_==0?"(isotropic)":""));
  }
  else
  {
    log_info("+ Fixed zenith: %.2f deg", zenithRange_.at(0)/I3Units::deg);
  }
  
  // ----------------------
  
  GetRangeParameter("AzimuthRange", azimuthRange_, I3Units::deg);
  if(azimuthRange_.size()>1)
  {
    log_info("+ Azimuth range: [%.2f, %.2f] deg", azimuthRange_.at(0)/I3Units::deg, azimuthRange_.at(1)/I3Units::deg);
  }
  else
  {
    log_info("+ Fixed azimuth: %.2f deg", azimuthRange_.at(0)/I3Units::deg);
  }
  
  // ----------------------
  
  GetParameter("RandomServiceName", randomServiceName_);
  if(randomServiceName_.empty())
  {
    randomService_ = I3RandomServicePtr(new I3GSLRandomService(0));
    log_info("+ Random service: I3GSLRandomService  (default)");
  }
  else
  {
    randomService_ = GetContext().Get<I3RandomServicePtr>(randomServiceName_);
    if(randomService_) log_info("+ Random service: %s  (EXTERNAL)",  randomServiceName_.c_str());
    else               log_fatal("No random service \"%s\" in context!", randomServiceName_.c_str());
  }

  // Initialize counters
  eventCounter_ = 0;
  particleCounter_ = numParticles_;
}


bool I3ParticleInjector::NextEvent(int& runID, int& evtID, I3Particle& primary, I3FrameConstPtr frame)
{
  if(eventCounter_ >= numEvents_) return false;
  eventCounter_++;
  
  // Fill tankKey_ list with all tanks (if this option is enabled)
  // This is only done the first time when tankKeys_ is still empty
  if(useAllTanks_ && tankKeys_.empty())
  {
    I3TankResponseMapPtr respMap = responseService_->GetTankResponseMap();
    
    I3TankResponseMap::const_iterator resp_iter;
    for(resp_iter=respMap->begin(); resp_iter!=respMap->end(); ++resp_iter)
    {
      tankKeys_.push_back(resp_iter->first);
    }
  }

  // Check if specified tanks really exist (only for the first event)
  if(eventCounter_==1)
  {
    std::vector<TankKey>::iterator tank_iter=tankKeys_.begin();
    while(tank_iter!=tankKeys_.end())
    {
      I3TankResponsePtr tank = responseService_->GetTankResponse(*tank_iter);
      if(tank)
      {
	++tank_iter;
      }
      else
      {
	log_error("Tank %s does not exist. Removing tank from list!", tank_iter->str().c_str());
	tank_iter = tankKeys_.erase(tank_iter);
      }
    };
  }
  
  // Set global tank iterator to first tank
  // at the beginning of an event
  tankKey_iter_ = tankKeys_.begin();
  particleCounter_ = 0;
  
  runID = 1;
  evtID = eventCounter_;
  
  return true;
}


bool I3ParticleInjector::NextParticle(ExtendedI3Particle& particle)
{
  // If number of particles per tank has been reached
  // return false and terminate event
  if(particleCounter_ >= numParticles_) return false;
  
  // This should never happen ...
  if(tankKey_iter_==tankKeys_.end())
  {
    log_error("Something is wrong with the TankKey iterator!");
    return false;
  }
  
  // This should also never happen since it got tested in the NextEvent method
  I3TankResponsePtr tank = responseService_->GetTankResponse(*tankKey_iter_);
  if(!tank)
  {
    log_fatal("Tank %s does not exist.", tankKey_iter_->str().c_str());
    return false;
  }
  
  // Generate particle position
  double xPos(NAN), yPos(NAN);
  GetXY(xPos, yPos);
  xPos += tank->GetX();
  yPos += tank->GetY();
  
  particle.SetType(particleType_);
  particle.SetAirShowerComponent(Undefined);
  particle.SetShape(I3Particle::TopShower);
  particle.SetPos(xPos, yPos, tank->GetZ()); // Generate particle on horizontal plane though tank center
  particle.SetDir(GetZenith(), GetAzimuth());
  particle.SetEnergy(GetEnergy());
  particle.SetSpeed(I3Constants::c); // Approximately
  particle.SetTime(0);
  
  // Shift start point of particle to start height
  ShiftAlongTrack(particle, tank->GetZ() + 0.5*tank->GetTankHeight() + startHeight_);
  
  tankKey_iter_++;
  if(tankKey_iter_==tankKeys_.end())
  {
    // Only increase particle counter if all tanks have been hit
    particleCounter_++;
    tankKey_iter_=tankKeys_.begin();
  }
  
  return true;
}


std::map<std::string, int> I3ParticleInjector::GetAirShowerComponentNameMap() const
{
  static std::map<std::string, int> m;
  if (!m.size())
    m["Undefined"] = Undefined;
  return m;
}


void I3ParticleInjector::GetRangeParameter(const std::string& name, std::vector<double>& range,  double unit)
{
  GetParameter(name, range);
  if(range.empty() || range.size()>2)
  {
    log_fatal("%s: Invalid number of enries (must be less than 3)!", name.c_str());
    return;
  }

  // Apply correct units
  std::vector<double>::iterator iter;
  for(iter=range.begin(); iter!=range.end(); ++iter)
  {
    *iter *= unit;
  }
}


void I3ParticleInjector::GetXY(double& x, double& y)
{
  double radius(NAN);
  if(radiusRange_.size()==1)
  {
    radius = radiusRange_.at(0);
  }
  else
  {
    double r1_sqr = radiusRange_.at(0)*radiusRange_.at(0);
    double r2_sqr = radiusRange_.at(1)*radiusRange_.at(1);
    
    radius = sqrt((r2_sqr - r1_sqr)*randomService_->Uniform(0,1) + r1_sqr);
  }
  
  double phi(NAN);
  if(phiRange_.size()==1)
  {
    phi = phiRange_.at(0);
  }
  else
  {
    phi = randomService_->Uniform(phiRange_.at(0), phiRange_.at(1));
  }
  
  x = radius*cos(phi);
  y = radius*sin(phi);
}


void I3ParticleInjector::ShiftAlongTrack(I3Particle& particle, double z)
{
  double shift = (particle.GetZ() - z)/particle.GetDir().GetZ();
  
  double x = particle.GetX() - shift*particle.GetDir().GetX();
  double y = particle.GetY() - shift*particle.GetDir().GetY();
  
  particle.SetPos(x, y, z);
}


double I3ParticleInjector::GetEnergy()
{
  if(energyRange_.size()==1) return energyRange_.at(0);
  
  double prob = randomService_->Uniform(0,1);
  
  if (specIndex_ == -1.) {   // needs special treatment
    return energyRange_.at(0) * pow(energyRange_.at(1)/energyRange_.at(0), prob);
  } else {
    double pow1 = pow(energyRange_.at(0), specIndex_+1.0);
    double pow2 = pow(energyRange_.at(1), specIndex_+1.0);
    
    return pow(prob*(pow2-pow1) + pow1, 1.0/(specIndex_+1.0));
  }
}


double I3ParticleInjector::GetZenith()
{
  if(zenithRange_.size()==1) return zenithRange_.at(0);
  
  double power = zenithPower_ + 1.0;
  double c1 = pow(cos(zenithRange_.at(0)/I3Units::rad), power);
  double c2 = pow(cos(zenithRange_.at(1)/I3Units::rad), power);
  
  return acos(pow(c1 + (c2 - c1)*randomService_->Uniform(0,1), 1.0/power))*I3Units::rad;
}


double I3ParticleInjector::GetAzimuth()
{
  if(azimuthRange_.size()==1) return azimuthRange_.at(0);
  return randomService_->Uniform(azimuthRange_.at(0), azimuthRange_.at(1));
}


I3Particle::ParticleType I3ParticleInjector::GetParticleType(const std::string& typeName)
{
  I3Particle::ParticleType type = I3Particle::unknown;
  if(typeName=="mu-")        type = I3Particle::MuMinus;
  else if(typeName=="mu+")   type = I3Particle::MuPlus;
  else if(typeName=="e-")    type = I3Particle::EMinus;
  else if(typeName=="e+")    type = I3Particle::EPlus;
  else if(typeName=="gamma") type = I3Particle::Gamma;
  else if(typeName=="p+")    type = I3Particle::PPlus;
  else if(typeName=="p-")    type = I3Particle::PMinus;
  else if(typeName=="n")     type = I3Particle::Neutron;
  else if(typeName=="pi0")   type = I3Particle::Pi0;
  else if(typeName=="pi+")   type = I3Particle::PiPlus;
  else if(typeName=="pi-")   type = I3Particle::PiMinus;
  else                       type = I3Particle::unknown;
  
  return type;
}


void I3ParticleInjector::FillTankList(const std::vector<std::string>& tankList)
{
  tankKeys_.clear();
  std::vector<std::string>::const_iterator iter;
  for(iter=tankList.begin(); iter!=tankList.end(); ++iter)
  {
    int str;
    char tankLabel[2];
    if(sscanf(iter->c_str(),"%d%1s", &str, tankLabel)==2)
    {
      switch(tankLabel[0])
      {
        case 'A':
        case 'a':
	{
	  TankKey tankKey(str, TankKey::TankA);
	  tankKeys_.push_back(tankKey);
	  break;
	}
        case 'B':
        case 'b':
	{
	  TankKey tankKey(str, TankKey::TankB);
	  tankKeys_.push_back(tankKey);
	  break;
	}
        default:
	{
	  log_error("\"%s\" is not a valid tank key. Ignoring it!", iter->c_str());
	  break;
	}
      }
    }
    else
    {
      log_error("\"%s\" is not a valid tank key. Ignoring it!", iter->c_str());
    }
  }
}


I3_SERVICE_FACTORY(I3InjectorFactory<I3ParticleInjector>);
