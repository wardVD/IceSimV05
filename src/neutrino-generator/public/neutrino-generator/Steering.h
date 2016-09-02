/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file Steering.h
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *   @brief steering information to run nugen
 *    Now Steering class keeps only run-base info thus
 *    it is singleton.
 *    There is no setter function because only an interface 
 *    friend class should be able to change all parameter.
 *    Modified from Aya Ishihara's original version in 2013.
 */

#ifndef NuGSTEERING_H
#define NuGSTEERING_H

#include "icetray/I3ServiceBase.h"
#include "icetray/I3SingleServiceFactory.h"
#include "dataclasses/I3Map.h"
#include "earthmodel-service/EarthModelService.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"
#include "phys-services/surfaces/SamplingSurface.h"

namespace nugen {

I3_FORWARD_DECLARATION(InteractionManagerBase); 
I3_FORWARD_DECLARATION(Particle); 

class Steering : public I3ServiceBase {

 public:

  /**
   * @brief default constructor is for I3Service only
   */
  Steering(const I3Context &c);


  /**
   * @brief constructors for pybindings
   */
  Steering(earthmodel::EarthModelServicePtr earth);

  Steering(earthmodel::EarthModelServicePtr earth, 
           SimMode simMode,
           VTXGenMode vtxGenMode,
           InjectionMode injectionType);

  /**
   * @brief destructor
   */
  virtual ~Steering();

  //static boost::shared_ptr<Steering> GenerateInstance();

  /**
   * configure
   */
  void Configure();

  /**
   * @brief Checker if all the steering values are correctly filled
   * @todo fill it...
   */
  void ViewSteering() const {}

  /**
   * @brief number of total event generated
   */
  inline const int GetNGen() const { return numberOfEvents_; }

  /**
   * @brief a distance which separates a full neutrino track trajectory
   * into "near detector area" and "in Earth propagation area".
   *
   * This radius must be larger than most longest possible track
   * range in your simulation.
   * For example, if you simulate Tau up to 10^9 GeV, the maximum
   * range of the tau + mu(in meter, in ice medium) will be 
   * 155574[m]. MaxDetVolumLen must be larger than the value.
   */
  inline const double GetMaxDetVolumeLen() const 
  { return max_detection_volume_len_; } 

  /**
   * @brief Simulation area mode
   *  FULL - from EarthSurface to detector
   *  INEARTH - InEarth simulation only (ends at near_det_radius)
   *  NEARDET - Simulation starts from a distance near_det_radius_[m]
   *  FINALONLY - final interaction only
   */
  inline const SimMode GetSimMode() const { return simMode_; }

  /**
   * @brief VTX Generator switch
   *  NUGEN - use nugen (anis) final interaction tables.
   *  GENIE - use Genie to generate vertex and secondary particles
   *  HYBRID - use Genie for lowE neutrino, NuGen for highE neutrino
   */
  inline const VTXGenMode GetVTXGenMode() const { return vtxGenMode_; }

  /**
   * @brief energy threshold to switch NuGen and Genie
   * active only when HYBRID is chosen for VTXGenMode
   */
  inline const double VTXGenEThreshold() const { return vtxEThreshold_; }

  /**
   * @brief Benchmark Mode Flag
   *
   *  Sep.16 2013 this flag is still under construction. 
   * 
   *  OFF     - Not benchmark, physics run mode (default)
   *  DETEDGE - Interaction vtx is always on edge of MuonGun::Surface.
   *            Energy of secondary particle is always same as
   *            primary particle.
   *            If flavor is NuMu and NuTau, secondary cascades are
   *            not generated.
   *  INDET   - vtx is inside of the Surface area
   *            Energy of secondary particle is always same as
   *            primary particle.
   *            If flavor is NuMu and NuTau, secondary cascades are
   *            not generated.
   *  INDETPHYS - vtx is inside of the Surface area
   *            Secondary particles are generated with same process as
   *            physics run.
   */
  inline const BenchMode BenchmarkMode() const { return benchmarkMode_; }

  /**
   * @brief Name of EarthModelService
   */
  inline const std::string &GetEarthModelName() const { return earthname_; }

  /**
   * Name of MCTree 
   */
  inline const std::string &GetMCTreeName() const { return mctreename_; }

  /**
   * Name of weight dict
   */
  inline const std::string &GetWeightDictName() const { return weightname_; }

  /**
   * get world radius
   */
  inline const double GetWorldRadius() const { return worldRadius_; }

  /**
   * @brief InjectionMode.
   *  
   * CIRCLE : injects neutrino in circle area, same as legacy nugen
   *          Define circle radius with SetCylinderParams().
   * SURFACE : injects neutrino only if the neutrino path through
   *           the defined surface around IceCube.
   *           You may set sampling surface with SetDetectionSurface(),or
   *           define cylinder param via SetCylinderParams().
   * DYNAMIC : Same as SURFACE , but the surface size changes as
   *           energy increases. NOT IMPREMENTED YET.
   */
  inline const std::string &GetInjectionModeString() const { return injectionModeString_; }
  inline const InjectionMode GetInjectionMode() const { return injectionMode_; }
  inline I3Surfaces::SamplingSurfacePtr GetDetectionSurface() const { return detectionSurface_; }
  inline std::vector<double> GetCylinderParams() const { return cylinderParams_; }

  /**
   * @brief parameters for DetectionVolume for CIRCLE injection.
   * If you think icecube can detect photons from a cascade or track
   * 1200m before or behind of IceCube-center, set them 1200m.
   * Note that longest half-path in IceCube is ~ 750m in diagonal. 
   */
  const double GetInjectionRadius() const;
  const double GetActiveHeightBefore() const;
  const double GetActiveHeightAfter() const;

  /**
   * @brief parameters for DetectionVolume for SURFACE or DYNAMIC option.
   * By default NuGen doesn't know anything about surface,
   * but only when you set cylinderParams_ it knows the parameter.
   */
  const double GetCylinderRadius() const;
  const double GetCylinderHeight() const;
  const I3Position GetCylinderOrigin() const;

  /**
   * @brief options for muon range calculation
   * also used for column depth calculation
   */
  inline const bool   DoMuonRangeExtension() const { return doMuRangeExtension_; }
  inline const double GetMuonRangeOpt() const { return muRangeOpt_; }
  inline const double GetStepSize()     const { return stepsize_; }
  inline const double GetFineStepSize() const { return finestepsize_; }
  inline const double GetDensityTolerance() const { return densityTolerance_; }

  /**
   * Surviving probability is usually calculated with a formula
   * Psurv = exp(-n*sigma) 
   * where n = number of targets, sigma = cross section.
   * However, if n*sigma is small enough, it can be apploximated as 
   * Psurv = 1 - n*sigma.
   * This function gives which n*sigma we start to use the approx. form. 
   * Mainly used by Step::CalcPsurv function.
   */
  inline const double GetPsurvApproxLimit() const { return psurvApproxLimit_; }

  /**
   * @brief returns maximum length of detection volume.
   * Any muon generated farther than the distance should not reach to 
   * detector.
   */
  inline const double GetMaxDetVolLen() const { return max_detection_volume_len_; }

  /**
   * @return if we use simple form to calculate scattering angle
   */
  inline const  bool UseSimpleScatterForm() const { return  useSimpleScatterForm_; }

  inline const  bool IgnoreOutgoingAngleForNC() const { return  ignoreOutgoingAngleForNC_; }

  /**
   * @return global scale parameter for cross sections
   */
  inline const InteractionTypeDoubleMap & GetGlobalXsecScales() const 
  { return global_xsec_scales_; }
  
  /**
   * @return pointer to EarthModelService
   */
  earthmodel::EarthModelServicePtr GetEarthModelService() const;

  /**
   * @brief add interaction manager 
   */
  void AddInteractionManager(VTXGenMode mode, 
                  boost::shared_ptr<InteractionManagerBase> manager) ;

  /**
   * @brief get proper interaction manager
   */
  boost::shared_ptr<InteractionManagerBase> GetInteractionManager(
                                     const Particle &p);
  /**
   * @brief put special particle
   */
  void AddParticle(std::string pname, boost::shared_ptr<Particle> p);

  /**
   * @brief get special particle
   */
  boost::shared_ptr<Particle> GetParticle(std::string pname);

  /**
   * @brief check if parameters are reasonable
   */
   void CheckParameters();

  /**
   * @brief print out parameters.
   * you must set log_level to info to see it.
   */
   void PrintSetting();

   //
   // setters
   //

   void SetNGen(int n) { numberOfEvents_ = n; }
   void SetSimMode(SimMode m) { simMode_ = m; }
   void SetVTXGenMode(VTXGenMode m) { vtxGenMode_ = m; }
   void SetDoMuonRangeExtension(bool doit) { doMuRangeExtension_ = doit; }
   void SetWorldRadius(double r) { worldRadius_ = r; }
   void SetInjectionMode(InjectionMode m) { injectionMode_ = m; }
   void SetUseSimpleScatterForm(bool t) { useSimpleScatterForm_ = t; }
   void SetMuonRangeOpt(double t) { muRangeOpt_ = t; }

   
   /**
    * @brief set Detection Surface 
    */
   void SetDetectionSurface(I3Surfaces::SamplingSurfacePtr s)
        { detectionSurface_ = s; }

   /**
    * @brief set DetectionVolume params.
    * This function overwrites any setting you have done for Detection
    * Volume. For Surface or Dynamic injection, sampling Surface object
    * will be replaced.
    * See note for cylinderParams_ to get more info.
    */
   void SetCylinderParams(std::vector<double> s) 
        { cylinderParams_ = s; CheckParameters();}

 private :

  /**
   * @brief number of events generated
   */
  int numberOfEvents_;

  /**
   * @brief radius that separates in_earth propagation area and
   * near detector area ( = maximum detection volume length) 
   */
  double max_detection_volume_len_;

  // mode flags
  std::string simModeString_;
  SimMode     simMode_;
  std::string vtxGenModeString_;
  VTXGenMode  vtxGenMode_;
  double      vtxEThreshold_;
  std::string benchmarkModeString_;
  BenchMode   benchmarkMode_;

  // earth model name
  std::string earthname_;

  // mctree name, if source type is MCTREE
  std::string mctreename_;

  // name of weight dict
  std::string weightname_;

  /** 
   * size of radius for this simulation 
   * your detection volume must be within this radius 
   * from the center of the Earth.
   * If you dont' set it, nugen automatically sets IceAirBoundary in 
   * earthmodel-service as worldRadius.
   */ 
  double worldRadius_;

  // injection geom info
  std::string injectionModeString_;
  InjectionMode injectionMode_;

  // for general surface injection
  I3Surfaces::SamplingSurfacePtr detectionSurface_;

  /**
   * @brief cylinder params
   * size of vector is 5.
   * for CIRCLE injection mode:
   * param[0] : Injection Radius
   * param[1] : Active height before
   * param[2] : Active height after
   * for others:
   * param[0] : Cylinder Radius
   * param[1] : Cylinder Height 
   * param[2-4] : Cylinder origin (x, y, z)
   */
  std::vector<double> cylinderParams_;

  // MuonRange
  bool   doMuRangeExtension_;

  /**
   * @brief muRangeOpt_ is the option to calculate muon range
   * Don't touch default setting unless you know
   * well about the parameter.
   *
   * 0 : use GetMuonRange() [m.w.e] for extension (legacy)
   *
   * others : use GetMuonRangeInMeter() [m]
   *   CAUTION ---
   *   fmod(muRangeOpt_, 10) gives a scaling parameter 
   *   of the result of GetMuRangeInMeter()[m].
   *   Therefore, option 10, 20, etc gives range meter 0[m] !
   *   If you want to use Dima's original parameter with scaling factor 1,
   *   set 11 (11 is default value)
   *
   * 0 < opt < 10 : use GetMuonRangeInMeter() [m] 
   * with Dima's MuRange parameter.
   * (somehow they are inconsistent with Dima's internal report)
   *
   * 10 < opt < 20 : use GetMuonRangeInMeter() [m] 
   * with Dima's original MuRange parameter in internal report.
   *
   * 20 < opt < 30 : use GetMuonRangeInMeter() [m] 
   * with Gary's fitting parameter used in NuSim
   *
   * see Particle.cxx to check how the parameter is used.
   */
  double muRangeOpt_;

  /**
   * these parameters may be obsolate in future
   */
  double stepsize_;
  double finestepsize_;
  double densityTolerance_;

  /**
   * Global cross section scale
   * scale for cross section 
   */
  std::vector<double> global_xsec_scales_vec_;
  InteractionTypeDoubleMap global_xsec_scales_;

  /**
   * threshold used to calculation surviving probability
   */
  double psurvApproxLimit_;

  /**
   * flag if we use simple calculation for outgoing lepton angle
   * Affects NuGen only
   */
  bool useSimpleScatterForm_;

  /**
   * flag if we ignore outgoing angle for NC interaction
   * Affects NuGen only
   */
  bool ignoreOutgoingAngleForNC_;

  /**
   * @brief pointer to the EarthModelService
   */
  earthmodel::EarthModelServicePtr earth_ptr_;

  /**
   * @brief Map to InteractionManagerBase
   */
  std::map<VTXGenMode, boost::shared_ptr<InteractionManagerBase> > managers_;

  /**
   * @brief Map to special particles
   */
  std::map<std::string, boost::shared_ptr<Particle> > particles_;

 private :

  //logging
  SET_LOGGER("I3NuG");

};
I3_POINTER_TYPEDEFS(Steering);

}
#endif //I3NuGSTEERINGINFO_H
