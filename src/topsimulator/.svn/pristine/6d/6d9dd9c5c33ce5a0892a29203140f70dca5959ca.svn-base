/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3ParamTankResponse.cxx
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#include <topsimulator/interface/I3IceTopResponseFactory.h>
#include <topsimulator/responses/I3ParamTankResponse.h>
#include <topsimulator/responses/vem_yield.h>
#include <topsimulator/GeoFunctions.h>
//#include <pmt-simulator/I3PMTConstants.h>
#include <dataclasses/I3Constants.h>

#include <phys-services/I3GSLRandomService.h>

const double I3ParamTankResponse::MEAN_NORMALIZED_PE = 0.850529;

const double I3ParamTankResponse::PERL_SNOW_EQUIV = 0.3;

I3ParamTankResponse::I3ParamTankResponse(I3Configuration& config, const I3Context& context, const TankKey& tankKey): I3TankResponse(config, context, tankKey)
{
  tauZirco_ = (7.94*I3Units::m/I3Constants::c)/I3Units::ns;
  AddParameter("TimeConstantZirco","Time constant in [ns] for tanks with zirconium coating.", tauZirco_);
  
  tauTyvek_ = (12.6*I3Units::m/I3Constants::c)/I3Units::ns;
  AddParameter("TimeConstantTyvek","Time constant in [ns] for tanks with a Tyvek liner.", tauTyvek_);

  useSnowPar_ = true;
  AddParameter("UseSnowParam",
	       "If True use Patricks's parametrization (only for e+/-, mu+/- and gammas) to simulate the snow. "
	       "Otherwise use Stefan's parametrization without the snow effect.",
	       useSnowPar_);
  
  randomServiceName_.clear();
  AddParameter("RandomServiceName","Name of external random service.", randomServiceName_);
}


void I3ParamTankResponse::Configure()
{
  log_info("Configuring Tank Response:");
  
  GetParameter("TimeConstantZirco", tauZirco_);
  tauZirco_*=I3Units::ns;
  log_info("+ Time Constant (Zirconium): %.1f ns", tauZirco_/I3Units::ns);

  GetParameter("TimeConstantTyvek", tauTyvek_);
  tauTyvek_*=I3Units::ns;
  log_info("+ Time Constant (Tyvek)    : %.1f ns", tauTyvek_/I3Units::ns);
  
  GetParameter("UseSnowParam", useSnowPar_);
  log_info("+ VEM yield parametrization: %s", (useSnowPar_?"WITH snow (default)":"WITHOUT snow"));

  // Look for a random generator service
  GetParameter("RandomServiceName", randomServiceName_);
  if(randomServiceName_.empty()) randomService_ = I3RandomServicePtr(new I3GSLRandomService(0));
  else                           randomService_ = GetContext().Get<I3RandomServicePtr>(randomServiceName_);
  if(!randomService_) log_fatal("Missing random service!");
  
  if(randomServiceName_.empty()) log_info("+ Random service: I3GSLRandomService  (default)");
  else log_info("+ Random service: %s  (EXTERNAL)",  randomServiceName_.c_str());
}


void I3ParamTankResponse::Initialize(const I3Geometry& geometry,
				     const I3Calibration& calib,
				     const I3DetectorStatus& status)
{
  // Get the stations geometry
  const I3StationGeoMap& stationMap = geometry.stationgeo;
  
  // Get the dom status
  const std::map<OMKey, I3DOMStatus>& domStatusMap = status.domStatus;
  
  // Get the VEM calibration 
  const std::map<OMKey, I3VEMCalibration>& vemCalMap = calib.vemCal;
  
  I3StationGeoMap::const_iterator station_iter = stationMap.find(tankKey_.string);
  if(station_iter==stationMap.end())
  {
    log_fatal("Station %d doesn't exist in geometry!", tankKey_.string);
    return;
  }
    
  unsigned int tankID = tankKey_.tank==TankKey::TankA?0:1;
  
  const I3TankGeo& tankGeo = station_iter->second.at(tankID);
    
  // Get tank dimensions
  tankRadius_ = tankGeo.tankradius;
  tankHeight_ = tankGeo.tankheight;
  
  // Get fill and snow heights
  fillHeight_ = tankGeo.fillheight;
  snowHeight_ = tankGeo.snowheight;
  perlHeight_ = tankHeight_ - fillHeight_;
    
  // Calculate tank position (tank center)
  // tankGeo.position corresponds to the average position
  // of the two DOMs in a tank
  posX_ = tankGeo.position.GetX();
  posY_ = tankGeo.position.GetY();
  posZ_ = tankGeo.position.GetZ() - fillHeight_ + 0.5*tankHeight_; // tankGeo.position.GetZ() is at the top of the ice block in the tank
  
  // Set decay time for specific tank coating
  switch(tankGeo.tanktype)
  {
     case I3TankGeo::Tyvek_Lined:
       decayTime_ = tauTyvek_;
       break;
     case I3TankGeo::Zirconium_Lined:
       decayTime_ = tauZirco_;
       break;
     default:
       log_fatal("Unknown type of tank %s! Is your GCD-file up-to-date?", tankKey_.str().c_str());
       return;
  };
    
  // Loop over all DOMs in the tank
  pePerVEM_.clear();
  I3Vector<OMKey>::const_iterator dom_iter;
  for(dom_iter=tankGeo.omKeyList_.begin(); dom_iter!=tankGeo.omKeyList_.end(); dom_iter++)
  {
    // Get the DOMStatus of this DOM to see if it is in the configuration
    std::map<OMKey, I3DOMStatus>::const_iterator status_iter = domStatusMap.find(*dom_iter);
    if(status_iter==domStatusMap.end()) continue;
    
    // Check if PMT is powered up
    if(!(status_iter->second.pmtHV>0))
    {
      log_warn("HV of %s is down!", dom_iter->str().c_str());
    }
    
    // Get the VEM calibration
    std::map<OMKey, I3VEMCalibration>::const_iterator vem_iter = vemCalMap.find(*dom_iter);
    if(vem_iter==vemCalMap.end())
    {
      log_warn("Missing VEM calibration of module %s. Skipping it!", dom_iter->str().c_str());
      continue;
    }
    
    // Fill the map of pePerVEM values for each DOM
    // The actual pePerVEM value is scaled down by the mean PE
    // charge (0.85) to account for the asymmetric single photoelectron
    // distribution --> This only works together with the pmt-simulator
    // option "TreatIceTopAsInIce"=True!!!!
    //pePerVEM_[*dom_iter] = (vem_iter->second.pePerVEM/vem_iter->second.corrFactor)/I3PMTConstants::MEAN_NORMALIZED_PE;
    pePerVEM_[*dom_iter] = (vem_iter->second.pePerVEM/vem_iter->second.corrFactor)/MEAN_NORMALIZED_PE;
  }
}


bool I3ParamTankResponse::TrackParticle(const ExtendedI3Particle& particle, HitHistoCollection& hitHC, HitHistoCollection& cherHitCollection)
{
  bool hitTank = false;
  ExtendedI3Particle iceTrack(particle); // copy constructor preserves ParticleID
  double zIceCenter = posZ_ + 0.5*(fillHeight_ - tankHeight_);
  if(GeoFunctions::IntersectCylinder(posX_, posY_, zIceCenter, fillHeight_, tankRadius_, particle, iceTrack))
  {
    hitTank = TankResponse(iceTrack, hitHC, cherHitCollection);
  }
  
  return hitTank;
}


bool I3ParamTankResponse::TankResponse(const ExtendedI3Particle& iceTrack, HitHistoCollection& hitHC, HitHistoCollection& cherHitCollection)
{
  // Calculate the snow surface in relative coordinates
  // with respect to the center of the ice block
  double zSurface = 0.5*fillHeight_ + perlHeight_ + snowHeight_;
    
  // Calculate track length trough snow (including perlite)
  double snowLength = (iceTrack.GetZ() - zSurface)/iceTrack.GetDir().GetZ();
  
  if(iceTrack.GetType() != I3Particle::MuMinus &&
     iceTrack.GetType() != I3Particle::MuPlus  &&
     snowLength > 10.0*I3Units::m)
  {
    // Discard particle if it is not a muon and has to pass more
    // than 10 m of material (snow + perlite) before it reaches the ice block in the tank.
    // Ideally one would use energy dependent range tables for each particle type.
    return false;
  }
  
  if(useSnowPar_)
  {
    // Snow lentgh contains perlite length.
    // --> Correct snow length for lower perlite density 
    
    I3Particle perlTrack;
    double zPerlCenter = 0.5*(fillHeight_ + perlHeight_); // Relative to center of ice block !!!
    
    // Perlite coordinates relative to center of ice block !!!
    if(GeoFunctions::IntersectCylinder(0, 0, zPerlCenter, perlHeight_, tankRadius_, iceTrack, perlTrack))
    {
      snowLength += (PERL_SNOW_EQUIV - 1.0)*perlTrack.GetLength();
    }
  }
    
  // Determine the mean expected VEM value for various particle types
  double vem_mean=0;
  switch(iceTrack.GetType())
  {
     case I3Particle::EPlus:
       if(useSnowPar_) vem_mean = vem_yield::em_snow(iceTrack.GetEnergy()/I3Units::GeV, snowLength/I3Units::m); // Using parametrization for electrons with snow!
       else            vem_mean = vem_yield::ep(iceTrack.GetEnergy()/I3Units::GeV); 
       break;
     case I3Particle::EMinus: 
       if(useSnowPar_) vem_mean = vem_yield::em_snow(iceTrack.GetEnergy()/I3Units::GeV, snowLength/I3Units::m);
       else            vem_mean = vem_yield::em(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::Gamma:
       if(useSnowPar_) vem_mean = vem_yield::ga_snow(iceTrack.GetEnergy()/I3Units::GeV, snowLength/I3Units::m);
       else            vem_mean = vem_yield::ga(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::MuPlus:
       if(useSnowPar_) vem_mean = vem_yield::mm_snow(iceTrack.GetEnergy()/I3Units::GeV, snowLength/I3Units::m); // Using parametrization for muons with snow!
       else            vem_mean = vem_yield::mp(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::MuMinus:
       if(useSnowPar_) vem_mean = vem_yield::mm_snow(iceTrack.GetEnergy()/I3Units::GeV, snowLength/I3Units::m);
       else            vem_mean = vem_yield::mm(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::PPlus:
       vem_mean = vem_yield::p(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::PMinus:
       vem_mean = vem_yield::ap(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::Neutron:
       vem_mean = vem_yield::n(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::NeutronBar:
       vem_mean = vem_yield::an(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::PiPlus:
       vem_mean = vem_yield::pp(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::PiMinus:
       vem_mean = vem_yield::pm(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::KPlus:
       vem_mean = vem_yield::kp(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::KMinus:
       vem_mean = vem_yield::km(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     case I3Particle::K0_Long:
     case I3Particle::K0_Short:
       vem_mean = vem_yield::k0(iceTrack.GetEnergy()/I3Units::GeV);
       break;
     default:
       log_warn("Man, check out that strange particle \"%s\" ?!", iceTrack.GetTypeString().c_str());
  }
  
  // Exit here if there is no signal
  if(vem_mean<=0) return false;
    
  // Scale signal relative to track length
  vem_mean *= iceTrack.GetLength()/fillHeight_;
  
  // Loop over all active DOMs in tank
  std::map<OMKey, double>::const_iterator pePerVEM_iter;
  for(pePerVEM_iter=pePerVEM_.begin(); pePerVEM_iter!=pePerVEM_.end(); ++pePerVEM_iter)
  {
    // Calculate mean number of photoelectons
    // according to actual VEM calibration
    double npe_mean = vem_mean*pePerVEM_iter->second;
    
    // Dial npe according to Poisson distribution
    int num_hits = randomService_->Poisson(npe_mean);
	
    //log_trace("%s: Generating %d NPEs from %.3f VEM (%s, energy=%.3f MeV, length = %.2f m)",
    //	  pePerVEM_iter->first.str().c_str(), npe, vem_mean, iceTrack.GetTypeString().c_str(),
    //	  iceTrack.GetEnergy()/I3Units::MeV, iceTrack.GetLength()/I3Units::m); 
    
    // Generate hits
    GenerateHits(num_hits, iceTrack, hitHC.GetHitHisto(pePerVEM_iter->first));
  }
  
  return true;
}


void I3ParamTankResponse::GenerateHits(int npe, const ExtendedI3Particle& p,
                                       HitHisto& hitHisto)
{
  if (npe == 0)
    return;

  // draw individual npe with time delay from exponential distribution
  const double t0 = p.GetStartTime();

  if (npe == 1) {
    // no hokus pokus for single pe
    hitHisto.Fill(t0 + randomService_->Exp(decayTime_), 1, p);
  } else {
    // fill times into a vector and pass that to HitHisto
    std::vector<double> times(npe);
    for(int j = 0; j < npe; ++j)
      times[j] = t0 + randomService_->Exp(decayTime_);
    hitHisto.Fill(times, p);
  }
}


I3_SERVICE_FACTORY(I3IceTopResponseFactory<I3ParamTankResponse>);
