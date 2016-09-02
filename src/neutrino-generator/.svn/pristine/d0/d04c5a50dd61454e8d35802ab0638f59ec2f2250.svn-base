#ifndef I3NuGINTERACTIONINFO_H
#define I3NuGINTERACTIONINFO_H
/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file I3NuGInteractionInfo.h
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief I3NuGInteractionInfo header file
 *   Total Crosssection header file for I3NeutrinoGenerator
 *   interface to interaction and decay classes 
 *   sigmatotal is the sum of all crossections     
 */

#include "dataclasses/physics/I3Particle.h"
#include "earthmodel-service/EarthModelService.h"
#include "phys-services/I3RandomService.h"
#include "icetray/I3ServiceBase.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"
#include <vector>
#include <map>

////////////////////////////////////////////////////////////////

namespace nugen {
   I3_FORWARD_DECLARATION(Steering); 
   I3_FORWARD_DECLARATION(Particle); 
   I3_FORWARD_DECLARATION(InteractionBase); 
   I3_FORWARD_DECLARATION(TauDecay); 
}

class I3NuGInteractionInfo : public I3ServiceBase {

 public:

 /**
   * enums
   */
  enum NuGMaterialConstitute{PROTON=2212, NEUTRON=2112, ELECTRON=11};
  enum NuGPropagatingMedium{ICE=0, ROCK=1, UNDEF=2};

  /**
   * constructor & destructor
   */
  I3NuGInteractionInfo(const I3Context &c);
  I3NuGInteractionInfo(I3RandomServicePtr random,
                       boost::shared_ptr<nugen::Steering> steer,
                       const std::string &xsec_name);
  //I3NuGInteractionInfo(const I3NuGInteractionInfo &c);

  virtual ~I3NuGInteractionInfo();

  /**
   * configure
   */
  virtual void Configure();

  /**
   * initialize
   */
  void Initialize();


  /**
   * get total crosssection
   * ------------------------
   * Dec 14 2010 K.Hoshina
   * added scale parameters and weights 
   * for selecting a *FINAL* interaction.
   * Leave them as default (=1) for propagation.
   * If you want to scale cross sections, use
   * I3NuGStaticParams::gCC_SCALE_FACTOR etc.
   * See comments in source code.
   * Jun 16 2011 
   * added public method and made old functions private
   */

 public:

  double GetTotalCrosssection(boost::shared_ptr<nugen::Particle> eve_ptr);
  double GetTotalCrosssection(boost::shared_ptr<nugen::Particle> eve_ptr, 
                        const I3Position & posCE);

 private:
  double GetTotalCrosssection(boost::shared_ptr<nugen::Particle> eve_ptr, 
                        double ccfactor, 
                        double ncfactor, 
                        double grfactor,
                        double *ccweight=0, 
                        double *ncweight=0, 
                        double *grweight=0);

  double GetTotalCrosssection(boost::shared_ptr<nugen::Particle> eve_ptr, 
                        const I3Position & posCE,
                        double ccfactor, 
                        double ncfactor, 
                        double grfactor,
                        double *ccweight=0, 
                        double *ncweight=0, 
                        double *grweight=0);

 public:

  // helper function for drawing xsec files
  double GetXsecCGS(std::string inttype, double energy);


  /**
   * MakeInteraction choose one interaction among all candidate
   * interactions and fill daughter particles
   * ------------------------
   * Dec 14 2010 K.Hoshina
   * added scale parameters for selecting a FINAL interactions.
   * set 0 to deactivate an interaction.
   * See comments in source code.
   * argument n is not used anymore in the program then removed
   */
  nugen::InteractionType MakeInteraction(boost::shared_ptr<nugen::Particle> particle_ptr, 
                        double ccfactor=1, 
                        double ncfactor=1, 
                        double grfactor=1);

  /**
   * MakeDecay treats tau decay and fill daughter particles
   * argument n is not used anymore in the program then removed
   */
  bool   MakeDecay(boost::shared_ptr<nugen::Particle> particle_ptr);

  /**
   * print function for Interaction info
   */
  void ViewInteractionInfo();

  /**
   * calculate total columndepth from start to exit
   * If maxstepsize is greater than 0, the step size is
   * limited to less equal the maxstepsize.
   * Default maxstepsize = -1 (no limit). 
   * If boost::shared_ptr<nugen::Particle> is not NULL, it also calculates
   * sum of (crosssection * column_depth_dl)
   * ccfactor, ncfactor and grfactor are scaling factors of 
   * cc/nc/gr interactions.
   * default is 1.
   */
  const double CalculateTotalColumnDepth(const I3Position& start, 
                                         const I3Position& exit, 
                                         int    nsteps = 1000,
                                         double maxstepsize = -1,
                                         boost::shared_ptr<nugen::Particle> p_ptr = boost::shared_ptr<nugen::Particle>(),
                                         double ccfactor = 1,
                                         double ncfactor = 1,
                                         double grfactor = 1);

  /**
   * convert columndepth [g/m2] to length [m]
   *
   * !!! CAUTION !!!
   * To use the function, you MUST call CalculateTotalColumnDepth 
   * in advance.
   * It returns pre-calculated result.
   * The resolution of returned length depends on how you 
   * called CalculateTotalColumnDepth.
   * If you need to limit the resolution within 10m, for example,
   * you must specify maxstepsize = 10[m] .
   */
  const double ConvertColumnDepthToMeter(double columndep);


  /**
   * convert length [m] to columndepth [g/m2]
   *
   * !!! CAUTION !!!
   * To use the function, you MUST call CalculateTotalColumnDepth 
   * in advance.
   * It returns pre-calculated result.
   * The resolution of returned columndepth depends on how you 
   * called CalculateTotalColumnDepth.
   * If you need to limit the resolution within 10m, for example,
   * you must specify maxstepsize = 10[m] .
   */
  const double ConvertMeterToColumnDepth(double length_meter);


  /**
   * get (total crosssection * columndepth) at given columndepth[g/m2]
   *
   * !!! CAUTION !!!
   * To use the function, you MUST call CalculateTotalColumnDepth 
   * in advance.
   * It returns pre-calculated result.
   * The resolution of returned length depends on how you 
   * called CalculateTotalColumnDepth.
   * If you need 10m resolution, for example,
   * you must specify maxstepsize = 10[m] .
   */
  const double GetCrosssectionColumndepthAt(double columndepth);


  /**
   * get function for columndepth vs length maps. columndepth is in [g/m2]
   */
  std::map<double,double> GetLengthVsColumndepthMap() const { return length_vs_columndepth_map_;} 
  std::map<double,double> GetColumndepthVsLengthMap() const { return columndepth_vs_length_map_;}

  /**
   * setters
   */

  void SetRandomService(I3RandomServicePtr r) { random_ = r; }

  void SetMaterialRock(double pr, double nr, double er){protonrock_=pr, neutronrock_=nr,electronrock_=er;};
  void SetMaterialH2O(double ph, double nh, double eh) {protonh2o_ =ph, neutronh2o_ =nh,electronh2o_ =eh;};

 private:

  /**
   * read files
   */
  std::map<std::string, std::string> xsecs_;
  void   ReadInteractionFiles();
  void   ReadDecayStateFiles();


  /**
   * pointer to services and steering
   */
  I3RandomServicePtr    random_;
  std::string steeringName_;
  boost::shared_ptr<nugen::Steering> steer_;
  earthmodel::EarthModelServicePtr earth_;

  /**
   *  Location of the cross section tables.
   *  Default value is $I3_BUILD/neutrino-generator/resources/cross_section_data/
   */
  std::string tablesDir_;

  /**
   * interactionVect_ stores active interactions
   */
  std::vector<boost::shared_ptr<nugen::InteractionBase> >  interactionVect_;

  // this is for helper function...
  std::map<std::string, boost::shared_ptr<nugen::InteractionBase> >  interactionMap_;

  /**
   * Pointer to Decay Final State table
   */
  boost::shared_ptr<nugen::TauDecay>  tau_decay_ptr_;

  /**
   * following parameters are temporary buffers
   * to keep calculation of total cross section.
   * crosssection_map_ stores accumulated cross section as key,
   * pointer to an interaction as value
   */

  class PrevStateCrossSection {
  
    public :

     PrevStateCrossSection(
           NuGPropagatingMedium med = UNDEF,
           I3Particle::ParticleType ptype = I3Particle::unknown,
           double ene = -1,
           double ccf = -1,
           double ncf = -1,
           double grf = -1,
           double ccw = -1,
           double ncw = -1,
           double grw = -1,
           bool use_prev = false)
        :  medium_(med), 
           ptype_(ptype),
           energy_(ene), ccfactor_(ccf),
           ncfactor_(ncf), grfactor_(grf),
           ccweight_(ccw), ncweight_(ncw),
           grweight_(grw), 
           use_prev_interaction_vect_(use_prev){}

     bool UsePrev(const PrevStateCrossSection &cur)
     {
        if (medium_ == cur.medium_ &&
            ptype_ == cur.ptype_ &&
            energy_ == cur.energy_ &&
            ccfactor_ == cur.ccfactor_ &&
            ncfactor_ == cur.ncfactor_ &&
            grfactor_ == cur.grfactor_ &&
            use_prev_interaction_vect_ ) return true;
        return false;
     }

     NuGPropagatingMedium     medium_;
     I3Particle::ParticleType ptype_;
     double energy_;
     double ccfactor_;
     double ncfactor_;
     double grfactor_;
     double ccweight_;
     double ncweight_;
     double grweight_;
     bool   use_prev_interaction_vect_;
  }; 

  PrevStateCrossSection prev_state_crosssec_;
  std::map<double,nugen::InteractionBasePtr> crosssection_map_; 


  /**
   * following parameters are temporary buffers
   * to keep calculation of total column depth.
   * columndepth_map_ stores length from the start point as key,
   * accumulated columndepth as value.
   * crosssec_columndepth_map_ stores accumulated columndepth as key,
   * (crosssection * columndepth) as value.
   */
  class PrevStateColumnDepth {

    public :

     PrevStateColumnDepth(
           const I3Position &entCE = I3Position(1e20, 1e20, 1e20),
           const I3Position &extCE = I3Position(1e20, 1e20, 1e20),
           int    nsteps = -1,
           double maxstep = -1,
           boost::shared_ptr<nugen::Particle> p = boost::shared_ptr<nugen::Particle>(),
           double ccf = -1,
           double ncf = -1,
           double grf = -1);

     bool UsePrev(PrevStateColumnDepth &cur)
     {
        if (enterCE_ != cur.enterCE_ ||
            exitCE_ != cur.exitCE_ || 
            nsteps_ != cur.nsteps_ ||
            maxstepsize_ != cur.maxstepsize_ ||
            particleptr_ != cur.particleptr_) return false;

        // if particleptr is not set, you can use
        // previous calculation!
        if (!cur.particleptr_) {
           // if we use prev calculation, 
           // we must copy prev setting to cur.
           cur = *this;
           return true;
        }

        // particleptr is set.
        // CalculateTotalColumnDepth function will
        // calculate product of crosssection*cdep.
        // now you have to check energy.
        if (energy_ != cur.energy_ ||
            ccfactor_ != cur.ccfactor_ || 
            ncfactor_ != cur.ncfactor_ || 
            grfactor_ != cur.grfactor_) return false;

        // everything are same, use prev calculation
        return true;
     }

     I3Position       enterCE_;
     I3Position       exitCE_;
     int              nsteps_;
     double           maxstepsize_;
     boost::shared_ptr<nugen::Particle> particleptr_;
     double           energy_;
     double           ccfactor_;
     double           ncfactor_;
     double           grfactor_;
  };

  PrevStateColumnDepth prev_state_columndep_;
  std::map<double,double> length_vs_columndepth_map_; 
  std::map<double,double> columndepth_vs_length_map_; 
  std::map<double,double> crosssec_columndepth_map_; 
      
  /**
   * names of interaction files
   */
  std::string xsec_model_;
  unsigned int flavorMaskCCNu_,    materialMaskCCNu_;
  unsigned int flavorMaskCCNuBar_, materialMaskCCNuBar_;
  unsigned int flavorMaskNCNu_,    materialMaskNCNu_;
  unsigned int flavorMaskNCNuBar_, materialMaskNCNuBar_;
  unsigned int flavorMaskGRNuBar_, materialMaskGRNuBar_;
  double protonrock_,   protonh2o_;
  double neutronrock_,  neutronh2o_;
  double electronrock_, electronh2o_;
  double global_cc_scales_;
  double global_nc_scales_;
  double global_gr_scales_;

  void SetTauDecayPtr(nugen::TauDecayPtr ptr){tau_decay_ptr_ =ptr;}
  nugen::TauDecayPtr GetTauDecayPtr(){return tau_decay_ptr_;}

  //bool GetActiveFlavor(); the function is not defined!

  /**
   * @return density of specified medium & material
   */
  double GetDensityRatioForIsoscalarCrossSection(
      NuGPropagatingMedium material, NuGMaterialConstitute mask);
  
  /**
   * @return energy loss coeficient of tau
   */
  double GetTauBeta(double energy);
  
  /**
   * check function whether the flavor is active
   */
  bool IsFlavorActive(const unsigned int, const I3Particle::ParticleType);
  /**
   * check function whether the material is active
   */
  bool IsMaterialActive(const unsigned int, const I3NuGInteractionInfo::NuGMaterialConstitute);


  I3NuGInteractionInfo() : I3ServiceBase("I3NuGInteractionInfo") {}

  SET_LOGGER("I3NuG"); 
};

////////////////////////////////////////////////////////////////
typedef boost::shared_ptr<I3NuGInteractionInfo> I3NuGInteractionInfoPtr;

#endif //I3NuGINTERACTIONINFO_h









