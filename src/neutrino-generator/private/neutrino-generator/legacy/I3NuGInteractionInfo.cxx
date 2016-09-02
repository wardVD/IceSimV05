/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file I3NuGInteractionInfo.cxx
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief I3NuGInteractionInfo implementaion file
 *   This class accesss to the classes containing 
 *   all active interaction channels 
 */
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "neutrino-generator/interactions/InteractionBase.h"
#include "neutrino-generator/interactions/InteractionCC.h"
#include "neutrino-generator/interactions/InteractionNC.h"
#include "neutrino-generator/interactions/InteractionGR.h"
#include "neutrino-generator/interactions/TauDecay.h"
#include "neutrino-generator/legacy/StaticParams.h"
#include "neutrino-generator/utils/Constants.h"
#include "neutrino-generator/utils/Calculator.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"
#include "neutrino-generator/table-interface/TableUtils.h"
#include "phys-services/I3RandomService.h"
#include "icetray/I3SingleServiceFactory.h"
#include "dataclasses/I3Constants.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace nugen;
using namespace earthmodel;

//_____________________________________________________________
I3NuGInteractionInfo::PrevStateColumnDepth::PrevStateColumnDepth(
           const I3Position &entCE,
           const I3Position &extCE,
           int    nsteps,
           double maxstep,
           ParticlePtr p,
           double ccf,
           double ncf,
           double grf)
        :  enterCE_(entCE),
           exitCE_(extCE),
           nsteps_(nsteps),
           maxstepsize_(maxstep),
           particleptr_(p), 
           ccfactor_(ccf),
           ncfactor_(ncf),
           grfactor_(grf)
{
  if (particleptr_) {
     // save current energy!
     energy_ = particleptr_->GetEnergy();
  } else {
     energy_ = -1;
  }
}

//_____________________________________________________________
I3NuGInteractionInfo::I3NuGInteractionInfo(const I3Context &c):
  I3ServiceBase(c),
  steeringName_(""),
  tablesDir_(""),
  xsec_model_("csms"),
  flavorMaskCCNu_(101010), 
  materialMaskCCNu_(110),
  flavorMaskCCNuBar_(10101), 
  materialMaskCCNuBar_(110),
  flavorMaskNCNu_(101010), 
  materialMaskNCNu_(110),
  flavorMaskNCNuBar_(10101), 
  materialMaskNCNuBar_(110),
  flavorMaskGRNuBar_(10000), 
  materialMaskGRNuBar_(1),
  protonrock_(0.5),  
  protonh2o_(0.5),
  neutronrock_(0.5),  
  neutronh2o_(0.5),
  electronrock_(0.5),  
  electronh2o_(0.55555)
{
  log_trace("construct I3NuGInteractionInfo");

  AddParameter("RandomService","pointer to random service",random_);
  AddParameter("SteeringName","name of Steering ",steeringName_);

  AddParameter("TablesDir","Directory that contains the cross section tables",tablesDir_);

  AddParameter("CrossSectionModel",  "select from csms, css, cteq5",  xsec_model_);

  interactionVect_.clear();

}

//_____________________________________________________________
I3NuGInteractionInfo::I3NuGInteractionInfo(
                       I3RandomServicePtr random, 
                       SteeringPtr steer,
                       const string &xsec_model):
  I3ServiceBase("I3NuGInteractionInfo"),
  random_(random), 
  steeringName_("dummy"),
  steer_(steer),
  tablesDir_(""),
  xsec_model_(xsec_model),
  flavorMaskCCNu_(101010), 
  materialMaskCCNu_(110),
  flavorMaskCCNuBar_(10101), 
  materialMaskCCNuBar_(110),
  flavorMaskNCNu_(101010), 
  materialMaskNCNu_(110),
  flavorMaskNCNuBar_(10101), 
  materialMaskNCNuBar_(110),
  flavorMaskGRNuBar_(10000), 
  materialMaskGRNuBar_(1),
  protonrock_(0.5),  
  protonh2o_(0.5),
  neutronrock_(0.5),  
  neutronh2o_(0.5),
  electronrock_(0.5),  
  electronh2o_(0.55555)
{
  log_trace("construct I3NuGInteractionInfo");

  interactionVect_.clear();
}

//_____________________________________________________________
I3NuGInteractionInfo::~I3NuGInteractionInfo()
{
  //destructor is responsible for deleting the partial cross sections
}

//_____________________________________________________________
void I3NuGInteractionInfo::Configure()
{
  GetParameter("RandomService",random_);
  GetParameter("SteeringName",steeringName_);
  GetParameter("TablesDir",tablesDir_);
  GetParameter("CrossSectionModel",   xsec_model_);

  //ViewInteractionInfo();

  if (!random_) {
     random_ = context_.Get<I3RandomServicePtr>();
  }

  if (steeringName_ != "") {
     if (context_.Has(steeringName_)) {
        steer_ = context_.Get<SteeringPtr>(steeringName_);
     } else {
        log_error("You have to add I3NuGSteeringService to tray "
                  "before I3NuGInteractionInfo");
     }
  } else {
     log_error("You have to set SteeringServiceName!");
  }

  Initialize();

}

//_____________________________________________________________
void I3NuGInteractionInfo::Initialize()
{
  earth_ = steer_->GetEarthModelService();

  // read cross section list
  TableUtils::ReadTableList(tablesDir_, xsec_model_, xsecs_);

  ReadInteractionFiles();
  ReadDecayStateFiles();

  InteractionTypeDoubleMap scales = steer_->GetGlobalXsecScales();
  global_cc_scales_ = scales[CC];
  global_nc_scales_ = scales[NC];
  global_gr_scales_ = scales[GR];
}

//_____________________________________________________________
void I3NuGInteractionInfo::ReadInteractionFiles()
{
  interactionVect_.clear();

  log_debug("Read neutrino interaction files");
  InteractionCCPtr InteractionCCNu_ptr(new InteractionCC(random_, steer_));
  InteractionNCPtr InteractionNCNu_ptr(new InteractionNC(random_, steer_));

  InteractionCCNu_ptr->InitializeCrosssectionTable(xsecs_["CC_Nu_iso_xsec"]);
  InteractionNCNu_ptr->InitializeCrosssectionTable(xsecs_["NC_Nu_iso_xsec"]);

  InteractionCCNu_ptr->SetActiveFlavorMask(flavorMaskCCNu_);
  InteractionNCNu_ptr->SetActiveFlavorMask(flavorMaskNCNu_);

  InteractionCCNu_ptr->SetActiveMaterialMask(materialMaskCCNu_);
  InteractionNCNu_ptr->SetActiveMaterialMask(materialMaskNCNu_);

  InteractionCCNu_ptr->InitializeFinalStateTable(xsecs_["CC_Nu_iso_final"]);
  InteractionNCNu_ptr->InitializeFinalStateTable(xsecs_["NC_Nu_iso_final"]);

  interactionVect_.push_back(InteractionCCNu_ptr);
  interactionVect_.push_back(InteractionNCNu_ptr);
  interactionMap_["CC_Nu_iso"] = InteractionCCNu_ptr;
  interactionMap_["NC_Nu_iso"] = InteractionNCNu_ptr;

  log_debug("Read anti-neutrino interaction files");
  InteractionCCPtr InteractionCCNuBar_ptr(new InteractionCC(random_, steer_));
  InteractionNCPtr InteractionNCNuBar_ptr(new InteractionNC(random_, steer_));
  InteractionGRPtr InteractionGRNuBar_ptr(new InteractionGR(random_, steer_));
    
  InteractionCCNuBar_ptr->InitializeCrosssectionTable(xsecs_["CC_NuBar_iso_xsec"]);
  InteractionNCNuBar_ptr->InitializeCrosssectionTable(xsecs_["NC_NuBar_iso_xsec"]);

  //You don't need to initialize GR crosssection file 'cause it's dummy
  //InteractionGRNuBar_ptr->InitializeCrosssectionTable(GetGRNuBarCrosssectionFileName());    

  InteractionCCNuBar_ptr->SetActiveFlavorMask(flavorMaskCCNuBar_);
  InteractionNCNuBar_ptr->SetActiveFlavorMask(flavorMaskNCNuBar_);
  InteractionGRNuBar_ptr->SetActiveFlavorMask(flavorMaskGRNuBar_);

  InteractionCCNuBar_ptr->SetActiveMaterialMask(materialMaskCCNuBar_);
  InteractionNCNuBar_ptr->SetActiveMaterialMask(materialMaskNCNuBar_);
  InteractionGRNuBar_ptr->SetActiveMaterialMask(materialMaskGRNuBar_);

  InteractionCCNuBar_ptr->InitializeFinalStateTable(xsecs_["CC_NuBar_iso_final"]);
  InteractionNCNuBar_ptr->InitializeFinalStateTable(xsecs_["NC_NuBar_iso_final"]);

  //You don't need to initialize GR crosssection file 'cause it's dummy
  //InteractionGRNuBar_ptr->InitializeFinalStateTable(GetGRNuBarFinalStateFileName());

  interactionVect_.push_back(InteractionGRNuBar_ptr);
  interactionVect_.push_back(InteractionCCNuBar_ptr);
  interactionVect_.push_back(InteractionNCNuBar_ptr);
  interactionMap_["CC_NuBar_iso"] = InteractionCCNuBar_ptr;
  interactionMap_["NC_NuBar_iso"] = InteractionNCNuBar_ptr;
  interactionMap_["GR_NuBar"] = InteractionGRNuBar_ptr;

  vector<InteractionBasePtr>::const_iterator iter = interactionVect_.begin();
  for(; iter != interactionVect_.end(); ++iter){
    InteractionBasePtr int_ptr = *iter;    
    if(int_ptr)log_trace("Cross-section file: %s", int_ptr->GetCrosssectionFileName().c_str());
  }

  //----------------------------------------------------
  // initialize flag for GetTotalCrossSection()
  prev_state_crosssec_.use_prev_interaction_vect_ = false;
  //----------------------------------------------------

  log_trace("Interaction files are read");
}

//_____________________________________________________________
void I3NuGInteractionInfo::ReadDecayStateFiles()
{

  TauDecayPtr decay_ptr(new TauDecay(random_, steer_));

  decay_ptr->InitializeFinalStateTable(xsecs_["Tau_Decay"]);
  SetTauDecayPtr(decay_ptr);
  log_trace("Decay files are read");
}

//_____________________________________________________________
bool I3NuGInteractionInfo::MakeDecay(ParticlePtr tau_ptr)
{
  log_debug("In MakeTauDecay");
  // this is going to be true except the case that tau life time is chosen to be long enough to loose
  // all of their energy
  bool decay_made = false;


  double initial_energy = tau_ptr->GetEnergy()/I3Units::GeV;//[GeV]

  I3Position positionDC(tau_ptr->GetPos());
  I3Position positionCE(earth_->GetEarthCoordPosFromDetCoordPos(positionDC));
  I3Direction directionDC(tau_ptr->GetDir());
  I3Direction directionCE(earth_->GetEarthCoordDirFromDetCoordDir(directionDC));
  
  log_debug("start position CE (%f,%f,%f)", positionCE.GetX(), positionCE.GetY(), positionCE.GetZ());
  log_debug("direction CE (%f/pi,%f/pi)", directionCE.CalcTheta()/I3Constants::pi, directionCE.CalcPhi()/I3Constants::pi);

  //TAU_CTE=4.9021097E-5=c*\tau_\tau /m_\tau in m/GeV
  //[(c * tau_tau)/ m_tau] in m/GeV
  //lifetime returns tau lifetime *c/m_tau
  double rand = random_->Uniform(0.0, 1.0);
  double lifetime = - Constants::TAU_CTE*log(rand);//=lifetime*(c/m_tau=3e8/1.77[m/(s*GeV)])[m/GeV]  
  lifetime *=  Constants::M_TAU / Constants::C_LIGHT;//in [second] in co-moving frame

  //double lifetime = - Constants::TAU_CTE / Constants::C_LIGHT * Constants::M_TAU*log(random_->Uniform(0.0, 1.0));//[s] 
  double initial_range = lifetime * Constants::C_LIGHT * initial_energy / Constants::M_TAU ;

  //now we transfer this to distance in lab-frame
  log_debug("initial energy %f [GeV], in log(e/GeV)=%f", initial_energy, log10(initial_energy));
  log_debug("lifetime of %f[ps]", lifetime*1e12);
  log_debug("initial range without energy loss %f[m]", initial_range);

  // this is Aya's original
  int timeStepN = 10000;
  if     (initial_range>1e5)timeStepN=1000;//100km -> initial delta d >100m
  else if(initial_range>1e6)timeStepN=10000;
  else if(initial_range>1e7)timeStepN=100000;
  else if(initial_range>1e8)timeStepN=1000000;

  double delta_time = lifetime/timeStepN;

  /*
  // ... but the initial_range could be less than 1m (because we use random number!)
  // let's set minimum step len. Since NuGen doesn't manage final tau decay, 
  // min step len 100m would be enough.

  if (initial_range > 100.*I3Units::m) {
     timeStepN = int(initial_range / 100.*I3Units::m);
     delta_time = lifetime / timeStepN;
  } else if (initial_range > 10.*I3Units::m) {
     timeStepN = int(initial_range / 10.*I3Units::m); // min step 10m
     delta_time = lifetime / timeStepN;
  } else {
     timeStepN = 0;
     delta_time = 0;
  }
  */

  //if lifetime is 1[ps], typical propagation length (at log(E)=10) is 
  //1300[km]
  double current_energy = initial_energy;
  double distance=0;
  I3Position local(positionCE);

  //#define C_LIGHT     2.9979246e8   // m/s
  //#define TAU_CTE     4.9021097E-5  // c*\tau_\tau/m_\tau  in m/GeV
  //#define M_TAU       1.777         // tau lepton mass in GeV
  //cout << "M_TAU" << M_TAU << " C_LIGHT " << C_LIGHT << " TAU_CTE " << TAU_CTE << endl;
  for (int i=0; i<timeStepN; ++i) {
    
    double densityCGS = earth_->GetEarthDensityInCGS(local); // g/cm3
    double delta_d = delta_time* Constants::C_LIGHT * (current_energy/ Constants::M_TAU);//distance while delta time step size [m]
    //energy loss in this step [GeV] -- get tau beta in cm2/g * earth density in g/cm3 * 100 -> (1/m)
    double delta_e = current_energy*delta_d*GetTauBeta(current_energy)* densityCGS * 100;//(1/m)*(GeV)*m=GeV
    //log_trace("GetTauBeta %fx10^6",GetTauBeta(current_energy)*1000000);
    //log_trace("delta e %f, current e %f, delta d %f, earth %f",delta_e, current_energy, delta_d, GetEarthDensity(local));
    
    distance += delta_d;//[m] 
    
    if (delta_e>current_energy) {
       log_trace("tau attenuated during propagation");
       current_energy=0.0;
       break;
    }
    current_energy -= delta_e;//[GeV]
    //log_debug("dis step %f [m], delta e %f [GeV], current energy log %f", delta_d, delta_e, log10(current_energy));
    
    local += delta_d*directionCE;
    //log_debug("current position CE (%f,%f,%f), R=%f[m]", local.GetX(), local.GetY(), local.GetZ(), local.Magnitude());
  }
     
  log_debug("initial energy %f [GeV], tau lost %f [GeV], current E %f [GeV]", 
	    initial_energy, initial_energy - current_energy, current_energy);
  log_debug("initial energy log(initial E [GeV])=%f,  log(current E [GeV])=%f after propagation", 
	    log10(initial_energy), log10(current_energy));
  log_debug("Tau propagated %f [m] in lifetime of %f[ps] if no energy loss %f [m]", distance, lifetime*1e12, initial_range);
  I3Position decaypos = positionCE;
  tau_ptr->SetLength(distance*I3Units::m);
  tau_ptr->GetInteractionInfoRef().SetInteractionType(DECAY);
  tau_ptr->GetInteractionInfoRef().SetEnergyLost(initial_energy - current_energy);

  //now this position + directionCE*distance is the decay position, 
  //current_energy corresponds to that at the decay position
  if (current_energy) {
    decay_made=true;
    log_debug("        # daughter %d particles, before decay", tau_ptr->GetTotalNDaughters());
    TauDecayPtr decay_ptr = GetTauDecayPtr();
    log_trace("Will check Decay file: %s", decay_ptr->GetTauDecayFileName().c_str());
    decay_ptr->FillDaughterParticles(tau_ptr,current_energy); 
    log_debug("        # daughter list after decay %d particles", tau_ptr->GetTotalNDaughters());
  }
  
  tau_ptr->CheckParticle();
  tau_ptr->CheckDaughters();
  
  return decay_made;
}

//______________________________________________________________________
double I3NuGInteractionInfo::GetXsecCGS(std::string inttype,
                               double energy)
{
   if (!interactionMap_.count(inttype)) {
      log_error("interaction type %s does not exist", inttype.c_str());
   }
   InteractionBasePtr intptr = interactionMap_[inttype];
   return intptr->GetXsecCGS(energy);
}

//______________________________________________________________________
double I3NuGInteractionInfo::GetTotalCrosssection(ParticlePtr nuin_ptr) 
{
//
// use the public function for standard usage.
//
   return GetTotalCrosssection(nuin_ptr, 1.0, 1.0, 1.0);
}

//______________________________________________________________________
double I3NuGInteractionInfo::GetTotalCrosssection(ParticlePtr nuin_ptr, 
                             const I3Position & positionInCE)
{
//
// use the public function for standard usage.
//
   return GetTotalCrosssection(nuin_ptr, positionInCE, 
                               1.0, 1.0, 1.0);
}

//______________________________________________________________________
double I3NuGInteractionInfo::GetTotalCrosssection(ParticlePtr nuin_ptr,
                             double ccfactor, double ncfactor, double grfactor,
                             double *ccweight, double *ncweight, double *grweight)
{
//
// return total cross section
//

  I3Position positionDC(nuin_ptr->GetPos());
  I3Position positionCE(earth_->GetEarthCoordPosFromDetCoordPos(positionDC));
  //log_debug("start position to determine material consistute CE (%.2f, %.2f, %.2f), r=%.2f [m]",
  //	   	                  positionCE.GetX(), positionCE.GetY(), positionCE.GetZ(), positionCE.Magnitude());
  return GetTotalCrosssection(nuin_ptr, positionCE, 
                              ccfactor, ncfactor, grfactor,
                              ccweight, ncweight, grweight);
}

//______________________________________________________________________
double I3NuGInteractionInfo::GetTotalCrosssection(ParticlePtr nuin_ptr, 
                             const I3Position & positionInCE,
                             double ccfactor, double ncfactor, double grfactor,
                             double *ccweight, double *ncweight, double *grweight)
{  
//
// Returns total cross section at a given point for the given particle.
// If nothing happens, it returns 0.
//
// ----------------------------------------------------
// ccfactor, ncfactor and grfactor are used for weighted 
// interaction.
// These options should be use by MakeInteraction function only.
// To get total cross section in order to calculate interaction 
// probability etc. you must leave these factors as default.
//
// If you want to scale cross sections, set
// StaticParams::gCC_SCALE_FACTOR etc.
// These global scale parameters affects both propagation and
// interaction linearly, while weighting parameter ccfactor etc.
// are used for weighted interaction only.
// 
// ccfactor, ncfactor, and grfactor gives scaling ratio for each
// interaction.
// Set <=0 to deactivate them. Default: all 1. 
// If all factors are 1, weights are always 1.
//
// If you set ccfactor, ncfactor and grfactor with real numbers
// except for 1.0, it calculates ccweight, ncweight, grweight 
// as scaling weight.
// negative weight value represents N/A.
//
// [example case 1] (Change ratio) 
// ccfactor = 1, ncfactor = 2, grfactor = 1
// if cc=0.6, nc=0.3, gr=0.1, total 1.0
//  => cc'=0.6, nc'=0.6, gr'=0.1, total 1.3
//  => cc'=0.46, nc'=0.46, gr'=0.077, total ~ 1.0
// ccweight = 0.6/0.46 ~ 1.3 
// ncweight = 0.3/0.46 ~ 0.65
// grweight = 0.1/0.077 ~ 1.3
//
// [example case 2] (NC+GR mode for NuE)
// ccfactor = 0, ncfactor = 1, grfactor = 1
// if cc=0.6, nc=0.3, gr=0.1, total 1.0
//  => cc'=0, nc'=0.3, gr'=0.1, total 0.4
//  => cc'=0, nc'=0.75, gr'=0.25, total ~ 1.0
// ccweight = -1 (N/A)
// ncweight = 0.3/0.75 ~ 0.4
// grweight = 0.1/0.25 ~ 0.4
//
// [example case 3] (NC+GR mode for NuMu)
// ccfactor = 0, ncfactor = 1, grfactor = 1
// if cc=0.6, nc=0.3, gr=0.0, total 0.9
//  => cc =0.66... nc=0.33... gr=0.0, total ~ 1.0
//  => cc'=0, nc'=0.3, gr'=0, total 0.3
//  => cc'=0, nc'=1.0, gr'=0, total 1.0
// ccweight = -1 (N/A)
// ncweight = 0.333../1.0 = 0.333...
// grweight = -1 (N/A)
//
  //log_trace("GetTotalCrosssection called");

  if (ccfactor < 0 || ncfactor < 0 || grfactor < 0) {
     log_fatal("Does not support negative factor! ccfactor = %g, "
               "ncfactor = %g, grfactor = %g", ccfactor, ncfactor,
               grfactor);
  }


  //
  // check material ICE or ROCK
  // Now we may define airly thin density for rock beneath ice layer.
  // medium parameter must be defined purely from geometory
  // instead of refering ice density.
  //
  EarthModelService::MediumType mediumtype = 
               earth_->GetMedium(positionInCE);

  NuGPropagatingMedium medium = I3NuGInteractionInfo::ICE;

  if (mediumtype != EarthModelService::ICE) { 
     // it may be rock, air, or vacuum.
	  // In any case number of neutron and proton
	  // are same, then we use rock parameter.
     medium = I3NuGInteractionInfo::ROCK;
  }
  I3Particle::ParticleType ptype = nuin_ptr->GetType();
  double energy = nuin_ptr->GetEnergy();

  //
  // then compare arguments with previous setting
  // if they match, just return previous result!
  //  

  /*
  log_trace("GetTotalCrosssection is called with medium %d "
            "ptype %d energy %f ccfactor %d ncfactor %d grfactor %d", 
            int(medium), int(ptype), energy,
            ccfactor, ncfactor, grfactor);
  */
  PrevStateCrossSection state(medium, ptype,
                              energy, ccfactor,
                              ncfactor, grfactor);
                             
  if (prev_state_crosssec_.UsePrev(state)) {

     //log_trace("You have called the function before. Return saved value.");
     map<double,InteractionBasePtr>::reverse_iterator 
            iter = crosssection_map_.rbegin();

     /*
     log_trace("previous parameter medium %d ptype %d energy %f "
               "ccfactor %d ncfactor %d grfactor %d "
               "matched to current param. Return previous result %g", 
               int(prev_medium_), int(prev_type_), prev_energy_, 
               prev_ccfactor_, prev_ncfactor_, prev_grfactor_, iter->first);
     */
     if (ccweight) (*ccweight) = prev_state_crosssec_.ccweight_;
     if (ncweight) (*ncweight) = prev_state_crosssec_.ncweight_;
     if (grweight) (*grweight) = prev_state_crosssec_.grweight_;
     return iter->first;  // first: accumulated cross section
  }
  log_trace("This parameters are new. Need recalculate!");

  //
  // now we recalculate crosssection.
  // update flags
  //
  crosssection_map_.clear();
  prev_state_crosssec_ = state;

  // initialize weight factors. negative value represents N/A.
  if (ccweight) (*ccweight) = -1;
  if (ncweight) (*ncweight) = -1;
  if (grweight) (*grweight) = -1;

  // initialize temporary map
  map<InteractionType, double> crosssection_per_interaction_map_orig;
  crosssection_per_interaction_map_orig[CC] = 0.;
  crosssection_per_interaction_map_orig[NC] = 0.;
  crosssection_per_interaction_map_orig[GR] = 0.;

  map<InteractionType, double> crosssection_per_interaction_map;
  crosssection_per_interaction_map[CC] = 0.;
  crosssection_per_interaction_map[NC] = 0.;
  crosssection_per_interaction_map[GR] = 0.;

  log_trace("Get total-crosssection from %zd crosssection information",
               interactionVect_.size());
  log_trace("start CE for material (%.2f, %.2f, %.2f), r=%.2f [m]",
             positionInCE.GetX(), positionInCE.GetY(), positionInCE.GetZ(), positionInCE.Magnitude());

  // clear total buffers

  double total_crosssection = 0;
  double total_crosssection_orig = 0;

  vector<InteractionBasePtr>::const_iterator iter = interactionVect_.begin();

  //
  // loop over interactionVect_
  //
  for (; iter != interactionVect_.end(); ++iter) {

     // Get the interaction type
     InteractionBasePtr int_ptr = (*iter);
     InteractionType inttype = int_ptr->GetInteractionType();

     //
     // find a global scale and local scale
     //
     double global_scale = 1.0;
     double scale = 1.0;
     InteractionTypeDoubleMap gscales = steer_->GetGlobalXsecScales(); 
     if (inttype == CC) {
         global_scale = gscales[CC];
         scale = ccfactor;
     } else if (inttype == NC) {
         global_scale = gscales[NC];
         scale = ncfactor;
     } else if (inttype == GR) {
         global_scale = gscales[GR];
         scale = grfactor;
     }
   
     // 
     // active check for each neutrino flavor
     // Is this interaction applicable for the given particle?
     //
     if (IsFlavorActive(int_ptr->GetActiveFlavorMask(), ptype)) { 
        log_trace("Interaction %s is active, flavor mask %d, this particle's flavor %d", 
                   GetInteractionTypeString(inttype).c_str(), 
                   int_ptr->GetActiveMaterialMask(), ptype);


        //
        // prepare target nucleon candidates.
        //
        // NuGen handles various target nucleus separately.
        // Check whether the particle interact each nucleon.
        // This is important if we use different cross-section-tables
        // for proton, neutron and electron: though currently 
        // we use same tables for proton and neutron 
        // under the assumption of isoscalar parton (0.5n + 0.5p).
        // This is true for rock(SiO2), but not true for ice(H2O) and
        // we will improve it at next cross section table release.
        // The effect of the inconsistency in Ice will be ~4% 
        // at low energy. (K.Hoshina)
        //
        map<string, NuGMaterialConstitute> matmap;
        unsigned int matmask = int_ptr->GetActiveMaterialMask();
    
        if (IsMaterialActive(matmask, I3NuGInteractionInfo::PROTON)) { 
           matmap["PROTON"] = I3NuGInteractionInfo::PROTON;
        }
        if (IsMaterialActive(matmask, I3NuGInteractionInfo::NEUTRON)) { 
           matmap["NEUTRON"] = I3NuGInteractionInfo::NEUTRON;
        }
        if (IsMaterialActive(matmask, I3NuGInteractionInfo::ELECTRON)) { 
           matmap["ELECTRON"] = I3NuGInteractionInfo::ELECTRON;
        }

        //
        // loop over target nucleon candidates
        //
        map<string, NuGMaterialConstitute>::iterator matiter;
        for (matiter = matmap.begin(); matiter != matmap.end(); ++matiter) {
           string mattype = matiter->first;
           NuGMaterialConstitute matconst = matiter->second;

           // get ratio of cross section for the target nucleon.
           double ratio = GetDensityRatioForIsoscalarCrossSection(medium, matconst); 

           log_trace("this cross-section material %u active with %s",
                      matmask, mattype.c_str());
           log_trace("Material is %s,  Get %s Ratio %f", 
                     (medium==I3NuGInteractionInfo::ICE ? string("ICE") : string("ROCK")).c_str(),
                      mattype.c_str(), GetDensityRatioForIsoscalarCrossSection(medium, matconst));

           // get cross section in mb from cm2
           double sigma_at_e = int_ptr->GetXsecCGS(energy)*1e27;
           log_trace("Xsec for ene %g is %g [mb]", energy, sigma_at_e); 


           // calculate cross section

           // crosssection with global scale only
           double crosssection_orig = global_scale * ratio * sigma_at_e;
           // crosssection with global scale x artificial weight
           double crosssection = scale * crosssection_orig;

           if (crosssection <= 0) {  
              //set int_ptr for accumulated total cross section 0  
              //so that this interaction won't be selected. 
              //(we must have at least one entry for crosssection_map_) 
              crosssection_map_[0] = int_ptr; 
              crosssection_per_interaction_map[inttype] += crosssection; 
           } else { 
              total_crosssection += crosssection; 
              crosssection_per_interaction_map[inttype] += crosssection; 
              crosssection_map_[total_crosssection] = int_ptr; 
           } 
 
           log_trace("+++ total_crosssec = %g, "
                     "global scale = %g, %s, scale = %g, "
                     "PType %d, MatType %s, Medium ratio = %g, "
                     "sigma at energy %g = %g, crosssection = %g [mb]", 
                     total_crosssection, global_scale, 
                     GetInteractionTypeString(inttype).c_str(),
                     scale, ptype, mattype.c_str(), ratio, 
                     energy, sigma_at_e, 
                     crosssection); 

           // following variables are used to calculate weight
           total_crosssection_orig += crosssection_orig;
           crosssection_per_interaction_map_orig[inttype] += crosssection_orig;

           //log_trace("added map for accumulated crosssection %g", 
           //           total_crosssection);

        }
      
     } //is active flavor

  } // interaction loop end

  // you may reuse crosssection_map_ unless you change 
  // interactionVect_
  prev_state_crosssec_.use_prev_interaction_vect_ = true;

  // sanity check
  if (total_crosssection <= 0) {
     // OK, no interaction is applicable for this case.
     // return 0.
     return 0.;

  } else if (ccfactor == 1 && ncfactor == 1 && grfactor == 1) {
     // no calculation is needed anymore. 
     // set cc/nc/gr weight 1 and return.
     prev_state_crosssec_.ccweight_ = 1;
     prev_state_crosssec_.ncweight_ = 1;
     prev_state_crosssec_.grweight_ = 1;
     if (ccweight) (*ccweight) = 1;
     if (ncweight) (*ncweight) = 1;
     if (grweight) (*grweight) = 1;

     return total_crosssection;
  }
     
  //
  // now all variables used in following code should be greater than 0.
  // calculate weight of crosssection ratio
  // which compensate artificial effect of ratio of cross sections
  // due to ccfactor etc.
  //
  map<InteractionType, double>::iterator itmp;
  for (itmp = crosssection_per_interaction_map.begin(); 
       itmp!= crosssection_per_interaction_map.end(); ++itmp) {

     InteractionType inttype = itmp->first;
     double crosssection_tmp = itmp->second;
     double crosssection_orig = crosssection_per_interaction_map_orig[inttype];

     double orig_ratio = crosssection_orig / total_crosssection_orig;
     double tmp_ratio = crosssection_tmp / total_crosssection;

     double mod_weight = (tmp_ratio>0 ? orig_ratio/tmp_ratio : 0);

     if (inttype == CC) {
        prev_state_crosssec_.ccweight_ = mod_weight;
     } else if (inttype == NC) {
        prev_state_crosssec_.ncweight_ = mod_weight;
     } else if (inttype == GR) {
        prev_state_crosssec_.grweight_ = mod_weight;
     }

  }

  // store reference valiables
  if (ccweight) (*ccweight) = prev_state_crosssec_.ccweight_;
  if (ncweight) (*ncweight) = prev_state_crosssec_.ncweight_;
  if (grweight) (*grweight) = prev_state_crosssec_.grweight_;

  //printf("ccweight %g, ncweight %g, grweight %g\n", *ccweight,
  //       *ncweight, *grweight);

  return total_crosssection;

}

//_____________________________________________________________________
InteractionType I3NuGInteractionInfo::MakeInteraction(
                       ParticlePtr nuin_ptr,
                       double ccratio, double ncratio, double grratio)
{  
//
// Make an interaction and fill daughter particles.
// The argument nuin_ptr must have a track length that specify
// the interaction position.
// Make sure you have determined it within an event class.
//
// ccratio, ncratio, and grratio is scale factors 
// for CC, NC and Grashow interaction.
// If you set positive value, corresponding cross sections are scaled.
// e.g. if you set ccratio:ncratio:grration = 2:1:1, you will have 
// more CC interaction than normal operation.
// 
// It also set propagation weight which compensates
// the modified scales.
// See descriptions in GetTotalCrossserction function.
//


  log_trace("I3NuGInteractionInfo::MakeInteraction is called");

  // initialize interaction weight of the step
  double intweight = 1.0;

  //
  // get end position of the track which should be the
  // interaction vertex
  // 
  I3Position prepos(nuin_ptr->GetPos());
  I3Position interaction_position(nuin_ptr->GetEndPosition());
  I3Position interaction_position_CE(
                 earth_-> GetEarthCoordPosFromDetCoordPos(interaction_position));

  const double energy = nuin_ptr->GetEnergy();

  log_debug("Interaction position CE (%f,%f,%f) R=%f[m], energy=%f [GeV]", 
             interaction_position_CE.GetX(), interaction_position_CE.GetY(), 
             interaction_position_CE.GetZ(), interaction_position_CE.Magnitude(), energy);

  //
  // Get total cross section
  //
  double ccweight, ncweight, grweight;
  log_debug("ccr = %g, ncr = %g, grr = %g", ccratio, ncratio, grratio);
  double totalCrosssectionAtInteractionPosition 
               = GetTotalCrosssection(nuin_ptr, 
                                      interaction_position_CE, 
                                      ccratio, ncratio, grratio,
                                      &ccweight, &ncweight, &grweight);
  log_debug("ccw = %g, ncw = %g, grw = %g", ccweight, ncweight, grweight);

  if (totalCrosssectionAtInteractionPosition==0.0) {
     // it may happen if primary neutrino lose energy during
     // propagation and InIce nu energy is less than 10GeV.
     log_debug("no active neutrino");
     return NOINTERACTION;
  }    

  // choose an interaction with a random number.

  double  rand = random_->Uniform(0, 1) * totalCrosssectionAtInteractionPosition;
  log_debug("total crosssection %f x10^-12, random crosssection %f x10^-12", 
	    totalCrosssectionAtInteractionPosition*pow(10.,12.), rand*pow(10.,12.));

  // 
  // since we called GetTotalCrossSection in this function, the
  // temporary buffer crossection_map_ is filled!
  // Just find an interaction that corresponds to our random number
  //
  // key : accumulated probability
  // value: interaction pointer
  // ---------------------------------------------------------
  // entry id          | 0    | 1      | 2        | 3 ...... 
  // key(prob)         | c0   | c0+c1  | c0+c1+c2 | ....
  // val(interaction)  | p0   | p1     | p2       | ....
  // ---------------------------------------------------------
  // if 0 <= x <= c0 then
  // lower_bound(x) gives entry id 0
  // if c0 < x <= (c0+c1) then
  // lower_bound(x) gives entry id 1
  //

  map<double,InteractionBasePtr>::iterator iter;

  /* this is for debug.
  for (iter = crosssection_map_.begin(); iter != crosssection_map_.end() ; ++iter) {

     InteractionBasePtr int_ptr = iter->second;
     InteractionType inttype = int_ptr->GetInteractionType();
     log_debug("accum_xsec = %g, interaction %s", 
                iter->first, GetInteractionTypeString(inttype).c_str());
  }
  */

  iter = crosssection_map_.lower_bound(rand);
  InteractionBasePtr int_ptr = iter->second;
  InteractionType inttype = int_ptr->GetInteractionType();

  

  log_trace("        # Chosen an interaction %d(%s)", 
            inttype, GetInteractionTypeString(int_ptr->GetInteractionType()).c_str());

  //
  // fill crosssection weight
  //
  if (inttype == CC) intweight = ccweight;
  else if (inttype == NC) intweight = ncweight;
  else if (inttype == GR) intweight = grweight;

  //
  // set interaction type and cross section etc.
  //
  nuin_ptr->GetInteractionInfoRef().SetInteractionType(int_ptr->GetInteractionType());

  // this cross section is TRUE cross section 
  // (if you set GlobalCCFactor etc, this cross section is 
  //  NOT scaled with GlobalCCFactor yet)
  nuin_ptr->GetInteractionInfoRef().SetInteractionXsecCGS(int_ptr->GetXsecCGS(energy));

  // set propagation weight for this particle.
  // if you call the function with default values, 
  // the intweight should be 1.
  // you must set it before calling FillDaughterParticles().
  //
  double oldweight = nuin_ptr->GetPropagationWeight();
  //printf("prop weight old %g, new %g\n", oldweight, intweight*oldweight);
  nuin_ptr->SetPropagationWeight(oldweight * intweight);

  //
  // fill daughter particles
  //
  log_trace("        # daughter %d particles, before fill", nuin_ptr->GetTotalNDaughters());
  int_ptr->FillDaughterParticles(nuin_ptr); 
  log_trace("        # daughter list filled with %d particles", nuin_ptr->GetTotalNDaughters());
  
  //
  // check particles
  //
  nuin_ptr->CheckParticle();
  log_trace("parent energy %f [GeV], sum of daughter energies %f [GeV]", 
	    nuin_ptr->GetEnergy(),nuin_ptr->GetTotalDaughterEnergies());

  nuin_ptr->CheckDaughters();

  //
  // return interaction type
  //
  return nuin_ptr->GetInteractionInfoRef().GetInteractionType();
 
}

//_____________________________________________________________________
bool I3NuGInteractionInfo::IsMaterialActive(const unsigned int activematerial, 
					    const NuGMaterialConstitute material)
{
						 
  bool active=false;

  //100 neutron only mask (third digit is 1)
  // 10 proton only mask  (second digit is 1)
  //  1 electron only     (first digit is 1)
  //else is the conbination 
  //
  //thus, for example  
  //110 is active with nucleons(neutron and proton)
  // 11 proton and electron
  //111 all. proton, electron and neutron

  switch (material) {

     case PROTON :
 
        if (activematerial ==  10 || 
            activematerial ==  11 || 
            activematerial == 111 ||
            activematerial == 110) {

           active = true; // proton active
        }
		  break;

     case NEUTRON :
  
        if (activematerial == 100 || 
            activematerial == 110 ||
            activematerial == 101 ||
            activematerial == 111) {
          active = true; // neutron active
        }
		  break;

     case ELECTRON :
  
        if (activematerial == 111 || 
            activematerial == 101 ||
            activematerial ==   1 ||
            activematerial ==  11) {
          active = true; //electron active
        }
		  break;

     default :
        log_error("material not support?");

  }
  
  return active;
} 
///////////////////////////////////////////////////////////////////////////////////
bool I3NuGInteractionInfo::IsFlavorActive(const unsigned int activeflavor, const I3Particle::ParticleType nuflavor){
  bool active=false;
  // the sheme is nue, nuebar, numu, numubar, nutau, nutaubar
  // example: 1001 means numu and nutaubar are active
  bool thisIsNu=false;
  bool thisIsAntiNu=false;
  if(nuflavor == I3Particle::NuE || 
     nuflavor == I3Particle::NuMu||
     nuflavor == I3Particle::NuTau)thisIsNu=true;

  else if(nuflavor == I3Particle::NuEBar || 
	  nuflavor == I3Particle::NuMuBar||
	  nuflavor == I3Particle::NuTauBar)thisIsAntiNu=true;

  if      (activeflavor == 101010 && thisIsNu)                         active = true;   // its a  neutrino
  else if (activeflavor ==  10101 && thisIsAntiNu)                     active = true;   // its an anti neutrino
  else if (activeflavor ==  10000 && nuflavor == I3Particle::NuEBar  ) active = true;   // its an anti electron neutrino
  else if (activeflavor == 100000 && nuflavor == I3Particle::NuE     ) active = true;   // its an electron neutrino
  else if (activeflavor ==    100 && nuflavor == I3Particle::NuMuBar ) active = true;   // its an anti muon neutrino
  else if (activeflavor ==   1000 && nuflavor == I3Particle::NuMu    ) active = true;   // its a  muon neutrino
  else if (activeflavor ==      1 && nuflavor == I3Particle::NuTauBar) active = true;   // its an anti tau  neutrino
  else if (activeflavor ==     10 && nuflavor == I3Particle::NuTau   ) active = true;   // its a  tau neutrino
  
  return active;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Beta(E) = (1/E)*(dE/dX) in cm^2/g
// 8e-9                       is due to bremstrahlung
// 1.5e-7                     is due to pair-production
// 6.3*10^{-7}*(E/10^9)^(0.2) is due to photonuclear scattering
///////////////////////////////////////////////////////////////////////////
double I3NuGInteractionInfo::GetTauBeta(const double E) {
  if ( E > 1E4)  //old parametrization
    return (1.508 + 6.3*pow((E/1e9),0.2))*1e-7;
  else   
    return 2.138e-7;
  }
///////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________
const double I3NuGInteractionInfo::CalculateTotalColumnDepth(const  I3Position & entCE, 
                                                             const  I3Position & extCE, 
                                                             int    nsteps,
                                                             double maxstepsize,
                                                             ParticlePtr particle_ptr,
                                                             double ccfactor, 
                                                             double ncfactor, 
                                                             double grfactor)
{                     
/**
 * Note that this function called in term of the center of 
 * earth coordinate
 * If particle_ptr is not null, it also calculates 
 * accumulated value of 
 * (total_crosssection * column_depth) 
 * which is used to calculate interaction probability
 * as an index of exponential
 */

  //
  // check previous calculation
  //

  log_trace("CalculateTotalColumnDepth is called with "
            "entpos(%g,%g,%g), extpos(%g,%g,%g), len=%g "
            "nsteps=%d, "
            "maxstep=%g, particle=%p ",
            entCE.GetX(), entCE.GetY(), entCE.GetZ(), extCE.GetX(), extCE.GetY(), 
            extCE.GetZ(), (extCE-entCE).Magnitude(), 
            nsteps, maxstepsize, particle_ptr.get());

  PrevStateColumnDepth state(entCE, extCE, 
                             nsteps, maxstepsize,
                             particle_ptr,
                             ccfactor, ncfactor,
                             grfactor);

  if (prev_state_columndep_.UsePrev(state)) {

     log_trace("previous params :"
            "entpos(%g,%g,%g), extpos(%g,%g,%g), len=%g, nsteps=%d, "
            "maxstep=%f, particle=%p",
            prev_state_columndep_.enterCE_.GetX(), 
            prev_state_columndep_.enterCE_.GetY(), 
            prev_state_columndep_.enterCE_.GetZ(),
            prev_state_columndep_.exitCE_.GetX(), 
            prev_state_columndep_.exitCE_.GetY(), 
            prev_state_columndep_.exitCE_.GetZ(), 
            (prev_state_columndep_.exitCE_ 
             -prev_state_columndep_.enterCE_).Magnitude(),
            prev_state_columndep_.nsteps_, 
            prev_state_columndep_.maxstepsize_, 
            prev_state_columndep_.particleptr_.get());   

     // use former result!
     map<double,double>::reverse_iterator riter = length_vs_columndepth_map_.rbegin();
     double cdep = riter->second;
     log_trace("got last value %g", cdep);
     return cdep;  // second: accumulated column depth
  }

  prev_state_columndep_ = state;
  length_vs_columndepth_map_.clear();
  columndepth_vs_length_map_.clear();
  crosssec_columndepth_map_.clear();

  double maxLength = (extCE-entCE).Magnitude();
  I3Direction dirCE(extCE-entCE);
  double dx  = 0; 

  double columndepth   = 0;
  double crosssec_columndepth   = 0;
  double length   = 0;
  I3Position localCE(entCE);
  I3Position lastlocalCE(entCE);

  // set edge of map
  length_vs_columndepth_map_[length] = columndepth;
  columndepth_vs_length_map_[columndepth] = length;
  crosssec_columndepth_map_[columndepth] = crosssec_columndepth;

  // start loop.
  // 2015.5.28 removed nsteps stepping, instead, 
  // use DistanceToNextBoundary for minor bug fix.
  // CAUTION : this code is OK only when we assume constant
  // density in a layer. For final interaction, currently we
  // are using constant density for ice and bed rock.
  while (maxLength > length) {

     dx = Calculator::DistanceToNextBoundary(earth_, localCE, dirCE);
     length += dx;

     if (length > maxLength) {
        // this is the final step
        dx -= (length - maxLength);
        length = maxLength;
     }
     
     localCE = entCE + length * dirCE;  
     I3Position middleCE = 0.5*(lastlocalCE + localCE);

     // density check. Get New Density
     double dens = earth_->GetEarthDensityInCGS(middleCE);

     // get column depth in [g/m3]
     // columndepth is now in c*(g/cm3)*m = c*1.0e6 (g/m3)*m= 1.0e6 g/m2
     double current_columndepth = dens*dx*1.0e6;

     columndepth += current_columndepth;
     length_vs_columndepth_map_[length] = columndepth;
     columndepth_vs_length_map_[columndepth] = length;

     if (particle_ptr) {
        // get crosssection at current position
        // with all interaction
		  // This may be used when we calculate weight
		  // as a substitute of totalcrosssec * totalcolumndepth.
		  // By default, however, we don't use this calculation.
		  // to use, switchOn CrosssectionColumndepthOpt in I3NeutrinoGenerator.
        double current_totcrosssec = GetTotalCrosssection(particle_ptr,
                                     localCE, ccfactor, ncfactor, grfactor);

        crosssec_columndepth += current_totcrosssec * current_columndepth;
        crosssec_columndepth_map_[columndepth] = crosssec_columndepth;
     }

     lastlocalCE = localCE;

  }

  map<double,double>::reverse_iterator iter = length_vs_columndepth_map_.rbegin();
  return iter->second;  // second: accumulated column depth

}

//_____________________________________________________________________
const double I3NuGInteractionInfo::ConvertColumnDepthToMeter(double columndep)
{
/**
 * convert columndepth to meter
 *
 * !!! CAUTION !!!
 * you MUST call CalculateTotalColumnDepth beforehand.
 * This function returns pre-calculated result.
 * The resolution of returned columndepth depends on how you 
 * called CalculateTotalColumnDepth.
 * If you need 10m resolution, for example,
 * you must specify maxstepsize = 10[m] .
 */

  // key: accumulated columndepth
  // value : length corresponds accumulated columndepth
  // ---------------------------------------------------------
  // entry id  | 0    | 1      | 2        | 3 ...... 
  // key[g/m2] | 0    | c1     | c1+c2    | c1+c2+c3  ....
  // val[m]    | 0    | p1     | p2       | ....
  // ---------------------------------------------------------
  // if 0 <= x < c1 then
  // upper_bound(x) gives entry id 1
  // if c1 <= x < (c1+c2) then
  // upper_bound(x) gives entry id 2
  // we never get entry id 0 with upper_bound function
  // because x is always greater equal 0

  map<double,double>::iterator iter, previter, iterbegin, iterlast, iterend;
  iterbegin = columndepth_vs_length_map_.begin();
  iterend = columndepth_vs_length_map_.end();
  iterlast = iterend;
  --iterlast;

  if (iterlast->first == columndep) {
     // you want to get length at total columndepth, just return final entry
     // it's also essential to avoid range out with 
     // upper_bound function.
     return iterlast->second;
  }
  
  // otherwise you want to get geometrical length[m] up to a given columndepth..
  // Find it with upper_bound function

  iter = columndepth_vs_length_map_.upper_bound(columndep);

  if (iter == iterend) {
     log_error("Oops! columndepth %g is out of map boundary [%g,%g],"
               " can't get length",
                columndep, iterbegin->first, iterlast->first);
  } else if (iter == iterbegin) {
     log_error("Hmm. we shouldn't get 0th entry with this case. " 
               " columndepth %g map boundary [%g,%g],"
               " can't get columndepth",
                columndep, iterbegin->first, iterlast->first);
  }

  log_trace("columndepth %g is found in [%g,%g]",
                columndep, iterbegin->first, iterlast->first);

  // OK, now chosen columndepth is mostly larger than given
  // columndep because we used upper_bound function.
  // We apply linear interpolation to get more precise length 
  // for a given columndep.

  // check cdep step
  previter = iter;
  --previter;
  double dcolumndep = iter->first - previter->first;
  double dx = iter->second - previter->second;
  if (dcolumndep <= 0) { 
     log_error("only one entry is stored in the map? Check step size."); 
  }

  double ratio = (columndep - previter->first) / dcolumndep;
  double length = previter->second + ratio * dx;

  log_debug("return length %g for columndepth %g",
             length, columndep);

  return length;

}

//_____________________________________________________________________
const double I3NuGInteractionInfo::ConvertMeterToColumnDepth(double length_meter)
{
/**
 * convert length [m] to columndepth
 *
 * !!! CAUTION !!!
 * you MUST call CalculateTotalColumnDepth beforehand.
 * It returns pre-calculated result.
 * The resolution of returned length depends on how you 
 * called CalculateTotalColumnDepth.
 * If you need 10m resolution, for example,
 * you must specify maxstepsize = 10[m] .
 */

  // key : length corresponds accumulated columndepth
  // value: accumulated columndepth
  // ---------------------------------------------------------
  // entry id  | 0    | 1      | 2        | 3 ...... 
  // key[m]    | 0[m] | c1[m]  | c1+c2[m] | c1+c2+c3[m] | ....
  // val[g/m2] | 0    | p1     | p2       | ....
  // ---------------------------------------------------------
  // if 0 <= x < c1 then
  // upper_bound(x) gives entry id 1
  // if c1 <= x < (c1+c2) then
  // upper_bound(x) gives entry id 2
  // we never get entry id 0 with upper_bound function
  // because x is always greater equal 0
  //

  map<double,double>::iterator iter, previter, iterbegin, iterlast, iterend;
  iterbegin = length_vs_columndepth_map_.begin();
  iterend = length_vs_columndepth_map_.end();
  iterlast = iterend;
  --iterlast;

  if (iterlast->first == length_meter) {
     // you want to get total columndepth, just return final entry
     // it's also essential to avoid range out with 
     // upper_bound function.
     return iterlast->second;
  }
  
  // otherwise you want to get columndepth up to somewhere.
  // Find it with upper_bound function

  iter = length_vs_columndepth_map_.upper_bound(length_meter);

  if (iter == iterend) {
     log_error("Oops! length %g is out of map boundary [%g,%g],"
               " can't get columndepth",
                length_meter, iterbegin->first, iterlast->first);
  } else if (iter == iterbegin) {
     log_error("Hmm. we shouldn't get 0th entry with this case. " 
               " length %g map boundary [%g,%g],"
               " can't get columndepth",
                length_meter, iterbegin->first, iterlast->first);
  }

  log_trace("length %g is found in [%g,%g]",
                length_meter, iterbegin->first, iterlast->first);

  // OK, now chosen length is mostly larger than given
  // length_meter because we used upper_bound function.
  // We apply linear interpolation to get more precise columndepth
  // for length_meter.

  // check length step
  previter = iter;
  --previter;
  double dx = iter->first - previter->first;
  double dcolumndep = iter->second - previter->second;
  if (dx <= 0) { 
     log_error("only one entry stored in the map? Check step size."); 
  }

  double ratio = (length_meter - previter->first) / dx;
  double columndepth = previter->second + ratio * dcolumndep;

  log_debug("return columndepth %g for length %g",
             columndepth, length_meter);

  return columndepth;

}

//_____________________________________________________________________
const double I3NuGInteractionInfo::GetCrosssectionColumndepthAt(double columndepth)
{
/**
 * get (total crosssection * columndepth) at given columndepth[w.m.e]
 *
 * !!! CAUTION !!!
 * you MUST call CalculateTotalColumnDepth beforehand.
 * It returns pre-calculated result.
 * The resolution of returned length depends on how you 
 * called CalculateTotalColumnDepth.
 * If you need 10m resolution, for example,
 * you must specify maxstepsize = 10[m] .
 */

  // key : accumulated columndepth
  // val : accumulated columndepth * crosssection
  // ---------------------------------------------------------
  // entry id  | 0    | 1      | 2        | 3 ...... 
  // key[g/m2] | 0    | c1     | c1+c2[m] | c1+c2+c3[m] | ....
  // val[g]    | 0    | p1     | p2       | ....
  // ---------------------------------------------------------
  // if 0 <= x < c1 then
  // upper_bound(x) gives entry id 1
  // if c1 <= x < (c1+c2) then
  // upper_bound(x) gives entry id 2
  // we never get entry id 0 with upper_bound function
  // because x is always greater equal 0

  map<double,double>::iterator iter, previter, iterbegin, iterlast, iterend;
  iterbegin = crosssec_columndepth_map_.begin();
  iterend = crosssec_columndepth_map_.end();
  iterlast = iterend;
  --iterlast;

  if (iterlast->first == columndepth) {
     // you want to get total product, just return final entry
     // it's also essential to avoid range out with 
     // upper_bound function.
     return iterlast->second;
  }
  
  // otherwise you want to get product up to somewhere.
  // Find it with upper_bound function

  iter = crosssec_columndepth_map_.upper_bound(columndepth);
  
  if (iter == iterend) {
     log_error("Oops! columndepth %g is out of map boundary [%g,%g],"
               " can't get product",
                columndepth, iterbegin->first, iterlast->first);
  } else if (iter == iterbegin) {
     log_error("Hmm. we shouldn't get 0th entry with this case. " 
               " columndepth %g map boundary [%g,%g],"
               " can't get columndepth x crosssection.",
                columndepth, iterbegin->first, iterlast->first);
  }

  log_trace("columndepth %g is found in [%g,%g]",
                columndepth, iterbegin->first, iterlast->first);

  // OK, now chosen columndep is always larger than given columndepth 
  // because we used upper_bound function.
  // We apply linear interpolation to get more precise product 
  // for columndepth.

  // check length step
  previter = iter;
  --previter;
  double dcdep = iter->first - previter->first;
  double dproduct = iter->second - previter->second;
  if (dcdep <= 0) { 
     log_error("only one entry stored in the map? Check step size."); 
  }

  double ratio = (columndepth - previter->first) / dcdep;
  double product = previter->second + ratio * dproduct;

  log_debug("return product %g for columndepth %g",
             product, columndepth);

  return product;

}


//////////////////////////////////////////////////////////
void I3NuGInteractionInfo::ViewInteractionInfo(){
  
  map<string, string>::iterator i;
  for (i = xsecs_.begin(); i != xsecs_.end(); ++i) {
     log_info("%s %s", i->first.c_str(), i->second.c_str());
  }

  log_info("CC nu active flavor mask %d", flavorMaskCCNu_);
  log_info("CC nu active material mask %d", materialMaskCCNu_);

  log_info("CC nubar active flavor mask %d", flavorMaskCCNuBar_);
  log_info("CC nubar active material mask %d", materialMaskCCNuBar_);

  log_info("NC nu active flavor mask %d", flavorMaskNCNu_);
  log_info("NC nu active material mask %d", materialMaskNCNu_);

  log_info("NC nubar active flavor mask %d", flavorMaskNCNuBar_);
  log_info("NC nubar active material mask %d", materialMaskNCNuBar_);

  log_info("GR nubar active flavor mask %d", flavorMaskGRNuBar_);
  log_info("GR nubar active material mask %d", materialMaskGRNuBar_);
  
  log_info("ice ratio, proton:neutron:electron= %f:%f:%f", 
	   GetDensityRatioForIsoscalarCrossSection(ICE, PROTON), 
		GetDensityRatioForIsoscalarCrossSection(ICE, NEUTRON), 
		GetDensityRatioForIsoscalarCrossSection(ICE, ELECTRON));
  log_info("rock ratio, proton:neutron:electron= %f:%f:%f", 
	   GetDensityRatioForIsoscalarCrossSection(ROCK, PROTON), 
		GetDensityRatioForIsoscalarCrossSection(ROCK, NEUTRON), 
		GetDensityRatioForIsoscalarCrossSection(ROCK, ELECTRON));

}

//______________________________________________________________________
double I3NuGInteractionInfo::GetDensityRatioForIsoscalarCrossSection(
                             NuGPropagatingMedium medium, 
			  NuGMaterialConstitute material)
{

  double density=0.0;
  if (medium == ICE) {
     switch (material){
        case PROTON:  
           density = protonh2o_;
           break;
        case NEUTRON: 
           density = neutronh2o_;
           break;
        case ELECTRON:
           density = electronh2o_;
           break;
        default: 
           density = 0.0;
     }

  } else {
     switch (material) {
        case PROTON:  
           density = protonrock_;
           break;
        case NEUTRON: 
           density = neutronrock_;
           break;
        case ELECTRON:
           density = electronrock_;
           break;
        default: 
           density = 0.0;
     }
  }

  if (density < 0) { 
     // do not allow negative density 
     density = 0.; 
  } 
  return density;
}

typedef I3SingleServiceFactory<I3NuGInteractionInfo>
I3NuGInteractionInfoFactory;
I3_SERVICE_FACTORY( I3NuGInteractionInfoFactory );

