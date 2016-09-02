/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3CorsikaInjector.cxx
 * @version $Rev: $
 * @date $Date: $
 * @authors Tilo Waldenmaier, Alessio Tamburro
 */

#include <icetray/I3SingleServiceFactory.h>
#include <topsimulator/interface/I3InjectorFactory.h>
#include <topsimulator/injectors/I3CorsikaInjector.h>
//#include <phys-services/I3GSLRandomService.h>
#include <dataclasses/I3Constants.h>
#include <limits.h>
#include <cstdlib>

// Maximum number of samples per shower (must be a power of 10!)
const int I3CorsikaInjector::MAX_SAMPLES = 10000;

// Bin width for binning of particle distances to tank (needed for particle density map)
const double I3CorsikaInjector::DIST_BIN_WIDTH = 10.0*I3Units::cm;

// Maximum zenith angle for weighted particles
const double I3CorsikaInjector::MAX_ZENITH = 88.9*I3Units::deg;

using namespace topsim;



I3CorsikaInjector::I3CorsikaInjector(const I3Context& context):
  I3InjectorService(context),
  corsikaFiles_(0),
  relocationStation_(-1),
  relocationX_(.0),
  relocationY_(.0),
  relocationR_(.0),
  shiftX_(NAN),
  shiftY_(NAN),
  currentRunID_(-1),
  currentEvtID_(-1),
  numSamples_(1),
  sampleIndex_(MAX_SAMPLES),
  numHpTMuons_(0),
  hpTMuonCounter_(0),
  hpTMuTotalMomentum_(.0),
  hpTMuTransMomentum_(.0),
  cloneBuffer_(0),
  dcheck_(.0),
  alreadyWarned_(false),
#ifdef I3_USE_ROOT
  debugHistogram_(NULL),
#endif
  samplingWeight_(1.),
  weightedRandomSampling_(false),
  samplingRegionSide_(16*I3Units::km),
  onRegionSide_(5.*I3Units::m),
  arraySide_(1.5*I3Units::km),
  tankSampleDistance_(0*I3Units::m),
  randomServiceName_(""),
  randomService_(I3RandomServicePtr())
{
  ClearBuffers();
  
  corsikaFiles_.clear(); 
  AddParameter("FileNameList",
	       "List of CORSIKA file names.",
	       corsikaFiles_);
  
  relocationStation_ = -1;
  AddParameter("RelocationStation",
	       "Relocate showers centered on given station. "
	       "If set to a value > 0, RelocationX and RelocationY parameters are ignored.",
	       relocationStation_);

  std::vector<std::string> tankKeys;
  AddParameter("Tanks",
	       "Relocate showers around given tanks. ",
	       tankKeys);

  weightedRandomSampling_ = false;
  AddParameter("ImportanceSampling",
	       "Relocate showers centered at a location radomly sampled from shower particle distribution. "
	       "If set to a value > 0, RelocationX and RelocationY parameters are set to a randomly sampled location from shower particle distribution.",
	       weightedRandomSampling_);

  relocationX_ = 0.0;
  AddParameter("RelocationX",
	       "X center of relocation area in [m].",
	       relocationX_);
    
  relocationY_ = 0.0;
  AddParameter("RelocationY",
	       "Y center of relocation area in [m].",
	       relocationY_);

  relocationR_ = 0.0;
  AddParameter("RelocationR",
	       "Relocation radius in [m]. If zero, shower core(s) will be exactly at (relocationX, relocationY).",
	       relocationR_);
  
  numHpTMuons_ = 0;
  AddParameter("NumHpTMuons",
	       "Number of high pT muons to be inserted at the end of the simulation.",
	       numHpTMuons_);
  
  hpTMuTotalMomentum_ = 1000.0;
  AddParameter("HpTMuonTotalMomentum",
	       "Total momentum of high pT muons in [GeV/c].",
	       hpTMuTotalMomentum_);
  
  hpTMuTransMomentum_ = 8.0;
  AddParameter("HpTMuonTransverseMomentum",
	       "Transverse momentum of high pT muons in [GeV/c] (must be smaller than total momentum).",
	       hpTMuTransMomentum_);
  
  numSamples_ = 1;
  AddParameter("NumSamples",
	       "Number of samples per shower.",
	       numSamples_);

  dcheck_ = 25.0;
  AddParameter("UnThinRadius",
	       "Sampling radius in [m] around the tanks used for the un-thinning."
	       " Must be smaller or equal than the tank sampling radius",
	       dcheck_);
  
  randomServiceName_ = "";
  AddParameter("RandomServiceName",
	       "Name of the random service in the context. This has to be specified or you'll get fatal error",
	       randomServiceName_);
  
#ifdef I3_USE_ROOT
  AddParameter("PartDistr",
	       "Name of a rootfile to store particle distributions.",
	       "");
#endif

  AddParameter("IgnoreParticleTypes",
	       "List of particle types to skip over (for instance, muons are 5 and 6)",
	       ignoreTypes_);

  AddParameter("SamplingArea",
               "Total area over which to place the showers. For now, used only when ImportanceSampling==True.",
               samplingRegionSide_*samplingRegionSide_);

  AddParameter("OnRegionArea",
               "Area of the ON regions. Used only when ImportanceSampling==True.",
               onRegionSide_*onRegionSide_);

  AddParameter("TankSampleDistance",
               "Margin around each tank. All particles within this distance are added to the MCTree. Used only when ImportanceSampling==True.",
               tankSampleDistance_);

  raiseObsLevel_ = 0.0;
  AddParameter("RaiseObservationLevel",
	       "Adjust the z-coordinate of the observation level upward by this many meters. Only for correcting observation level problems after-the-fact. Default=0.",
	       raiseObsLevel_);
}


I3CorsikaInjector::~I3CorsikaInjector()
{
  ClearBuffers();
}


void I3CorsikaInjector::ClearBuffers()
{
  cloneBuffer_.clear();
  optDistMap_.clear();
}


void I3CorsikaInjector::Configure()
{
  log_info("Configuring the I3CorsikaInjector:");
  
  GetParameter("FileNameList", corsikaFiles_);
  
  GetParameter("RelocationX", relocationX_);
  relocationX_*=I3Units::m;
    
  GetParameter("RelocationY", relocationY_);
  relocationY_*=I3Units::m;
  
  GetParameter("RelocationStation", relocationStation_);

  std::vector<std::string> tankKeys;
  GetParameter("Tanks", tankKeys);
  if (tankKeys.size() > 0) {
    log_debug("+ Setting tanks");
    for (unsigned int i = 0; i != tankKeys.size(); ++i) {
      tankKeys_.push_back(GetTankKey(tankKeys[i]));
    }
  }
  else {
    log_debug("+ Default tanks");
    for (int string_id = 1; string_id <= 81; ++string_id ) {
      for (int k = 0; k != 2; ++k) {
        tankKeys_.push_back(TankKey(string_id, TankKey::TankID(k)));
      }
    }
  }

  GetParameter("ImportanceSampling", weightedRandomSampling_);
  
  // In case a relocationStation is specified, use the station position as relocation center
  if(relocationStation_ > 0)
  {
    log_info("+ Relocation station: %d", relocationStation_);
    
    I3TankResponsePtr tankA = responseService_->GetTankResponse(TankKey(relocationStation_, TankKey::TankA));
    I3TankResponsePtr tankB = responseService_->GetTankResponse(TankKey(relocationStation_, TankKey::TankB));
    if(!tankA && !tankB)
    {
      log_fatal("Selected station (%d) does not exist in geometry.", relocationStation_);
    }
    else if(!tankA)
    { 
      log_warn("Tank A of selected station %d does not exist in geometry. Using the center of tank B.", relocationStation_);
      relocationX_ = tankB->GetX();
      relocationY_ = tankB->GetY();
    } 
    else if (!tankB)
    { 
      log_warn("Tank B of selected station %d does not exist in geometry. Using the center of tank A.", relocationStation_);
      relocationX_ = tankA->GetX();
      relocationY_ = tankA->GetY();
    } 
    else 
    {
      relocationX_ = 0.5*(tankA->GetX()+tankB->GetX());
      relocationY_ = 0.5*(tankA->GetY()+tankB->GetY());
    }
  }
  
  log_info("+ Relocation center: (%.2f, %.2f) m", relocationX_/I3Units::m, relocationY_/I3Units::m);
    
  GetParameter("RelocationR", relocationR_);
  relocationR_*=I3Units::m;
  log_info("+ Relocation radius: %.2f m", relocationR_/I3Units::m);
  
  GetParameter("NumHpTMuons", numHpTMuons_);
  GetParameter("HpTMuonTotalMomentum", hpTMuTotalMomentum_);
  GetParameter("HpTMuonTransverseMomentum", hpTMuTransMomentum_);
  hpTMuTotalMomentum_*=I3Units::GeV;
  hpTMuTransMomentum_*=I3Units::GeV;
  if(numHpTMuons_>0)
  {
    log_info("+ High pT muons    : num = %d, p = %.2f GeV/c, pT = %.2f GeV/c",
	     numHpTMuons_, hpTMuTotalMomentum_/I3Units::GeV, hpTMuTransMomentum_/I3Units::GeV);
    
    if(hpTMuTransMomentum_>hpTMuTotalMomentum_)
    {
      log_fatal("Transverse momentum exceeds total momentum of high pT muons!");
    }
  }
    
  GetParameter("NumSamples", numSamples_);
  if(numSamples_<=0)
  {
    log_fatal("Number of samples must be larger than zero. Check your python script!");
  }
  if(numSamples_>MAX_SAMPLES)
  {
    log_fatal("Number of samples cannot exceed %d. Check your python script!", MAX_SAMPLES);
  }
  log_info("+ Number of re-samples: %d", numSamples_);
  nominalNumSamples_ = numSamples_;
  
  GetParameter("RandomServiceName", randomServiceName_);
  if(randomServiceName_.empty())
  {
    log_fatal("No random service provided!");
  }
  else
  {
    randomService_ = GetContext().Get<I3RandomServicePtr>(randomServiceName_); 
    if(randomService_) log_info("+ Random service: %s  (EXTERNAL)",  randomServiceName_.c_str());
    else               log_fatal("No random service \"%s\" in context!", randomServiceName_.c_str()); 
  }
  
  // Setting start value for un-thinning radius
  // Tilo: Why not directly use tank samling radius from response service (see below)?
  GetParameter("UnThinRadius", dcheck_);
  dcheck_*=I3Units::m;
    
  // Check if dcheck_ exceeds tank sampling radius
  if(responseService_ && dcheck_ > responseService_->GetTankSamplingRadius())
  {
    log_warn("UnThinRadius (%.2f m) exceeds TankSamplingRadius (%.2f m). Setting it to %.2f m!",
	     dcheck_/I3Units::m, 
	     responseService_->GetTankSamplingRadius()/I3Units::m,
	     responseService_->GetTankSamplingRadius()/I3Units::m);
    
    dcheck_ = responseService_->GetTankSamplingRadius();
  }
  else if (!responseService_)
    log_warn("No response service set");
  
#ifdef I3_USE_ROOT
  if (weightedRandomSampling_)
  {
    std::string pDistrName;
    GetParameter("PartDistr", pDistrName);

    if (!pDistrName.empty()) {
      pDistr_.reset(new TFile(pDistrName.c_str(),"RECREATE"));
      debugHistogram_ = new TH2D();
      debugHistogram_->SetDirectory(pDistr_.get()); // root sucks
      log_info("-------------> Particle distribution will be saved to \"%s\".", pDistrName.c_str());
    }
  }
#endif

  if (weightedRandomSampling_)
  {
    // this really looks retarded
    const int nbins(static_cast<int>(ceil(samplingRegionSide_/onRegionSide_) + 0.5));
    const int arraybins(static_cast<int>(ceil(arraySide_/onRegionSide_) + 0.5));

    // tweak sampling region sizes so they are multiples of the bin size
    samplingRegionSide_ = nbins*onRegionSide_;
    arraySide_ = arraybins*onRegionSide_;

    particleDistr_.reset(new SparseHistogram((nbins+arraybins-1)*onRegionSide_, nbins+arraybins-1));
    phDistr_.reset(new SparseHistogram(nbins*onRegionSide_, nbins));
    arrayFootprint_.reset(new SparseHistogram(arraybins*onRegionSide_, arraybins));

  }

  GetParameter("IgnoreParticleTypes", ignoreTypes_);

  GetParameter("SamplingArea", samplingRegionSide_);
  samplingRegionSide_ = sqrt(samplingRegionSide_);

  GetParameter("OnRegionArea", onRegionSide_);
  onRegionSide_ = sqrt(onRegionSide_);

  GetParameter("TankSampleDistance", tankSampleDistance_);

  GetParameter("RaiseObservationLevel", raiseObsLevel_);

  reader_.SetFileList(corsikaFiles_); 
  ClearBuffers();
  sampleIndex_ = MAX_SAMPLES; 
}


bool I3CorsikaInjector::NextEvent(int& runID, int& evtID, I3Particle& primary, I3FrameConstPtr frame)
{
  // Reset or increment sample index
  if(sampleIndex_>=numSamples_-1) sampleIndex_ = 0;
  else sampleIndex_++;
    
  if(sampleIndex_>0)
  {
    log_trace("Rewinding event");
    reader_.Rewind();
  }
  
  if(!reader_.NextEvent(primary, currentEvtID_))
  {
    if(!reader_.NextRun(currentRunID_))            return false;
    // "NextRun" is where the z of the observation level gets computed, so 
    // here is where you should "hack the observation level" (before reading in any particles)
    reader_.HackObsLevelUpwards(raiseObsLevel_);
    if(!reader_.NextEvent(primary, currentEvtID_)) return false;
    sampleIndex_ = 0; 
    alreadyWarned_ = false;
  }

  // Check if eventID will exceed INT_NAX
  if(currentEvtID_>((INT_MAX - MAX_SAMPLES)/MAX_SAMPLES))
  {
    log_error("Event number exceeds INT_MAX=%d. ", INT_MAX);
    return false;
  }
    
  // Create eventID as a combination of the
  // CORSIKA eventID and the sampling index
  evtID = currentEvtID_*MAX_SAMPLES + sampleIndex_;
  runID = currentRunID_;

  log_debug("Event %d, Run %d", evtID, runID);
  
  //Read once at the beginning to make particle distribution
  if (weightedRandomSampling_ && sampleIndex_==0) {
    CalculateOnRegions(primary);
  }

  if (weightedRandomSampling_){
    if (phDistr_->GetEntries() == 0) {
      shiftX_ = 0;
      shiftY_ = 0;

      log_debug("No ON regions.");
    }
    else if (relocsX_.size())  {
      shiftX_ = relocsX_.back();
      shiftY_ = relocsY_.back();
      relocsX_.pop_back();
      relocsY_.pop_back();

      log_debug("Trying (%f, %f), %lu remaining", shiftX_, shiftY_, relocsX_.size());
    }
    else{
      log_debug("No more ON regions.");
      return true;
    }
  }
  else{
    // Relocate the shower
    shiftX_ = relocationX_;
    shiftY_ = relocationY_;
  
    if(relocationR_>0)
    {
      double phi    = randomService_->Uniform(0, 360*I3Units::deg)/I3Units::rad;
      double radius = sqrt(randomService_->Uniform(0, relocationR_*relocationR_));
      shiftX_ += radius*cos(phi);
      shiftY_ += radius*sin(phi);
    }
  }
    
  RelocateParticle(primary, shiftX_, shiftY_);
  primary_ = primary;
  
  // Reset the high pT muon counter
  hpTMuonCounter_ = 0;
  
  // Reset buffers for un-thinning
  ClearBuffers();
  
  // Compute map of optimal un-thinning radii for each tank
  // Needed for un-thinning
  ComputeOptDistances();
  
  return true;
}


void
I3CorsikaInjector::CalculateArrayFootprint(const I3Particle& primary)
{
  // Each tank's footprint will be a square centered at the tank.
  // The half-side of the square is given by a sampling radius
  // plus half the bin size to account for the uncertainty in tank location within the bin
  // This is rounded up so it is a multiple of the bin size.
  const int delta(static_cast<int>(std::ceil((tankSampleDistance_ + onRegionSide_/2)/onRegionSide_) + 0.5));

  // We will shift each tank's footprint according to the primary's direction.
  // Otherwise the sampling radius must account for the horizontal displacement that occurs when
  // transporting the particle from the injection altitude to the tank's altitude.
  const I3Direction& dir = primary.GetDir();
  const double z = primary.GetPos().GetZ();

  arrayFootprint_->Reset();
  const int bins = arrayFootprint_->GetBins();
  for (unsigned int tk = 0; tk != tankKeys_.size(); ++tk) {
    I3TankResponsePtr tank = responseService_->GetTankResponse(tankKeys_[tk]);
    if(!tank)
      continue;
    const int i = arrayFootprint_->FindBin(tank->GetX() + (z - tank->GetZ())*dir.GetX()/dir.GetZ());
    const int j = arrayFootprint_->FindBin(tank->GetY() + (z - tank->GetZ())*dir.GetY()/dir.GetZ());
    for (int k = (i>delta?i-delta:0); k <= i+delta && k < bins; ++k)
      for (int l = (j>delta?j-delta:0); l <= j+delta && l < bins; ++l) {
        arrayFootprint_->Set(k, l, 1.);
      }
  }
}


void
I3CorsikaInjector::CalculateOnRegions(const I3Particle& primary)
{
  CalculateArrayFootprint(primary);

  log_debug("  Calculating ON regions");
  // Fill a histogram of particle positions (actually core displacements so the particles are at the origin)
  double weight;
  double hadgen;
  I3Particle particle;
  particleDistr_->Reset();
  while(reader_.NextParticle(particle, weight, hadgen, ignoreTypes_)) {
    particleDistr_->Fill(-particle.GetX(),-particle.GetY());
  }
  reader_.Rewind();
  int checkEvtID;
  I3Particle tmpPrimary;
  reader_.NextEvent(tmpPrimary, checkEvtID);

  log_debug("  Done iterating over particles in run %d, event %d", currentRunID_, currentEvtID_);

  // Fill the histogram of on regions
  topsim::Convolve(*arrayFootprint_, *particleDistr_, *phDistr_);
  log_debug("  Done calculating ON regions");

#ifdef I3_USE_ROOT
  if (pDistr_) {
    pDistr_->cd();

    std::ostringstream name;
    ConvertToROOT(*particleDistr_, debugHistogram_);
    name << "particles_" << currentRunID_ << "_" << currentEvtID_;
    debugHistogram_->Write(name.str().c_str());

    ConvertToROOT(*phDistr_, debugHistogram_);
    name.str("");
    name << "on_regions_" << currentRunID_ << "_" << currentEvtID_;
    debugHistogram_->Write(name.str().c_str());

    ConvertToROOT(*arrayFootprint_, debugHistogram_);
    name.str("");
    name << "array_" << currentRunID_ << "_" << currentEvtID_;
    debugHistogram_->Write(name.str().c_str());
  }
#endif

  log_debug("  Generating core locations...");

  // From now on, calculate the locations that will be sampled
  // get the bins with anything in them
  std::vector<SparseHistogramIndex> onRegions = phDistr_->GetNonEmptyBins();

  for (unsigned int i = 0; i != onRegions.size(); ++i) {
    log_trace("  ON region %d: (%f, %f)", int(i), phDistr_->GetBinCenter(onRegions[i].x), phDistr_->GetBinCenter(onRegions[i].y));
  }

  log_debug("  ON regions: %d", int(onRegions.size()));

  const int n_on = onRegions.size();

  if (phDistr_->GetEntries() > 0) {
    /*
      If there are bins with particles, chose 'n_draw' bins randomly (with substitution) from them.
      'n_drawn' is the minimum of 'numSamples_' and the number of bins with particles ('n_on').
     */

    const unsigned int n_drawn(n_on <= nominalNumSamples_?n_on:nominalNumSamples_);

    log_trace("  All core locations:");
    // Place each event at a random point inside a randomly chosen bin.
    for (unsigned int i = 0; i < n_drawn; ++i) {
      const int ir = randomService_->Integer(n_on);
      const double delta = 0.5*phDistr_->GetBinWidth();
      const double px = phDistr_->GetBinCenter(onRegions[ir].x) +
        randomService_->Uniform(-delta, delta);
      const double py = phDistr_->GetBinCenter(onRegions[ir].y) +
        randomService_->Uniform(-delta, delta);
      relocsX_.push_back(px);
      relocsY_.push_back(py);
      log_trace("   - Location (%f, %f) around (%f, %f), %d", px, py, phDistr_->GetBinCenter(onRegions[ir].x), phDistr_->GetBinCenter(onRegions[ir].y), ir);
    }

    log_debug("  Samples: %d", n_drawn);
  }
  else {
    log_debug("  There are no particles in the whole sampling region.");
  }

  numSamples_ = (relocsX_.size()? relocsX_.size(): 1);
  const int nbins = phDistr_->GetBins();
  samplingWeight_ = (double(nominalNumSamples_)/numSamples_)*n_on/(nbins*nbins);

  log_debug("  Done generating core locations.");


}

bool I3CorsikaInjector::NextParticle(ExtendedI3Particle& particle)
{  
  // If clone buffer not empty return
  // clones first before reading new
  // particles from CORSIKA file
  if(!cloneBuffer_.empty())
  {
    particle = ExtendedI3Particle(cloneBuffer_.back());
    cloneBuffer_.pop_back();
    return true;
  }
  
  double weight;
  double hadgen;
  while(reader_.NextParticle(particle, weight, hadgen, ignoreTypes_))
  {
    particle.SetAirShowerComponent(GetAirShowerComponent(particle, hadgen));
    RelocateParticle(particle, shiftX_, shiftY_);
    
    // If particle is weighted generate clones
    if(weight>1.0)
    {
      GenerateClones(particle, weight);
      if(cloneBuffer_.empty()) continue;
      
      particle = ExtendedI3Particle(cloneBuffer_.back());
      cloneBuffer_.pop_back();
    }
    
    log_trace("returning particle at (%f, %f)", particle.GetPos().GetX(), particle.GetPos().GetY());

    // If particle or clone have been found
    // exit Loop and NextParticle() method here
    return true;
  }
  
  // Last but not least generate high_pT muon(s) if this
  // option is enabled. Otherwise return FALSE and close event
  return InsertHighPtMuon(particle);
}


std::map<std::string, int> I3CorsikaInjector::GetAirShowerComponentNameMap() const
{
  static std::map<std::string, int> m;
  if (m.empty()) {
    m["Gamma"] = Gamma;
    m["Electron"] = Electron;
    m["GammaFromChargedMesons"] = GammaFromChargedMesons;
    m["ElectronFromChargedMesons"] = ElectronFromChargedMesons;
    m["Muon"] = Muon;
    m["Hadron"] = Hadron;
  }
  return m;
}


AirShowerComponent
I3CorsikaInjector::GetAirShowerComponent(const I3Particle& p, double hadgen)
  const
{
  switch (p.GetType()) {
  case I3Particle::Gamma:
    if (hadgen > 50) {
      return GammaFromChargedMesons;
    }
    else
      return Gamma;
  case I3Particle::EPlus:
  case I3Particle::EMinus:
    if (hadgen > 50)
      return ElectronFromChargedMesons;
    else
      return Electron;
  case I3Particle::MuPlus:
  case I3Particle::MuMinus:
    return Muon;
  default: // everything else has to be hadrons
    break;
  }
  return Hadron;
}


void I3CorsikaInjector::GenerateClones(const I3Particle& particle, const double& weight)
{
  // Get map of tanks which are close to particle
  I3TankResponseMapPtr respMap = responseService_->GetTankResponseMap(particle);
  if(!respMap) return;
  
  if(!alreadyWarned_ &&  
     (particle.GetType()==I3Particle::MuMinus || particle.GetType()==I3Particle::MuPlus) && 
     particle.GetEnergy() > 273.0*I3Units::GeV)
  {
    log_warn("Thinning seems to be enabled also for high energy muons. InIce muon tracks won't be correctly treated."
	     " --> Check your CORSIKA thinning options!");
    
    // Only warn for the first muon to avoid spam
    alreadyWarned_ = true;
  }
  
  // FIXME: Loop over candidate tanks
  // Tilo: Is this correct ??? Clone can either hit one or the other tank !!!
  // Tilo: What about correlated signals in multiple tanks ???
  log_warn("Check treatment of weighted particles which may hit more than one tank!");
  I3TankResponseMap::const_iterator resp_iter;
  for(resp_iter=respMap->begin(); resp_iter!=respMap->end(); ++resp_iter)
  {
    const TankKey& tankKey = resp_iter->first;
    
    // Get tank dimensions
    const double tankRadius(resp_iter->second->GetVirtualTankRadius());
    const double tankHeight(resp_iter->second->GetVirtualTankHeight()); 
    
    double dist = DistanceToTank(particle, resp_iter->second);
    dist = std::max(dist, tankRadius);
    
    // Get optimal distance for this tank
    std::map<TankKey, double>::const_iterator dist_iter = optDistMap_.find(tankKey);
    if(dist_iter==optDistMap_.end())
    {
      log_error("Didn't find optimal distance for Tank %s. Skipping particle!", tankKey.str().c_str());
      continue;
    }
    const double& doptimal = dist_iter->second;
    
    // Disregard particle if it is outside of optimal resampling area.
    if(dist > doptimal) continue;

    // Disregard weighted particles with zenith > MAX_ZENITH
    // Because they are causing infinite tank shadow areas
    if(particle.GetZenith()>MAX_ZENITH)
    {
      log_warn("Discarding nearly horizontal (zenith = %.2f deg) particle (%s) with weight %.2f at Tank %s!",
	       particle.GetZenith()/I3Units::deg, particle.GetTypeString().c_str(), weight, tankKey.str().c_str());
      continue;
    }
    
    double resArea  = I3Constants::pi*doptimal*doptimal;
		
    // The flux
    double pWeightDensity = weight/resArea;
	  
    // Average number of clones to be generated
    double avgN = pWeightDensity*GetTankShadowArea(particle, tankRadius, tankHeight);  
    
    // Get number of clones to be generated
    // The opt dist assures 1 clone/particle or something
    // and the number of particles within opt dist
    // resembles the density within dcheck_
    int nclones = randomService_->Poisson(avgN);
    
    // Generate clones and store them in buffer
    for(int i=0; i<nclones; ++i)
    {	      
      // Generate clone position on tank surface
      // in local tank coord syst z>=0, z<=tankHeight
      double x(NAN), y(NAN), z(NAN);
      SetClonePosition(x, y, z, particle, tankHeight, tankRadius);
		    
      // Convert to absolute IceCube coordinates
      x += resp_iter->second->GetX();
      y += resp_iter->second->GetY();
      z += resp_iter->second->GetZ() - 0.5*tankHeight;
		    
      // Determine time of clone on tank surface
      double dx = x - particle.GetX();
      double dy = y - particle.GetY();
      double dz = z - particle.GetZ();
		    
      double dl = dx*primary_.GetDir().GetX() + dy*primary_.GetDir().GetY() + dz*primary_.GetDir().GetZ();
		    
      // Apply a random spearing of the clone time according to a log-normal distribution
      // with sigma = 0.2 as proposed by Billoir (sigma could also be chosen by the user)
      double t = particle.GetTime() + (dl/I3Constants::c)*exp(0.2*randomService_->Gaus(0,1));
		    
      // Shift clone vertex to observation level
      ShiftToObsLevel(x, y, z, t, particle);
		    
      // Generate clone
      // Copy all settings from original particle
      // except position and time
      I3Particle clone(particle);
      clone.SetPos(x, y, z);
      clone.SetTime(t);
      
      // Add clone to buffer
      cloneBuffer_.push_back(clone);
    }
  }
}


void I3CorsikaInjector::SetClonePosition(double &x, double &y, double &z, const I3Particle& particle, const double height, const double radius)
{
  //
  // Generates random clone position on tank surface
  //
  
  double x_el, y_el, z_el;

  double alpha = particle.GetZenith();           
  double Phi_alpha = particle.GetAzimuth();

  double HA = 0.;
  double HB = height;
  double HA_HB = HA + HB;
  double Rmax = radius;

  double a_x = ( HA_HB/2. ) * sin( alpha ) + Rmax * cos( alpha ); //
  double x_loc = ( HA_HB/2. ) * sin( alpha );

  // sizes of the small ellipse
  double a_elip = Rmax * cos( alpha ) ; //minor semi-axis
  double b_elip = Rmax ; // major semi-axis
  //cout << "a,b elip:" << a_elip << " " << b_elip << endl;
  //////

  double y_upplus,y_upminus;
  double xrot1,yrot1,zrot1;
  double xrot2,yrot2,zrot2;

  do {
    x_el = 2.0 * ( randomService_->Uniform(1.0) - 0.5 ) * a_x;
    y_el = 2.0 * ( randomService_->Uniform(1.0) - 0.5 ) * Rmax;

    y_upminus = ( b_elip/a_elip ) * sqrt( a_elip * a_elip - (x_el+x_loc) * (x_el + x_loc) );
    y_upplus = ( b_elip/a_elip ) * sqrt( a_elip * a_elip - (x_el-x_loc) * (x_el - x_loc) );
  }
  while  ( !(   ( (-x_loc <= x_el &&  x_el <= x_loc) &&
                  (-Rmax < y_el && y_el <Rmax ) )
                ||
                ( ( x_loc < x_el &&  x_el < a_x ) &&
                  ( -y_upplus < y_el && y_el < y_upplus ) )
                ||
                ( ( -x_loc > x_el && -a_x < x_el) &&
                  ( -y_upminus < y_el && y_el < y_upminus) )
		)
	   );
  
  z_el = 0;
  
  // plane  rotation in around y, -alpha beacuse up for phi=0
  xrot1 = x_el * cos( -(alpha) ) - z_el * sin( -(alpha) );
  yrot1 = y_el;
  zrot1 = x_el * sin( -(alpha) ) + z_el * cos( -(alpha) );
  
  //plane rotation around z axis !!!
  //points on the surface
  xrot2 = xrot1 * cos( Phi_alpha ) - yrot1 * sin( Phi_alpha );
  yrot2 = xrot1 * sin( Phi_alpha ) + yrot1 * cos( Phi_alpha );
  zrot2 = zrot1;

  //direction perpendicular to the surface
  //and pointing upward
  I3Particle direction;
  direction.SetDir( -cos(Phi_alpha)*sin(alpha),
		    -sin(Phi_alpha)*sin(alpha),
		    -cos(alpha));
  direction.SetLength(1.0);

  if ( zrot2 >= -height/2. && zrot2 <= height/2. )
  {
    //cout << "inside" << endl;
    
    I3Position pointSV (xrot2, yrot2, zrot2 + height/2.) ;
    I3Position pointSV_inter( xrot2, yrot2, zrot2 + height/2. );
    
    double dr = 0.001; // step of backward displacement
    int j = 0;

    double radius_match2 = pow(pointSV_inter.GetX(), 2) + pow(pointSV_inter.GetY(), 2);
    double height_match = pointSV_inter.GetZ();

    while ( radius_match2 <= pow(radius,2) && height_match <= height )
      {
	pointSV_inter.SetX( pointSV.GetX() - j * dr * direction.GetDir().GetX() );
	pointSV_inter.SetY( pointSV.GetY() - j * dr * direction.GetDir().GetY() );
	pointSV_inter.SetZ( pointSV.GetZ() - j * dr * direction.GetDir().GetZ() );

	radius_match2 = pow(pointSV_inter.GetX(), 2) + pow(pointSV_inter.GetY(), 2);
	height_match = pointSV_inter.GetZ();
	
	if ( radius_match2 * height_match >= pow(radius,2) * (height) )
	  {	    
	    break;
	  }
	
	j = j + 1;
      }
    
    xrot2 = pointSV_inter.GetX();
    yrot2 = pointSV_inter.GetY();
    zrot2 = pointSV_inter.GetZ();
    
    x = xrot2;
    y = yrot2;
    z = zrot2;
  }
  else if ( zrot2 < -height/2. )
    {
      //cout << "lower-outside" << endl;
      
      I3Position pointSV( xrot2, yrot2, zrot2 + height/2. ); // vector defining the point on the surface
      
      I3Position pointSV_inter( xrot2, yrot2, zrot2 + height/2. ); // vector defining the point on the surface

      double dr = 0.001; // step of backward movement
      int j = 0;
      double radius_match2 = pow(pointSV_inter.GetX(), 2) + pow(pointSV_inter.GetY(), 2);
      double height_match = pointSV_inter.GetZ();

      while ( radius_match2 <= pow(radius,2)  && height_match <= height )
	{
	  pointSV_inter.SetX(pointSV.GetX() - j * dr * direction.GetDir().GetX() );
	  pointSV_inter.SetY(pointSV.GetY() - j * dr * direction.GetDir().GetY() );
	  pointSV_inter.SetZ(pointSV.GetZ() - j * dr * direction.GetDir().GetZ() );

          radius_match2 = pow(pointSV_inter.GetX(), 2) + pow(pointSV_inter.GetY(), 2);
          height_match = pointSV_inter.GetZ();
	  
          j = j + 1;
	}
      
      xrot2 = pointSV_inter.GetX();
      yrot2 = pointSV_inter.GetY();
      zrot2 = pointSV_inter.GetZ();
      
      x = xrot2;
      y = yrot2;
      z = zrot2;

    }
  else if ( zrot2 > height/2. )
    {
      //cout << "upper-outside" << endl;
      
      I3Position pointSV( xrot2, yrot2, zrot2 + height/2. ); // vector defining the point on the surface
      
      I3Position pointSV_inter( xrot2, yrot2, zrot2 + height/2. ); // vector defining the point on the surface
      
      double dr = 0.001; // step of backward movement
      int j = 0;
      double radius_match2 = pow(pointSV_inter.GetX(), 2) + pow(pointSV_inter.GetY(), 2);
      double height_match = pointSV_inter.GetZ();
      
      while ( radius_match2 <= pow(radius,2)  && height_match >= height )
	{
	  pointSV_inter.SetX(pointSV.GetX() + j * dr * direction.GetDir().GetX() );
	  pointSV_inter.SetY(pointSV.GetY() + j * dr * direction.GetDir().GetY() );
	  pointSV_inter.SetZ(pointSV.GetZ() + j * dr * direction.GetDir().GetZ() );

	  radius_match2 = pow(pointSV_inter.GetX(), 2) + pow(pointSV_inter.GetY(), 2);
          height_match = pointSV_inter.GetZ();
	  
          j = j + 1;
	}
      
      xrot2 = pointSV_inter.GetX();
      yrot2 = pointSV_inter.GetY();
      zrot2 = pointSV_inter.GetZ();
      
      
      x = xrot2;
      y = yrot2;
      z = zrot2;
      
    }


    // Rounding off positions
    if( x*x + y*y > radius*radius)
    {
      double corr = sqrt(x*x + y*y) - radius;
      double angle = asin(fabs(y)/sqrt(x*x + y*y));
      
      double xcorr  = fabs(x) - cos(angle)*corr;
      if( x < 0 ) x = -xcorr;
      else        x = xcorr;
      
      double ycorr   = fabs(y) - sin(angle)*corr;
      if ( y < 0 ) y = -ycorr;
      else         y = ycorr;
    }
    
    if( z < 0 ) z = 0;
    if( z > height ) z = height;
}


void I3CorsikaInjector::ComputeOptDistances()
{
  // This method computes the optimal sampling regions for each tank
  // if the shower is thinned. Otherwise exit here.
  if(!reader_.IsThinned()) return;
  log_debug("Computing optimal distances to tanks ...");

  // Loop over all shower particles and fill 
  // density map for each tank
  double weight;
  I3Particle particle;
  std::map<TankKey, DensityMap_t> densityMap;
  while(reader_.NextParticle(particle, weight, ignoreTypes_))
  {
    RelocateParticle(particle, shiftX_, shiftY_);
	
    // Get list of tanks where the particle is within the sampling region 
    // If list is empty (no tank in range) discard the particle.
    I3TankResponseMapPtr respMap = responseService_->GetTankResponseMap(particle);
    if(!respMap) continue;
    
    // Loop over tanks
    I3TankResponseMap::const_iterator resp_iter;
    for(resp_iter=respMap->begin(); resp_iter!=respMap->end(); ++resp_iter)
    {
      const TankKey &tankKey = resp_iter->first;
      
      // Get tank dimensions
      const double tankRadius(resp_iter->second->GetVirtualTankRadius());
      const double tankHeight(resp_iter->second->GetVirtualTankHeight());
		
      double dist = DistanceToTank(particle, resp_iter->second);
      //dist = std::max(dist, tankRadius);
      
      // Disregard particle if it is outside of initial (maximum) resampling radius "dcheck_"
      if(dist > dcheck_) continue;
		
      // Disregard weighted particles with zenith > 88.9 deg
      // Because they are causing infinite tank shadow areas
      if(weight>1.0 && particle.GetZenith()>MAX_ZENITH)
      {
	log_warn("Discarding nearly horizontal (zenith = %.2f deg) particle (%s) with weight %.2f at Tank %s!",
		 particle.GetZenith()/I3Units::deg, particle.GetTypeString().c_str(), weight, tankKey.str().c_str());
	continue;
      }
      
      // Use distance binning to save memory and CPU
      unsigned int distBin = static_cast<unsigned int>(std::floor(dist/DIST_BIN_WIDTH));

      // Create or get density entry for TankKey at distBin 
      DensityEntry_t& densityEntry = (densityMap[tankKey])[distBin];
      densityEntry.value += weight*GetTankShadowArea(particle, tankRadius, tankHeight);
      densityEntry.count++;
      
      // FIXME
    log_warn("Check treatment of weighted particles which may hit more than one tank!");
      // Stop looping over tanks as soon as one tank was hit ???????????????????????????????????????????????????????
      // break;  
    }
  }

  // Rewind to beginning of event
  int checkEvtID;
  I3Particle tmpPrimary;
  reader_.Rewind();
  reader_.NextEvent(tmpPrimary, checkEvtID);
  if(checkEvtID!=currentEvtID_)
  {
    log_fatal("Failed to rewind event runID=%d, evtID=%d", currentRunID_, currentEvtID_);
  }
  
  // Calculate optimal distances to individual tanks
  optDistMap_.clear();
  std::map<TankKey, DensityMap_t>::const_iterator density_iter;
  for(density_iter = densityMap.begin(); density_iter != densityMap.end(); ++density_iter)
  {
    const TankKey& tankKey = density_iter->first;
    double doptimal = GetOptDistance(tankKey, density_iter->second);
    if(!finite(doptimal))
    {
      log_error("Invalid optimal distance (%f) of Tank %s. Skipping this tank!", doptimal, tankKey.str().c_str());
      continue;
    }
    
    optDistMap_[tankKey] = doptimal;
  }
}


double I3CorsikaInjector::GetOptDistance(const TankKey& tankKey, const DensityMap_t& dMap)
{
  I3TankResponsePtr tankResponse = responseService_->GetTankResponse(tankKey);
//   double coreX = relocationX_;
//   double coreY = relocationY_;
       
//   double distanceToCore = sqrt( pow(tankResponse->GetX() - coreX, 2) + 
// 				pow(tankResponse->GetY() - coreY, 2) );

  // This method determines the optimal resampling radius for a tank 
  
  // Total number of particles within dcheck_ from tank
  unsigned int counter = 0;
  DensityMap_t::const_iterator density_iter;
  for(density_iter = dMap.begin(); density_iter!=dMap.end(); ++density_iter)
  {
    counter += density_iter->second.count;
  }
    
  double radius = dcheck_;
  double sumD  = 0.0;
  
  unsigned int partW  = 0;  

  double closest = 99999999;
  
  // Loop over particle distances from tank
  // Distances are ordered here
  for(density_iter = dMap.begin(); density_iter!=dMap.end(); ++density_iter)
  {
    double dfound = (density_iter->first + 1)*DIST_BIN_WIDTH;
    if(dfound>dcheck_) dfound = dcheck_;
    
    sumD  += density_iter->second.value;
    partW += density_iter->second.count;
    
    double sumW = sumD/(I3Constants::pi*dfound*dfound); 

    double diff1 = fabs(partW - sumW); 
    double diff2 = fabs(partW - counter*(dfound*dfound)/(dcheck_*dcheck_)); 

    double percpart = ((double) partW)/((double)counter);

    if(diff1+diff2 <= closest && percpart >= 0.05 && dfound > 1.00)//phenomenologically derived
    { 
       radius = dfound; 

       closest = diff1+diff2; 
    }
  }

  return radius;
}


void I3CorsikaInjector::RelocateParticle(I3Particle& particle, const double& shiftX, const double& shiftY)
{
  // This function shifts the particle on the x-y plane
  particle.SetPos(shiftX + particle.GetX(),
		  shiftY + particle.GetY(),
		  particle.GetZ());
}


double I3CorsikaInjector::DistanceToTank(const I3Particle& particle, I3TankResponsePtr tankResponse)
{
  // This functions calculates the distance between the particle and the tank center
  // on the bottom plane of the tank.
  
  double shift = (tankResponse->GetZ() - 0.5*tankResponse->GetVirtualTankHeight() - particle.GetZ())/particle.GetDir().GetZ();
  
  double dx = particle.GetX() - tankResponse->GetX() + shift*particle.GetDir().GetX();
  double dy = particle.GetY() - tankResponse->GetY() + shift*particle.GetDir().GetY();
  
  return sqrt(dx*dx + dy*dy);
}


double I3CorsikaInjector::GetTankShadowArea(const I3Particle& particle, const double& tankRadius, const double& tankHeight)
{ 
  double nx = particle.GetDir().GetX();
  double ny = particle.GetDir().GetY();
  double tanTheta = -1.0*sqrt(nx*nx + ny*ny)/particle.GetDir().GetZ();  // Muliply with -1 because nz<0 for downgoing particle
  if(tanTheta<0) log_error("Negative tan(theta)!");
  
  return tankRadius*tankRadius*I3Constants::pi + 2.0*tankRadius*tankHeight*tanTheta;
}


void I3CorsikaInjector::ShiftToObsLevel(double& x, double& y, double& z, double& t, const I3Particle& particle)
{
  // CORSIKA observation level corresponds to z coordinate of particle
  
  // Shift along particle direction
  double shift = (particle.GetZ() - z)/particle.GetDir().GetZ();
  
  x += shift*particle.GetDir().GetX();
  y += shift*particle.GetDir().GetY();
  z  = particle.GetZ();
  t += shift/I3Constants::c;
}


bool I3CorsikaInjector::InsertHighPtMuon(I3Particle& particle)
{
  // This method randomly generates high pT muons according to the 
  // specified total and transverse momentum. The vertex is the
  // first interaction point of the shower
  
  // Exit here if desired muon number is reached
  if(hpTMuonCounter_ >= numHpTMuons_) return false;
  hpTMuonCounter_++;
    
  const I3Direction& primDir = primary_.GetDir();
  
  // Determine two directions perpendicular to primary direction
  double perpZen_1 = primDir.GetZenith()/I3Units::rad + 0.5*I3Constants::pi;
  double perpAzi_1 = primDir.GetAzimuth()/I3Units::rad;
  
  double perpX_1 = sin(perpZen_1)*cos(perpAzi_1);
  double perpY_1 = sin(perpZen_1)*sin(perpAzi_1);
  double perpZ_1 = cos(perpZen_1);
  
  double perpX_2 = perpY_1*primDir.GetZ() - perpZ_1*primDir.GetY();
  double perpY_2 = perpZ_1*primDir.GetX() - perpX_1*primDir.GetZ();
  double perpZ_2 = perpX_1*primDir.GetY() - perpY_1*primDir.GetX();
    
  // Dial random direction perpendicular to primary
  double phi = randomService_->Uniform(0, 2.0*I3Constants::pi);
    
  double perpX = cos(phi)*perpX_1 + sin(phi)*perpX_2;
  double perpY = cos(phi)*perpY_1 + sin(phi)*perpY_2;
  double perpZ = cos(phi)*perpZ_1 + sin(phi)*perpZ_2;
    
  double perpLen = hpTMuTransMomentum_/hpTMuTotalMomentum_;
  double primLen = sqrt(1.0 - perpLen*perpLen);
    
  double mu_nx = primLen*primDir.GetX() + perpLen*perpX;
  double mu_ny = primLen*primDir.GetY() + perpLen*perpY;
  double mu_nz = primLen*primDir.GetZ() + perpLen*perpZ;
  if(mu_nz>0) log_warn("Upward going high pT muon (nz = %.2f)!", mu_nz); 
    
  double normCheck = mu_nx*mu_nx + mu_ny*mu_ny + mu_nz*mu_nz;
  if(fabs(normCheck - 1.0) > 0.00001)
  {
    log_error("Direction of high pT muon not correctly normalized (norm=%.5f)!", normCheck);
    return false;
  }
  
  // First interaction point
  double x_start = primary_.GetX() - primary_.GetLength()*primary_.GetDir().GetX();
  double y_start = primary_.GetY() - primary_.GetLength()*primary_.GetDir().GetY();
  double z_start = primary_.GetZ() - primary_.GetLength()*primary_.GetDir().GetZ();
    
  // Calculate muon position on the observation level
  double mu_length = (primary_.GetZ() - z_start)/mu_nz;
    
  double mu_x = x_start + mu_length*mu_nx;
  double mu_y = y_start + mu_length*mu_ny;
  double mu_z = primary_.GetZ();  // Observation level
    
  // Set muon properties
  particle.SetType(I3Particle::MuMinus);
  particle.SetShape(I3Particle::TopShower);
  particle.SetPos(mu_x, mu_y, mu_z);
  particle.SetDir(mu_nx, mu_ny, mu_nz);
  particle.SetLength(mu_length);
  particle.SetEnergy(hpTMuTotalMomentum_); // Approximation for relativistic energies (as done in I3CorsikaReader)
  particle.SetSpeed(I3Constants::c);  // Approximately
  particle.SetTime(mu_length/I3Constants::c);
  
  return true;
}


I3FrameObjectConstPtr I3CorsikaInjector::GetEventInfo()
{
  I3CorsikaShowerInfoPtr showerInfo = reader_.GetShowerInfo();
  if (showerInfo)
  {
    showerInfo->crsSampleID = sampleIndex_;
    if (weightedRandomSampling_) {
      showerInfo->resampleRadius = samplingRegionSide_/sqrt(I3Constants::pi);
    }
    else {
      showerInfo->resampleRadius = relocationR_;
    }
    showerInfo->weight = samplingWeight_;
    showerInfo->nResampleNominal = nominalNumSamples_;
    showerInfo->nResample = numSamples_;
  }
  return boost::dynamic_pointer_cast<I3FrameObject>(showerInfo);
}


void I3CorsikaInjector::Finish()
{
}


TankKey
I3CorsikaInjector::GetTankKey(std::string key)
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


#ifdef I3_USE_ROOT
void
I3CorsikaInjector::ConvertToROOT(const SparseHistogram& sparse, TH2D* h)
{
  const int bins = sparse.GetBins();
  const double side = bins*sparse.GetBinWidth();
  h->Reset();
  h->ResetStats();
  h->SetBins(bins, -side/2, side/2, bins, -side/2, side/2);
  for (int i = 0; i != bins; ++i) {
    for (int j = 0; j != bins; ++j) {
      h->SetBinContent(i+1, j+1, sparse.At(i,j));
    }
  }
}
#endif

I3_SERVICE_FACTORY(I3InjectorFactory<I3CorsikaInjector>);
