/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3CorsikaThinnedInjector.cxx
 * @version $Rev: $
 * @date $Date: $
 * @authors Hans Dembinski
 */

#include <topsimulator/injectors/I3CorsikaThinnedInjector.h>
#include <topsimulator/interface/I3InjectorFactory.h>
#include <icetray/I3DefaultName.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/I3Constants.h>

#include <limits>
#include <cmath>
#include <boost/foreach.hpp>

namespace {
  static const double qNaN = std::numeric_limits<double>::quiet_NaN();

  void Relocate(I3Particle& p, double dx, double dy) {
    // shifts particle on the x-y plane
    p.SetPos(dx + p.GetX(), dy + p.GetY(), p.GetZ());
  }
}


I3CorsikaThinnedInjector::I3CorsikaThinnedInjector(const I3Context& context):
  I3InjectorService(context),
  unthinner_(4 * I3Units::km, 30, 12),
  shiftX_(0),
  shiftY_(0),
  currentRunID_(0),
  currentEvtID_(0),
  randomServiceName_(""),
  randomService_(I3RandomServicePtr())
{  
  AddParameter("FileNameList",
         "List of CORSIKA file names.",
         corsikaFiles_);

  AddParameter("IgnoreParticleTypes",
         "List of particle types to skip over (for instance, muons are 5 and 6)",
         ignoreTypes_);

  raiseObsLevel_ = 0.0;
  AddParameter("RaiseObservationLevel",
         "Adjust the z-coordinate of the observation level upward by this many "
         "meters. Only for correcting observation level problems after-the-fact. Default=0.",
         raiseObsLevel_);

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
         "Relocation radius in [m]. If zero, shower core(s) will be exactly at "
         "(relocationX, relocationY).",
         relocationR_);
  
  numSamples_ = 1;
  AddParameter("NumSamples",
         "Number of samples per shower.",
         numSamples_);

  randomServiceName_ = "";
  AddParameter("RandomServiceName",
         "Name of the random service in the context. This has to be specified or "
         "you'll get fatal error.",
         randomServiceName_);

  smartUnthinning_ = true;
  AddParameter("SmartUnthinning",
         "If true (default), particles with weight 1 are not unthinned and simply "
         "passed through. If false, all particles are unthinned, whatever their weight.",
         smartUnthinning_);
}


I3CorsikaThinnedInjector::~I3CorsikaThinnedInjector()
{}


void I3CorsikaThinnedInjector::Configure()
{
  log_info("Configuring the I3CorsikaThinnedInjector:");
  
  GetParameter("FileNameList", corsikaFiles_);
  reader_.SetFileList(corsikaFiles_); 

  GetParameter("IgnoreParticleTypes", ignoreTypes_);

  GetParameter("RaiseObservationLevel", raiseObsLevel_);

  GetParameter("RelocationX", relocationX_);
  relocationX_ *= I3Units::m;
    
  GetParameter("RelocationY", relocationY_);
  relocationY_ *= I3Units::m;
    
  GetParameter("RelocationR", relocationR_);
  relocationR_ *= I3Units::m;
  log_info("+ Relocation radius: %.2f m", relocationR_/I3Units::m);
  
  GetParameter("NumSamples", numSamples_);
  sampleIndex_ = numSamples_ - 1;

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

  GetParameter("SmartUnthinning", smartUnthinning_);
}


bool
I3CorsikaThinnedInjector::NextEvent(int& runID, int& evtID,
                                    I3Particle& primary,
                                    I3FrameConstPtr frame)
{
  sampleIndex_ = ++sampleIndex_ % numSamples_;

  if (sampleIndex_ == 0) {
    // load next shower from CORSIKA file,
    // collected particle statistics on new shower
    if (!reader_.NextEvent(primary, currentEvtID_))
    {
      if (!reader_.NextRun(currentRunID_))
        return false;
      // "NextRun" is where the z of the observation level gets computed, so 
      // here is where you should "hack the observation level"
      // (before reading in any particles)
      reader_.HackObsLevelUpwards(raiseObsLevel_);
      if (!reader_.NextEvent(primary, currentEvtID_))
        return false;
    }

    // initialize unthinner and collect particle statistics
    unthinner_.SetPrimary(primary);
    I3Particle p; double weight;
    while (reader_.NextParticle(p, weight, ignoreTypes_))
      unthinner_.AddParticle(p, weight);

    const double dr_safety = 1.0 * I3Units::m; // make configurable?
    const double dh_safety = 0.5 * I3Units::m; // make configurable?

    // (re)collect station positions
    const I3Geometry& geo = frame->Get<I3Geometry>(I3DefaultName<I3Geometry>::value());
    if (unthinner_.GetStations().size() != geo.stationgeo.size())
      unthinner_.SetStations(geo, dr_safety, dh_safety);
  }

  reader_.Rewind();
  reader_.NextEvent(primary, currentEvtID_);

  // Create eventID as a combination of the
  // CORSIKA eventID and the sampling index
  const int offset = std::pow(10, int(std::ceil(std::log10(numSamples_))));
  evtID = currentEvtID_ * offset + sampleIndex_;
  runID = currentRunID_;

  log_debug("Event %d, Run %d", evtID, runID);

  // Relocate the shower
  shiftX_ = relocationX_;
  shiftY_ = relocationY_;

  if (relocationR_ > 0.0)
  {
    const double phi    = randomService_->Uniform(0, 360 * I3Units::degree);
    const double radius = sqrt(randomService_->Uniform(0, relocationR_*relocationR_));
    shiftX_ += radius * cos(phi);
    shiftY_ += radius * sin(phi);
  }

  Relocate(primary, shiftX_, shiftY_);
  unthinner_.SetPrimaryPos(primary.GetPos());

  return true;
}


bool
I3CorsikaThinnedInjector::NextParticle(ExtendedI3Particle& particle)
{  
  while (clones_.empty()) {
    double weight;
    double hadgen;
    if (!reader_.NextParticle(particle, weight, hadgen, ignoreTypes_))
      return false;

    Relocate(particle, shiftX_, shiftY_);
    particle.SetAirShowerComponent(GetAirShowerComponent(particle, hadgen));
    if (smartUnthinning_ && weight == 1.0) {
      // skip unthinning and return original particle
      log_trace("returning original particle at (%f, %f)",
            particle.GetPos().GetX(), particle.GetPos().GetY());
      return true;
    }

    // make clones
    unthinner_.Sample(clones_, particle, weight, *randomService_);
  }

  particle = clones_.back();
  clones_.pop_back();
  log_trace("returning cloned particle at (%f, %f)",
            particle.GetPos().GetX(), particle.GetPos().GetY());

  return true;
}


std::map<std::string, int>
I3CorsikaThinnedInjector::GetAirShowerComponentNameMap() const
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
I3CorsikaThinnedInjector::GetAirShowerComponent(const I3Particle& p, double hadgen)
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


I3FrameObjectConstPtr I3CorsikaThinnedInjector::GetEventInfo()
{
  I3CorsikaShowerInfoPtr showerInfo = reader_.GetShowerInfo();
  if (showerInfo)
  {
    showerInfo->crsSampleID = sampleIndex_;
    showerInfo->resampleRadius = qNaN;
    showerInfo->weight = qNaN;
    showerInfo->nResampleNominal = numSamples_;
    showerInfo->nResample = numSamples_;
  }
  return boost::dynamic_pointer_cast<I3FrameObject>(showerInfo);
}

I3_SERVICE_FACTORY(I3InjectorFactory<I3CorsikaThinnedInjector>);
