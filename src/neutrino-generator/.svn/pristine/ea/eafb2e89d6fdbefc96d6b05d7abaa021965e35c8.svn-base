/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief Interaction implimentation file
 *
 */
//////////////////////////////////////////////////////////////////////
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/interactions/InteractionManagerBase.h"
#include "neutrino-generator/interactions/InteractionBase.h"
#include "neutrino-generator/interactions/NuGInteractionManager.h"
#include "neutrino-generator/table-interface/TableUtils.h"
#include "neutrino-generator/table-interface/CrosssectionTableReader.h"
#include "neutrino-generator/table-interface/FinalStateTableReader.h"
#include "neutrino-generator/interactions/InteractionCC.h"
#include "neutrino-generator/interactions/InteractionNC.h"
#include "neutrino-generator/interactions/InteractionGR.h"
#include "neutrino-generator/interactions/TauDecay.h"
#include "neutrino-generator/utils/Utils.h"
#include "neutrino-generator/utils/Constants.h"

#include "icetray/I3SingleServiceFactory.h"

using namespace std;
using namespace earthmodel;

namespace nugen {

//_________________________________________________________________
NuGInteractionManager::NuGInteractionManager(const I3Context &c)
       : InteractionManagerBase(c)
{

   AddParameter("RandomService","pointer to random service",random_);
   steeringName_ = "";
   AddParameter("SteeringName","name of Steering ",steeringName_);

   path_ = "";
   AddParameter("PathToFileDir",
                "Path to the directory containing the "
                "data files",
                path_);

   xsec_model_string_ = "csms"; 
   AddParameter("CrossSectionModel", 
                "Name of cross section model (=modelname). "
                "modelname.list must "
                "exist under cross_section_data,"
                " either in $I3_DATA/neutrino-generator/ or $I3_BUILD/neutrino-generator/resources.",
                  xsec_model_string_);

}

//_________________________________________________________________
NuGInteractionManager::NuGInteractionManager(
                         I3RandomServicePtr rand, 
                         SteeringPtr p,
                         const std::string &model,
                         const std::string &path,
                         const string &name) 
       : InteractionManagerBase(rand, p, name), 
         xsec_model_string_(model), path_(path)
{
   Initialize();
}


//_________________________________________________________________
void NuGInteractionManager::Configure()
{
   GetParameter("RandomService",random_);
   GetParameter("SteeringName",steeringName_);
   GetParameter("PathToFileDir", path_);
   GetParameter("CrossSectionModel", xsec_model_string_);

   if (context_.Has(steeringName_)) {
      steer_ = context_.Get<SteeringPtr>(steeringName_);
   } else {
      log_error("You have to add I3NuGSteeringService to tray "
               "before I3NuGInteractionManager");
   }

   Initialize();
}

//_________________________________________________________________
void NuGInteractionManager::Initialize()
{

   //-----------------------------------------------------------
   // 1. Generate interactions
   // CC/NC interactions are applicable for all flavor neutrino
   // and target proton and neutron
   //-----------------------------------------------------------

   // read cross section list
   TableUtils::ReadTableList(path_, xsec_model_string_, xsecs_);

   // 1. generate CC Nu-P interactions
   InteractionCCPtr ccNuP_ptr(new InteractionCC(random_, steer_));
   ccNuP_ptr->InitializeCrosssectionTable(xsecs_["CC_Nu_p_xsec"]);
   ccNuP_ptr->InitializeFinalStateTable(xsecs_["CC_Nu_p_final"]);

   // 2. generate CC NuBar-P interactions
   InteractionCCPtr ccNuBarP_ptr(new InteractionCC(random_, steer_));
   ccNuBarP_ptr->InitializeCrosssectionTable(xsecs_["CC_NuBar_p_xsec"]);
   ccNuBarP_ptr->InitializeFinalStateTable(xsecs_["CC_NuBar_p_final"]);

   // 3. generate CC Nu-N interactions
   InteractionCCPtr ccNuN_ptr(new InteractionCC(random_, steer_));
   ccNuN_ptr->InitializeCrosssectionTable(xsecs_["CC_Nu_n_xsec"]);
   ccNuN_ptr->InitializeFinalStateTable(xsecs_["CC_Nu_n_final"]);

   // 4. generate CC NuBar-N interactions
   InteractionCCPtr ccNuBarN_ptr(new InteractionCC(random_, steer_));
   ccNuBarN_ptr->InitializeCrosssectionTable(xsecs_["CC_NuBar_n_xsec"]);
   ccNuBarN_ptr->InitializeFinalStateTable(xsecs_["CC_NuBar_n_final"]);

   // 5. generate NC Nu-P interactions
   InteractionNCPtr ncNuP_ptr(new InteractionNC(random_, steer_));
   ncNuP_ptr->InitializeCrosssectionTable(xsecs_["NC_Nu_p_xsec"]);
   ncNuP_ptr->InitializeFinalStateTable(xsecs_["NC_Nu_p_final"]);

   // 6. generate NC NuBar-P interactions
   InteractionNCPtr ncNuBarP_ptr(new InteractionNC(random_, steer_));
   ncNuBarP_ptr->InitializeCrosssectionTable(xsecs_["NC_NuBar_p_xsec"]);
   ncNuBarP_ptr->InitializeFinalStateTable(xsecs_["NC_NuBar_p_final"]);

   // 7. generate NC Nu-N interactions
   InteractionNCPtr ncNuN_ptr(new InteractionNC(random_, steer_));
   ncNuN_ptr->InitializeCrosssectionTable(xsecs_["NC_Nu_n_xsec"]);
   ncNuN_ptr->InitializeFinalStateTable(xsecs_["NC_Nu_n_final"]);

   // 8. generate NC NuBar-N interactions
   InteractionNCPtr ncNuBarN_ptr(new InteractionNC(random_, steer_));
   ncNuBarN_ptr->InitializeCrosssectionTable(xsecs_["NC_NuBar_n_xsec"]);
   ncNuBarN_ptr->InitializeFinalStateTable(xsecs_["NC_NuBar_n_final"]);

   // prepare possible neutrino types
   vector<I3Particle::ParticleType> allflavors;
   allflavors.push_back(I3Particle::NuE);
   allflavors.push_back(I3Particle::NuMu);
   allflavors.push_back(I3Particle::NuTau);

   // clear drivers_ multi-dimension map
   drivers_.clear();

   const int pdg_p = Constants::PDG_P; // pdg code for proton
   const int pdg_n = Constants::PDG_N; // pdg code for neutron

   for (unsigned int i=0; i<allflavors.size(); ++i) {

      MatDrivers cc_per_mat_map;
      cc_per_mat_map[pdg_p] = ccNuP_ptr;
      cc_per_mat_map[pdg_n] = ccNuN_ptr;

      MatDrivers nc_per_mat_map;
      nc_per_mat_map[pdg_p] = ncNuP_ptr;
      nc_per_mat_map[pdg_n] = ncNuN_ptr;

      IntTypeDrivers per_type_map;
      per_type_map[CC] = cc_per_mat_map;
      per_type_map[NC] = nc_per_mat_map;

      drivers_[allflavors[i]] = per_type_map;
   }

   // prepare possible anti-neutrino types

   vector<I3Particle::ParticleType> allantiflavors;
   allantiflavors.push_back(I3Particle::NuEBar);
   allantiflavors.push_back(I3Particle::NuMuBar);
   allantiflavors.push_back(I3Particle::NuTauBar);

   for (unsigned int i=0; i<allantiflavors.size(); ++i) {

      MatDrivers cc_per_mat_map;
      cc_per_mat_map[pdg_p] = ccNuBarP_ptr;
      cc_per_mat_map[pdg_n] = ccNuBarN_ptr;

      MatDrivers nc_per_mat_map;
      nc_per_mat_map[pdg_p] = ncNuBarP_ptr;
      nc_per_mat_map[pdg_n] = ncNuBarN_ptr;

      IntTypeDrivers per_type_map;
      per_type_map[CC] = cc_per_mat_map;
      per_type_map[NC] = nc_per_mat_map;

      drivers_[allantiflavors[i]] = per_type_map;
   }
 
 
   //-----------------------------------------------------------
   // 2. Generate GR interaction
   // GR interaction is applicable only for NuEBar and electron
   // and doesn't use any table (hardcoded).
   //-----------------------------------------------------------

   InteractionGRPtr grNuEBarE_ptr(new InteractionGR(random_, steer_));

   MatDrivers gr_per_mat_map;
   gr_per_mat_map[Constants::PDG_E] = grNuEBarE_ptr; 

   // then add it proper map
   (drivers_[I3Particle::NuEBar])[nugen::GR] = gr_per_mat_map;

   //-----------------------------------------------------------
   // 3. Read tau decay file
   //-----------------------------------------------------------

   tau_decay_ = TauDecayPtr(new TauDecay(random_, steer_));
   tau_decay_->InitializeFinalStateTable(xsecs_["Tau_Decay"]);

   // set default interaction weight scale
   default_intfactors_[CC] = 1.0;
   default_intfactors_[NC] = 1.0;
   default_intfactors_[GR] = 1.0;
   default_intfactors_[DECAY] = 1.0;

   //-----------------------------------------------------------
   // 4. Set me on Steering 
   //-----------------------------------------------------------
   steer_->AddInteractionManager(NUGEN, GetSharedPtr());

}

//_________________________________________________________________
void NuGInteractionManager::CalcTotalXsec(
         const I3Position & pos_CE,
         I3Particle::ParticleType ptype,
         double energy,
         const InteractionTypeDoubleMap & intfactors, 
         const InteractionTypeDoubleMap & globalfactors)
{

   earthmodel::EarthModelServicePtr earth = steer_->GetEarthModelService();

   // check if we need to re-calculate Xsec

   EarthModelService::MediumType medtype = earth->GetMedium(pos_CE); 
   if (! UpdateXsec(medtype, ptype, energy,
                    intfactors, globalfactors) ) return; 

   // update buffers
   cur_ptype_ = ptype;
   cur_energy_ = energy;
   cur_medium_type_= medtype;
   cur_int_factors_.clear();
   cur_int_factors_ = intfactors;
   cur_global_factors_.clear();
   cur_global_factors_ = globalfactors;

   // get material's number-ratio map
   // NuGen uses count ratio of Proton, Neutron and Electron.
   // Use PNERatioMap and calc average mass of a target.
   const std::map<int, double> & matratio = 
                     earth->GetPNERatioMap(medtype); 

   // mass of a modified isoscalar particle
   double m_ave = 0;
   std::map<int, double>::const_iterator mi;
   for (mi = matratio.begin(); mi != matratio.end(); ++mi) {
      double ratio = mi->second;
      switch (mi->first) {
         case 2212 :
           m_ave += ratio * Constants::PMASS; // proton mass in g 
           break;
         case 2112 :
           m_ave += ratio * Constants::NMASS; // neutron mass in g 
           break;
         case 11 :
           m_ave += ratio * Constants::EMASS; // electron mass in g 
           break;
      } 
   }

   // get drivers map for given particle type
   if (drivers_.count(ptype) != 1) {
      log_error("Drivers for particle type %d does not exist!", ptype);
   }
   const IntTypeDrivers &intd = drivers_[ptype];


   // clear InteractionBufMaps
   intbufmap_.clear(); 
   intbufmap_scaled_.clear(); 
   double totXsec = 0;
   double totXsec_scaled = 0;
   double totXsec_per_gram = 0;
   double totXsec_per_gram_scaled = 0;

   // start interaction type (CC/NC/GR/DECAY) loop
   IntTypeDrivers::const_iterator inti;
   for (inti = intd.begin(); inti != intd.end(); ++inti) {
      InteractionType inttype = inti->first;
      double intfactor = Utils::GetMapValue(intfactors, inttype);
      double globalfactor = Utils::GetMapValue(globalfactors, inttype);
            
      // start material type loop (Proton, Neutron, Electron)
      const MatDrivers & matd = inti->second;
      MatDrivers::const_iterator mati; 
      for (mati = matd.begin(); mati != matd.end(); ++mati) {

         // material pid
         int matpid = mati->first;
         InteractionBasePtr interaction = mati->second;
         double curxsec = interaction->GetXsecCGS(energy);

         // xsec (applied globalfactor)
         double xsec = globalfactor * curxsec;

         // xsec_per_gram [cm2/g] = xsec[cm2] * ratio / m_ave[g]
         // get count ratio of this material in unit volume
         double mat_ratio = earth->GetPNERatio(medtype, matpid);
         double xsec_per_gram = xsec * mat_ratio / m_ave;

         // scaled parameters
         double xsec_scaled = xsec * intfactor;
         double xsec_per_gram_scaled = xsec_per_gram * intfactor;

         InteractionBuf intbuf;
          
         if (xsec > 0) {
            totXsec += xsec; 
            totXsec_scaled += xsec_scaled; 
            totXsec_per_gram += xsec_per_gram; 
            totXsec_per_gram_scaled += xsec_per_gram_scaled; 

            intbuf.interaction_ = interaction;
            intbuf.xsec_ = xsec;
            intbuf.xsec_scaled_ = xsec_scaled;
            intbuf.xsec_per_gram_ = xsec_per_gram;
            intbuf.xsec_per_gram_scaled_ = xsec_per_gram_scaled;
            intbuf.xsec_scaled_ = xsec_scaled;
            intbuf.sum_xsec_ = totXsec;
            intbuf.sum_xsec_scaled_ = totXsec_scaled;
            intbuf.sum_xsec_per_gram_ = totXsec_per_gram;
            intbuf.sum_xsec_per_gram_scaled_ = totXsec_per_gram_scaled;
            // scale_weight_ is 1.0 for intbuf, but will be updated
            // for intbuf_scaled_ later.
            intbuf.scale_weight_ = 1.0; 
            intbufmap_[totXsec_per_gram] = intbuf; 

            // store scaled map
            if (xsec_per_gram_scaled > 0) {
               intbufmap_scaled_[totXsec_per_gram_scaled] = intbuf; 
            }
         }
      } 
   }

   // calculate & update interaction weights for weighted map

   InteractionBufMap::iterator i;
   for (i = intbufmap_scaled_.begin();
        i != intbufmap_scaled_.end(); ++i) {
      // must be a reference to UPDATE update a weight
      InteractionBuf & buf = i->second;
      double ratio_orig = buf.xsec_per_gram_ / totXsec_per_gram;
      double ratio_scaled = buf.xsec_per_gram_scaled_ / totXsec_per_gram_scaled;
      double weight = ratio_orig / ratio_scaled;
      buf.scale_weight_ = weight;
   }
}

}

typedef I3SingleServiceFactory<nugen::NuGInteractionManager>
I3NuGInteractionManager;
I3_SERVICE_FACTORY(I3NuGInteractionManager);

