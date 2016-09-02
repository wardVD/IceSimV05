/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file InteractionInfo.h
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *   @brief Stores interaction geom info, used by Particle class
 */
#ifndef NuGInteractionGeo_H
#define NuGInteractionGeo_H

#include "dataclasses/I3Position.h"
#include "dataclasses/I3Map.h"
#include "neutrino-generator/utils/Utils.h"
#include <string>

namespace nugen {

class Particle;

class InteractionGeo {

  public:

  friend class Particle;

  InteractionGeo() : injectionArea_(-1),
                     impactParam_(-1), 
                     mod_detlen_before_(-1),
                     mod_detlen_after_(-1), 
                     range_m_(-1), range_mwe_(-1) 
                     {}
  virtual ~InteractionGeo(){}

  void FillInteractionGeo(I3MapStringDoublePtr wmap) const
  {
     // do NOT fill InjectionArea, because it's actually not
     // an "interaction" geom. This value should be filled by
     // primary generator.
     (*wmap)["InjectionAreaCGS"] = injectionArea_ / I3Units::cm2;
     (*wmap)["ImpactParam"]  = impactParam_;
     (*wmap)["RangeInMeter"]  = range_m_;
     (*wmap)["RangeInMWE"]  = range_mwe_;
     (*wmap)["TrueActiveLengthBefore"]  = mod_detlen_before_;
     (*wmap)["TrueActiveLengthAfter"]  = mod_detlen_after_;
  }

  /**
   * @brief copy interaction geo info from MCWeightDict
   */
  void RestoreInteractionGeo(I3MapStringDoubleConstPtr wmap) 
  {
     double injectionAreaCGS = -1;
     Utils::GetMapValue(*wmap, std::string("InjectionAreaCGS"), injectionAreaCGS);
     // convert CGS unit to SI unit
     injectionArea_ = injectionAreaCGS * I3Units::cm2;

     Utils::GetMapValue(*wmap, std::string("ImpactParam"), impactParam_);
     Utils::GetMapValue(*wmap, std::string("RangeInMeter"), range_m_);
     Utils::GetMapValue(*wmap, std::string("RangeInMWE"), range_mwe_);
     Utils::GetMapValue(*wmap, std::string("TrueActiveLengthBefore"), mod_detlen_before_);
     Utils::GetMapValue(*wmap, std::string("TrueActiveLengthAfter"), mod_detlen_after_);
  }

  /**
   * @brief Check whether a position is within detection volume.
   * Do not forget to call InteractionInfo::CalculateInteractionGeometry()
   * before this function is called! 
   */
  bool IsInside(const I3Position &p) const 
  {
     // check distance from p to impact pos.
     // (impact pos = closest approach from icecube center 
     //  to a track)
     I3Position toClosestPos(closestDetVolDC_ - p); 
     double dist = toClosestPos.Magnitude();
     if (dist < 0.01*I3Units::m) { 
        // avoid too small dist so that isBefore gives always
        // reasonable value. dist < 0.01[m] should be safe to 
        // say "IsInside = True", because impact position is
        // inside the detection volume by definition.
        return true; 
     }
     I3Direction voldir(exitDetVolDC_ - enterDetVolDC_);
     bool isBefore = toClosestPos * voldir > 0. ? true : false;
     if (isBefore) {
        return (dist < mod_detlen_before_);
     } else {
        return (dist < mod_detlen_after_);
     }
  }

  /**
   * @brief get maximum detection cylinder length
   */
  double GetMaxLength()
  {
     return mod_detlen_before_ + mod_detlen_after_;
  }

  // getters
  const double GetInjectionArea()    const { return injectionArea_; }
  const double GetImpactParam()      const { return impactParam_; }
  const double GetModDetLenBefore()  const { return mod_detlen_before_; }
  const double GetModDetLenAfter()   const { return mod_detlen_after_; }
  const double GetLeptonRangeMeter() const { return range_m_; }
  const double GetLeptonRangeMWE()   const { return range_mwe_; }
  const I3Position &GetImpactPosDC()   const { return closestDetVolDC_; }
  const I3Position &GetEnterDetVolDC() const { return enterDetVolDC_; }
  const I3Position &GetExitDetVolDC()  const { return exitDetVolDC_; }

  // setters -- be careful to use these setters!
  void SetEnterDetVolDC(const I3Position &v) { enterDetVolDC_ = v; }
  void SetExitDetVolDC(const I3Position &v)  { exitDetVolDC_ = v; }
  void SetInjectionArea(double s)  { injectionArea_ = s; }
  void SetImpactParam(double s)    { impactParam_ = s; }
  void SetLeptonRangeMeter(double l)  { range_m_ = l; }
  void SetLeptonRangeMWE(double l)  { range_mwe_ = l; }

  private:

  /**
   * @brief injection area[m^2] for this particle
   */
  double injectionArea_;

  /**
   * @brief impact parameter of the particle
   */
  double impactParam_;

  /**
   * @brief distance from the closest approach to the front 
   * surface of the detection volume after muon range extension
   */
  double mod_detlen_before_; 

  /**
   * @brief distance from the closest approach to the end
   * surface of the detection volume, in case of the
   * end position is clipped with Earth's surface
   */
  double mod_detlen_after_; 

  /**
   * @brief maximum range of the particle (if mu or tau) in [m]
   */
  double range_m_;

  /**
   * @brief maximum range of the particle (if mu or tau) in 
   * [meter water equiv.]
   */
  double range_mwe_;

  /**
   * @brief closest position of a track to IceCube center in 
   * Detector Coordinate (impact position)
   */
  I3Position closestDetVolDC_; 

  /**
   * @brief enter of detection volume in Detector Coordinate
   * if the perticle position is within detection volume,
   * it is identical to particle position 
   */
  I3Position enterDetVolDC_;  

  /**
   * @brief exit of detection volume in Detector Coordinate
   */
  I3Position exitDetVolDC_;   

  SET_LOGGER("I3NuG");

};

I3_POINTER_TYPEDEFS(InteractionGeo);

}

#endif 
