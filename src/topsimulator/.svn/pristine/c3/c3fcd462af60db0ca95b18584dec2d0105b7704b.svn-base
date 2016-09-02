/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3CorsikaGridInjector.cxx
 * @version $Rev: $
 * @date $Date: $
 * @authors Tilo Waldenmaier, Alessio Tamburro
 */

#include <topsimulator/interface/I3InjectorFactory.h>
#include <topsimulator/injectors/I3CorsikaGridInjector.h>
#include <phys-services/I3GSLRandomService.h>


I3CorsikaGridInjector::I3CorsikaGridInjector(const I3Context& context):
  I3InjectorService(context),
  currentRunID_(0),
  currentEvtID_(0)
{
  randomService_ = I3RandomServicePtr(); 
  
  corsikaFiles_.clear(); 
  AddParameter("FileNameList",
	       "List of CORSIKA file names.",
	       corsikaFiles_);
  
  AddParameter("Tanks",
	       "List of tank keys in the form [\"1A, 1B, 2A, 2B, ...\"] "
	       "(need to exist in the GCD file). "
	       "Sampling grid squares will be centered on these tanks. "
	       "Either tanks or stations can be specified at the same time!",
	       std::vector<std::string>());
  
  stationList_.clear();
  AddParameter("Stations",
	       "List of station numbers "
	       "(need to exist in the GCD file). "
	       "Sampling grid squares will be centered on these stations. "
	       "Either tanks or stations can be specified at the same time!",
	       stationList_);
  
  gridSize_ = 10.0;
  AddParameter("GridSize",
	       "Size of the sampling grid in [m].",
	       gridSize_);
  
  randomServiceName_ = "";
  AddParameter("RandomServiceName",
	       "Name of the random service in the context. If empty default random service will be used.",
	       randomServiceName_);
}


I3CorsikaGridInjector::~I3CorsikaGridInjector()
{

}


void I3CorsikaGridInjector::Configure()
{
  log_info("Configuring the I3CorsikaGridInjector:");
  
  GetParameter("FileNameList", corsikaFiles_);
  
  GetParameter("GridSize", gridSize_);
  gridSize_*=I3Units::m;
  log_info("+ Grid size       : %.2f m", gridSize_);
  
  // Get tank list
  std::vector<std::string> tmpTankList;
  GetParameter("Tanks", tmpTankList);
  FillTankList(tmpTankList);
  
  // Get station list
  GetParameter("Stations", stationList_);
  
  if(!tankList_.empty() && !stationList_.empty())
  {
    log_fatal("You can either specify a tank list OR a station list.");
    return;
  }
  
  log_info("+ Grid centered on: %s", tankList_.empty()?"STATIONS":"TANKS");

  if(!tankList_.empty() && gridSize_<2.0*I3Units::m)
  {
    log_warn("Grid size smaller than tank size. Results may not be accurate!");
  }
  
  if(!stationList_.empty() && gridSize_<12.0*I3Units::m)
  {
    log_warn("Grid size smaller than station size. Results may not be accurate!");
  }
  
  GetParameter("RandomServiceName", randomServiceName_);
  if(randomServiceName_.empty())
  {
    randomService_ = I3RandomServicePtr(new I3GSLRandomService(0));
    log_info("+ Random service  : I3GSLRandomService  (default)");
  }
  else
  {
    randomService_ = GetContext().Get<I3RandomServicePtr>(randomServiceName_);
    if(randomService_) log_info("+ Random service  : %s  (EXTERNAL)",  randomServiceName_.c_str());
    else               log_fatal("No random service \"%s\" in context!", randomServiceName_.c_str());
  }
  
  reader_.SetFileList(corsikaFiles_); 
  
  // Initialize iterators
  centerList_.clear();
  centerIter_ = centerList_.end();
  eventBuffer_.clear();
  eventIter_  = eventBuffer_.end();
  currentPrimary_ = I3Particle();
  currentEvtID_ = 0;
}


bool I3CorsikaGridInjector::NextEvent(int& runID, int& evtID, I3Particle& primary, I3FrameConstPtr frame)
{
  if(centerList_.empty())
  {
    // Fill list of station/tank coordinates
    // This needs to be done here because the response
    // service is not available in the Configure() method.
    log_trace("Filling center list ...");
    FillCenterList();
    if(centerList_.empty())
    {
      log_fatal("No tanks or station positions specified!"); 
      return false;
    }
    centerIter_ = centerList_.end();
    log_trace("Center list has %zu entries.", centerList_.size());
  }
  
  // Increment eventIter if not at end.
  // --> Move to next grid sample.
  if(eventIter_!=eventBuffer_.end()) eventIter_++;
  
  if(eventIter_==eventBuffer_.end())
  {
    // Increment centerIter if not at end.
    // --> Move to next tank/station
    if(centerIter_!=centerList_.end()) centerIter_++;
    
    // If centerIter at end or event buffer empty
    // read new CORSIKA shower, fill buffer and set centerIter to begin
    if(centerIter_==centerList_.end() || eventBuffer_.empty())
    {
      int tmpEvtID;
      if(!reader_.NextEvent(currentPrimary_, tmpEvtID))
      {
	if(!reader_.NextRun(currentRunID_))                    return false;
	if(!reader_.NextEvent(currentPrimary_, tmpEvtID)) return false;
      }
      
      // Relocate shower within one grid square to avoid
      // discrete grid locations relative to shower core.
      double shiftX = gridSize_*randomService_->Uniform(-0.5, 0.5);
      double shiftY = gridSize_*randomService_->Uniform(-0.5, 0.5);
      RelocateParticle(currentPrimary_, shiftX, shiftY);
      
      // Loop over shower particles and store
      // individual grid samples in event buffer.
      // Each sample corresponds to one event
      log_trace("Filling event buffer ...");
      double weight;
      ExtendedI3Particle particle;
      eventBuffer_.clear();
      std::vector<int> emptylist;
      while(reader_.NextParticle(particle, weight, emptylist))
      {
        particle.SetAirShowerComponent(Undefined);
	RelocateParticle(particle, shiftX, shiftY);
	
	GridKey gridKey(particle.GetX(), particle.GetY(), gridSize_);
	eventBuffer_[gridKey].push_back(particle);
      }
      log_trace("Event buffer has %zu entries", eventBuffer_.size());
      
      // Reset centerIter to first entry
      centerIter_ = centerList_.begin();
    }
    
    // Reset eventIter to first entry/grid sample
    eventIter_  = eventBuffer_.begin();
  }
  
  // Transform core position into tank/station reference system
  primary = currentPrimary_;
  RelocateParticle(primary,
		   centerIter_->GetX() - eventIter_->first.GetX(),
		   centerIter_->GetY() - eventIter_->first.GetY());
  
  evtID = ++currentEvtID_;
  runID = currentRunID_;
  
  // Reset particle iterator
  particleIter_=eventIter_->second.begin();
  
  return true;
}


bool I3CorsikaGridInjector::NextParticle(ExtendedI3Particle& particle)
{
  log_trace("Next particle ...");
  if(eventIter_==eventBuffer_.end())          return false; 
  if(particleIter_==eventIter_->second.end()) return false;
  
  particle = ExtendedI3Particle(*particleIter_);
  
  // Transform particle position into tank/station reference system
  RelocateParticle(particle,
		   centerIter_->GetX() - eventIter_->first.GetX(),
		   centerIter_->GetY() - eventIter_->first.GetY());
  
  particleIter_++;
  
  return true;
}


std::map<std::string, int> I3CorsikaGridInjector::GetAirShowerComponentNameMap() const
{
  static std::map<std::string, int> m;
  if (!m.size())
    m["Undefined"] = Undefined;
  return m;
}


void I3CorsikaGridInjector::FillCenterList()
{
  centerList_.clear();
  
  if(!tankList_.empty())
  {
    std::vector<TankKey>::const_iterator tank_iter;
    for(tank_iter=tankList_.begin(); tank_iter!=tankList_.end(); ++tank_iter)
    {
      I3TankResponsePtr response = responseService_->GetTankResponse(*tank_iter);
      if(!response)
      {
	log_error("Tank %s does not exist. Ignoring it!", tank_iter->str().c_str());
	continue;
      }
      
      centerList_.push_back(I3Position(response->GetX(),
				       response->GetY(),
				       response->GetZ()));
    }
  }
  else
  {
    std::vector<int>::const_iterator sta_iter;
    for(sta_iter=stationList_.begin(); sta_iter!=stationList_.end(); ++sta_iter)
    {
      TankKey tankKeyA(*sta_iter, TankKey::TankA);
      I3TankResponsePtr tankA = responseService_->GetTankResponse(tankKeyA);
      if(!tankA)
      {
	log_error("Tank %s does not exist. Ignoring this station!", tankKeyA.str().c_str());
	continue;
      }
      
      TankKey tankKeyB(*sta_iter, TankKey::TankB);
      I3TankResponsePtr tankB = responseService_->GetTankResponse(tankKeyB);
      if(!tankB)
      {
	log_error("Tank %s does not exist. Ignoring this station!", tankKeyB.str().c_str());
	continue;
      }
	
      centerList_.push_back(I3Position(0.5*(tankA->GetX() + tankB->GetX()),
				       0.5*(tankA->GetY() + tankB->GetY()),
				       0.5*(tankA->GetZ() + tankB->GetZ())));
    }
  }
}


void I3CorsikaGridInjector::FillTankList(const std::vector<std::string>& tankList)
{
  tankList_.clear();
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
	  tankList_.push_back(tankKey);
	  break;
	}
        case 'B':
        case 'b':
	{
	  TankKey tankKey(str, TankKey::TankB);
	  tankList_.push_back(tankKey);
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


void I3CorsikaGridInjector::RelocateParticle(I3Particle& particle, double shiftX, double shiftY)
{
  // This function shifts the particle on the x-y plane
  particle.SetPos(shiftX + particle.GetX(),
		  shiftY + particle.GetY(),
		  particle.GetZ());
}


I3FrameObjectConstPtr I3CorsikaGridInjector::GetEventInfo()
{
  I3CorsikaShowerInfoPtr showerInfo = reader_.GetShowerInfo();
  /*
  if (showerInfo)
  {
    showerInfo->crsSampleID = sampleIndex_;
    showerInfo->resampleRadius = relocationR_;
    showerInfo->nResample = numSamples_;
  }
  */
  return boost::dynamic_pointer_cast<I3FrameObject>(showerInfo);
}


I3_SERVICE_FACTORY(I3InjectorFactory<I3CorsikaGridInjector>);
