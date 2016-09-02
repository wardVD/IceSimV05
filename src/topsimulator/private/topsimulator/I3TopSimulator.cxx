/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3TopSimulator.cxx
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <topsimulator/I3TopSimulator.h>
#include <topsimulator/ExtendedI3Particle.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <icetray/I3Bool.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/I3Double.h>
#include <stdexcept>

I3_MODULE(I3TopSimulator);

// Name of the IncEventID boolean in the frame (see I3MCTimeGeneratorService)
const std::string I3TopSimulator::INC_ID_NAME = "MCTimeIncEventID";

I3TopSimulator::I3TopSimulator(const I3Context& context): I3Module(context)
{
  injector_ = I3InjectorServicePtr();
  response_ = I3IceTopResponseServicePtr();
  
  responseServiceName_ = I3DefaultName<I3IceTopResponseService>::value();
  AddParameter("ResponseServiceName",
         "Name of the IceTop response service.", 
         responseServiceName_);

  injectorServiceName_ = I3DefaultName<I3InjectorService>::value();
  AddParameter("InjectorServiceName",
         "Name of the injector service.", 
         injectorServiceName_);
  
  mcPrimaryName_ = "MCPrimary";
  AddParameter("PrimaryName",
         "Name of the primary particle in the frame.",
         mcPrimaryName_);
  
  hitSeriesName_ = "IceTopMCHits";
  //hitSeriesName_ = ""; // empty means it is not created
  AddParameter("IceTopHitSeriesName",
         "Name of the resulting IceTop hit series in the frame.",
         hitSeriesName_);
  
  peSeriesName_ = "IceTopMCPEs";
  AddParameter("IceTopPESeriesName",
         "Name of the resulting IceTop pe series in the frame.",
         peSeriesName_);

  cherSeriesName_ = ""; // empty means it is not created
  AddParameter("IceTopCherenkovHitSeriesName",
         "For debugging. Name of the resulting cherenkov hit series in the frame.",
         cherSeriesName_);
  
  icMCTreeName_ = "InIceMCTree";
  AddParameter("InIceMCTreeName",
         "Name of the I3MCTree to store InIce particles (muons above energy threshold) in the frame. If empty InIceMCTree is disabled.",
         icMCTreeName_);

  itMCTreeName_ = "";
  AddParameter("IceTopMCTreeName",
         "Name of the I3MCTree to store IceTop particles (particles which hit an IceTop tank) in the frame. If empty IceTopMCTree is disabled.",
         itMCTreeName_);
  
  testPulsesName_ = "";
  AddParameter("IceTopTestPulsesName",
         "Name of the IceTop RecoPulsesSeries (without any PMT/DOM effects). For debuging only!",
         testPulsesName_);
  
  hitBinWidth_ = 1.0;
  AddParameter("HitBinWidth",
         "Bin width for hit binning in [ns]. Must be greater than zero!",
         hitBinWidth_);
  
  muEnergyCut_ = 273.0;
  AddParameter("MuonEnergyCutOff",
         "Muon energy threshold in [GeV] above which muons will be stored in the \"InIceMCTree\" (if enabled).",
         muEnergyCut_);
  
  writeEvtHeader_ = false;
  AddParameter("WriteEventHeader",
         "Create an event header to store the run and event numbers.",
         writeEvtHeader_);

  std::vector<std::string> tankKeys;
  AddParameter("Tanks",
         "Only simulate the given tanks.",
         tankKeys);

  compressPEs_ = 2;
  AddParameter("CompressPEs",
         "Merge pes generated within hitBinWidth time window, and...\n"
         "0: keep full particle source information [largest number of I3MCPE objects].\n"
         "1: keep information on air shower class (e.g.: electron, gamma, muon, hadron) [medium number of I3MCPE objects].\n"
         "2: drop particle source information completely [smallest number of I3MCPE objects].",
         compressPEs_);

  useInjectorComponents_ = false;
  AddParameter("UseInjectorComponents", "Use the particle class information given by the injector, as opposed to determining it here. Only valid if CompressPEs==1.", useInjectorComponents_);

  AddOutBox("OutBox");
}


I3TopSimulator::~I3TopSimulator()
{
}

 
void I3TopSimulator::Configure()
{
  log_info("Configuring the I3TopSimulator:");

  GetParameter("InjectorServiceName", injectorServiceName_);
  log_info("+ Injector Service: %s", injectorServiceName_.c_str());
  injector_ = GetContext().Get<I3InjectorServicePtr>(injectorServiceName_);
  if(!injector_) log_fatal("No injector service \"%s\" in context", injectorServiceName_.c_str());
  
  GetParameter("ResponseServiceName", responseServiceName_);
  log_info("+ Response Service: %s", responseServiceName_.c_str());
  response_ = GetContext().Get<I3IceTopResponseServicePtr>(responseServiceName_);
  if(!response_) log_fatal("No response service \"%s\" in context", responseServiceName_.c_str());
  
  GetParameter("PrimaryName", mcPrimaryName_);
  log_info("+ MC primary: %s", mcPrimaryName_.c_str());
  
  GetParameter("IceTopHitSeriesName", hitSeriesName_);
  log_info("+ IceTop hit series : %s", hitSeriesName_.c_str());
  
  GetParameter("IceTopPESeriesName", peSeriesName_);
  log_info("+ IceTop pe series : %s", peSeriesName_.c_str());

  GetParameter("IceTopCherenkovHitSeriesName", cherSeriesName_);
  log_info("+ IceTop Cherenkov series : %s", cherSeriesName_.c_str());

  GetParameter("InIceMCTreeName", icMCTreeName_);
  log_info("+ InIce particle tree: %s", (icMCTreeName_.empty())?"DISABLED":icMCTreeName_.c_str());

  GetParameter("IceTopMCTreeName", itMCTreeName_);
  log_info("+ IceTop particle tree: %s", (itMCTreeName_.empty())?"DISABLED":itMCTreeName_.c_str());
  
  GetParameter("HitBinWidth", hitBinWidth_);
  hitBinWidth_*=I3Units::ns;
  if(hitBinWidth_>0) log_info("+ Hit bin width: %.1f ns", hitBinWidth_/I3Units::ns);
  else               log_fatal("Hit bin width must be greater than zero!");
  
  GetParameter("MuonEnergyCutOff", muEnergyCut_);
  muEnergyCut_*=I3Units::GeV;
  log_info("+ Muon cutoff energy: %.2f GeV", muEnergyCut_/I3Units::GeV);
  
  GetParameter("WriteEventHeader", writeEvtHeader_);
  log_info("+ Event header: %s", writeEvtHeader_?"ENABLED":"DISABLED");
  
  GetParameter("IceTopTestPulsesName", testPulsesName_);

  std::vector<std::string> tankKeys;
  GetParameter("Tanks", tankKeys);
  if (tankKeys.size() > 0) {
    log_info("+ Setting tanks");
    for (unsigned int i = 0; i != tankKeys.size(); ++i) {
      tankKeys_.insert(GetTankKey(tankKeys[i]));
    }
  }

  GetParameter("CompressPEs", compressPEs_);
  std::string msg;
  switch (compressPEs_) {
  case 0:
    msg = "0 - keep full source particle information";
    break;
  case 1:
    msg = "1 - keep particle class (gamma, electron, muon, hadron)";
    break;
  case 2:
    msg = "2 - no source particle information";
    break;
  default:
    throw std::runtime_error("unknown value for compressPEs_");
  }
  log_info("+ PE compression: %s", msg.c_str());

  GetParameter("UseInjectorComponents", useInjectorComponents_);
  useInjectorComponents_ *= (useInjectorComponents_==1);
  if (useInjectorComponents_) {
    log_info("Using the particle class information given by the injector.");
  }
  else {
    log_warn("Using the particle class information given by I3TopSimulator");
  }
}


void I3TopSimulator::DetectorStatus(I3FramePtr frame)
{
  // Configure and initialize response and injector services
  response_->Configure();
  response_->Initialize(frame->Get<I3Geometry>(),
      frame->Get<I3Calibration>(),
      frame->Get<I3DetectorStatus>()); 
  
  injector_->SetResponseService(response_);
  injector_->Configure();
  
  // Debugging stuff
  // response_->Print();
  // response_->FillHistogram();

  PushFrame(frame);
}


void I3TopSimulator::DAQ(I3FramePtr frame)
{
  log_debug("   Simulating IceTop");
  int runID, evtID;
  I3ParticlePtr mcPrimary(new I3Particle(I3Particle::Null, I3Particle::unknown));
  if(!injector_->NextEvent(runID, evtID, *mcPrimary, frame))
  {
      // Exit here and stop IceTray if no more events are available
      RequestSuspension();
      log_debug("   No more events.");
      // No PushFrame here!
      return; 
  }
  
  // If InIce MCTree is enabled create an empty tree
  I3MCTreePtr mcTreeIC = I3MCTreePtr();
  if(!icMCTreeName_.empty()) 
  {
      mcTreeIC = I3MCTreePtr(new I3MCTree()); 
      I3MCTreeUtils::AddPrimary(*mcTreeIC, *mcPrimary);
  }
  
  // If IceTop MCTree is enabled create an empty tree
  I3MCTreePtr mcTreeIT = I3MCTreePtr();
  if(!itMCTreeName_.empty()) 
  {
      mcTreeIT = I3MCTreePtr(new I3MCTree()); 
      I3MCTreeUtils::AddPrimary(*mcTreeIT, *mcPrimary);
  }

  // Tell the response service of a new event
  response_->BeginEvent(*mcPrimary);

  std::map<std::string, int> component_map;
  if (useInjectorComponents_) {
    component_map = injector_->GetAirShowerComponentNameMap();
  }
  else {
    component_map = GetAirShowerComponentNameMap();
  }
  std::vector<int> components;
  for (std::map<std::string, int>::const_iterator it = component_map.begin(); it != component_map.end(); ++it) {
    components.push_back(it->second);
  }
  HitHistoCollection hitHistos(hitBinWidth_, compressPEs_ > 0, components);
  HitHistoCollection cherenkovHistos(hitBinWidth_, true, components);
  ExtendedI3Particle injectorParticle;
  while(injector_->NextParticle(injectorParticle))
  {
    // create new particle with unique particle ID, so that
    // mapping between I3MCTrees and I3MCPE maps works,
    // injector particles all have same ID :-(
    ExtendedI3Particle particle;
    particle.SetType(injectorParticle.GetType());
    particle.SetShape(injectorParticle.GetShape());
    particle.SetPos(injectorParticle.GetPos());
    particle.SetDir(injectorParticle.GetDir());
    particle.SetTime(injectorParticle.GetTime());
    particle.SetLength(injectorParticle.GetLength());
    particle.SetEnergy(injectorParticle.GetEnergy());
    particle.SetSpeed(injectorParticle.GetSpeed());

    if (useInjectorComponents_)
      particle.SetAirShowerComponent(injectorParticle.GetAirShowerComponent());
    else
      particle.SetAirShowerComponent(GetAirShowerComponent(injectorParticle));

    // Get map of tanks (tank responses) where the particle is within the sampling radius 
    I3TankResponseMapPtr respMap = response_->GetTankResponseMap(injectorParticle);
    if(respMap)
    {
      bool hitTank = false;
      I3TankResponseMap::const_iterator resp_iter;
      for(resp_iter=respMap->begin(); resp_iter!=respMap->end(); ++resp_iter)
      {
        if (tankKeys_.size() > 0 && tankKeys_.find(resp_iter->second->GetTankKey()) == tankKeys_.end())
          continue;
        hitTank |= resp_iter->second->TrackParticle(particle, hitHistos, cherenkovHistos);
      }
      
      // Store particles which hit the tanks in IceTop MCTree
      if(mcTreeIT && hitTank)
      {        
        particle.SetShape(I3Particle::TopShower); 
        particle.SetLocationType(I3Particle::IceTop);
        
        I3MCTreeUtils::AppendChild(*mcTreeIT, *mcPrimary, particle);
      }
    }
  
    // Store muons above the energy threshold in InIce MCTree
    if(mcTreeIC &&
       (particle.GetType()==I3Particle::MuMinus ||
        particle.GetType()==I3Particle::MuPlus) &&
       particle.GetEnergy() > muEnergyCut_)
    {   
      particle.SetShape(I3Particle::InfiniteTrack); 
      particle.SetLength(0);
      particle.SetLocationType(I3Particle::InIce);
      
      I3MCTreeUtils::AppendChild(*mcTreeIC, *mcPrimary, particle);
    }
  }
  
  // Tell the response service that the event has ended
  response_->EndEvent(hitHistos, cherenkovHistos);
  
  if(writeEvtHeader_) WriteEventHeader(frame, runID, evtID);
  
  // Put primary to the frame (if it is valid)
  if(!mcPrimaryName_.empty() && mcPrimary->GetType()!=I3Particle::unknown)
  {
    frame->Put(mcPrimaryName_, mcPrimary);
  }
    
  if (compressPEs_ == 1) {
    I3MapStringIntPtr component_map_ptr(new I3MapStringInt());
    for (I3MapStringInt::iterator it = component_map.begin(); it != component_map.end(); ++it) {
      component_map_ptr->insert(std::pair<std::string, int>(it->first, it->second));
    }
    frame->Put("AirShowerComponents", component_map_ptr);
  }

  // Put additional primary info to the frame (if it exists)
  I3FrameObjectConstPtr eventInfo = injector_->GetEventInfo();
  if(eventInfo) frame->Put(mcPrimaryName_+"Info", eventInfo);
  
  // Put the MCTrees to the frame (if they exist)
  if(mcTreeIC) frame->Put(icMCTreeName_, mcTreeIC);
  if(mcTreeIT) frame->Put(itMCTreeName_, mcTreeIT);
  
  // Put the IceTop hits to the frame 
  if (!hitSeriesName_.empty()) {
    I3MCHitSeriesMapPtr hsm = hitHistos.GenerateMap<I3MCHitSeries>();
    log_debug("I3MCHit runID = %d, evtID = %d, nch = %zu", runID, evtID, hsm->size());
    frame->Put(hitSeriesName_, hsm);
  }
  if (!peSeriesName_.empty()) {
    I3MCPESeriesMapPtr pes;
    if (compressPEs_ == 1) {
      pes = hitHistos.GeneratePEClassMap();
    } else {
      pes = hitHistos.GenerateMap<I3MCPESeries>();
    }
    log_debug("I3MCPE runID = %d, evtID = %d, nch = %zu", runID, evtID, pes->size());
    frame->Put(peSeriesName_, pes);     
  }

  // Put the IceTop reco pulses to the frame
  if(!testPulsesName_.empty()) {
    I3RecoPulseSeriesMapPtr rpsm = hitHistos.GenerateMap<I3RecoPulseSeries>();
    log_debug("I3RecoPulse runID = %d, evtID = %d, nch = %zu", runID, evtID, rpsm->size());
    frame->Put(testPulsesName_, rpsm);
  }

  // Put IceTop Cherenkov pulses in frame
  if (!cherSeriesName_.empty()) {
    I3RecoPulseSeriesMapPtr cherPulses = cherenkovHistos.GenerateMap<I3RecoPulseSeries>();
    log_debug("Cherenkov RecoPulse runID = %d, evtID = %d, nch = %zu", runID, evtID, cherPulses->size());
    frame->Put(cherSeriesName_, cherPulses);
  }

  // Write bin width to the frame (required by pmt-simulator)
  frame->Put("HitBinWidth", I3DoublePtr(new I3Double(hitBinWidth_)));

  PushFrame(frame);
}


void I3TopSimulator::WriteEventHeader(I3FramePtr frame, int runID, int evtID)
{  
  // Get the event time (driving time)
  I3Time startTime(0,0);
  I3TimeConstPtr drivingTime = frame->Get<I3TimeConstPtr>("DrivingTime");
  if(drivingTime) startTime = *drivingTime;
  
  // Create new header
  I3EventHeaderPtr header = I3EventHeaderPtr(new I3EventHeader());
  header->SetStartTime(startTime);
  header->SetRunID(runID);
  header->SetSubRunID(0);
  header->SetEventID(evtID);

  frame->Put("IceTopSimulatorHeader", header);
}


TankKey
I3TopSimulator::GetTankKey(std::string key)
  const
{
  TankKey::TankID tank;
  char a = key[key.size()-1];
  switch (a) {
  case 'A':
  case 'a':
    tank = TankKey::TankA;
    break;
  case 'B':
  case 'b':
    tank = TankKey::TankB;
    break;
  default:
    log_fatal("Misconfigured 'Tanks' parameter, %s", key.c_str());
  }
  int string = std::atoi(key.substr(0, key.size()-1).c_str());
  log_debug("  + Getting tank key: %d %c", string, a);
  return TankKey(string, tank);
}


std::map<std::string, int> I3TopSimulator::GetAirShowerComponentNameMap() const
{
  std::map<std::string, int> component_map;
  component_map["Gamma"] = Gamma;
  component_map["Electron"] = Electron;
  component_map["Muon"] = Muon;
  component_map["Hadron"] = Hadron;
  return component_map;
}


AirShowerComponent
I3TopSimulator::GetAirShowerComponent(const I3Particle& p)
  const
{
  switch (p.GetType()) {
  case I3Particle::Gamma:
    return Gamma;
  case I3Particle::EPlus:
  case I3Particle::EMinus:
    return Electron;
  case I3Particle::MuPlus:
  case I3Particle::MuMinus:
    return Muon;
  default: // everything else has to be hadrons
    break;
  }
  return Hadron;
}
