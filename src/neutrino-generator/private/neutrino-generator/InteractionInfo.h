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
 *   @brief Stores interaction info, used by Particle Class
 */
#ifndef NuGInteractionInfo_H
#define NuGInteractionInfo_H

#include "neutrino-generator/utils/EnumTypeDefs.h"
#include "neutrino-generator/utils/Utils.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/I3Map.h"
#include <string>

namespace nugen {

class Particle;

class InteractionInfo {
  
 public:

  friend class Particle;

  InteractionInfo() : vertex_(0,0,0), vertexTime_(-1),
                      columnDepthCGS_(-1), lengthInVolume_(-1),
                      crosssectionCGS_(-1),
                      intType_(UNDEFINED),
                      total_crosssectionCGS_(-1),
                      bjorken_x_(-1),
                      bjorken_y_(-1),
                      total_column_depthCGS_(-1),
                      energyloss_(-1),
                      typeweight_(0.5), // 0.5 is default for I3NuGInjector. 
                      selectionweight_(1.0), 
                      posweight_(1.0), 
                      angweight_(1.0),
                      eneweight_(1.0), propweight_(1.0),
                      intposweight_(1.0), intweight_(1.0){}
  virtual ~InteractionInfo(){}

  /**
   * @brief copy weights from other intereactionInfo
   */
  void SetWeights(const InteractionInfo &intInfo) 
  {
     typeweight_ = intInfo.typeweight_; 
     selectionweight_ = intInfo.selectionweight_; 
     angweight_ = intInfo.angweight_; 
     posweight_ = intInfo.posweight_; 
     eneweight_ = intInfo.eneweight_; 
     propweight_ = intInfo.propweight_; 
     intposweight_ = intInfo.intposweight_; 
     intweight_ = intInfo.intweight_; 
  }

  /**
   * @brief copy weights to MCWeightDict
   */
  void FillWeights(I3MapStringDoublePtr wmap) const
  {
     (*wmap)["TypeWeight"]  = typeweight_;
     (*wmap)["SelectionWeight"]  = selectionweight_;
     (*wmap)["DirectionWeight"]  = angweight_;
     // for now they are not used so that I don't save them
     //(*wmap)["PositionWeight"]  = posweight_;
     //(*wmap)["EnergyWeight"]  = eneweight_;
     (*wmap)["TotalPrimaryWeight"] = GetTotalPrimaryWeight();
     (*wmap)["PropagationWeight"]  = propweight_;
     (*wmap)["InteractionPositionWeight"]  = intposweight_;
     (*wmap)["InteractionWeight"]  = intweight_;
     (*wmap)["TotalWeight"]  = GetTotalWeight();
  }

  /**
   * @brief copy interaction info to MCWeightDict
   */
  void FillInteractionInfo(I3MapStringDoublePtr wmap) const
  {
     FillWeights(wmap);
     (*wmap)["InteractionType"]        = intType_;
     (*wmap)["TotalXsectionCGS"]       = total_crosssectionCGS_;
     (*wmap)["InteractionXsectionCGS"] = crosssectionCGS_;
     (*wmap)["BjorkenX"]              = bjorken_x_;
     (*wmap)["BjorkenY"]              = bjorken_y_;
     (*wmap)["TotalColumnDepthCGS"]    = total_column_depthCGS_;
     (*wmap)["InteractionColumnDepthCGS"]  = columnDepthCGS_;
     (*wmap)["LengthInVolume"]         = lengthInVolume_;
     (*wmap)["EnergyLost"]             = energyloss_;
  }

  /**
   * @brief util function to get weight value from MCWeightDict
   */
  double GetWeightValue(I3MapStringDoubleConstPtr wmap, std::string key) 
  {
     const I3MapStringDouble::const_iterator i = wmap->find(key);
     if (i == wmap->end()) return 1.0;
     return i->second;
  }

  /**
   * @brief copy weight info from MCWeightDict
   */
  void RestoreWeights(I3MapStringDoubleConstPtr wmap) 
  {
     typeweight_ = GetWeightValue(wmap,"TypeWeight");
     selectionweight_ = GetWeightValue(wmap,"SelectionWeight");
     angweight_ = GetWeightValue(wmap,"DirectionWeight");
     // for now they are not used so that I don't save them
     //posweight_ = GetWeightValue(wmap,"PositionWeight");
     //eneweight_ = GetWeightValue(wmap,"EnergyWeight");
     propweight_ = GetWeightValue(wmap, "PropagationWeight");
     intposweight_ = GetWeightValue(wmap, "InteractionPositionWeight");
     intweight_ = GetWeightValue(wmap, "InteractionWeight");
  }

  /**
   * @brief copy interaction info from MCWeightDict
   */
  void RestoreInteractionInfo(I3MapStringDoubleConstPtr wmap) 
  {
     RestoreWeights(wmap);

     double inttype = -1;
     Utils::GetMapValue(*wmap, std::string("InteractionType"), inttype);
     intType_ = (InteractionType)inttype;

     Utils::GetMapValue(*wmap, std::string("TotalXsectionCGS"), total_crosssectionCGS_);
     Utils::GetMapValue(*wmap, std::string("InteractionXsectionCGS"), crosssectionCGS_);
     Utils::GetMapValue(*wmap, std::string("BjorkenX"), bjorken_x_);
     Utils::GetMapValue(*wmap, std::string("BjorkenY"), bjorken_y_);
     Utils::GetMapValue(*wmap, std::string("TotalColumnDepthCGS"), total_column_depthCGS_);
     Utils::GetMapValue(*wmap, std::string("InteractionColumnDepthCGS"), columnDepthCGS_);
     Utils::GetMapValue(*wmap, std::string("LengthInVolume"), lengthInVolume_);
  }

  /**
   * @brief calculate total weight
   */
  const double GetTotalPrimaryWeight() const
  {
     // the following equation should be true, but in order to 
     // keep consistency of definition of OneWeight, 
     // we omit typeweight from primary weight. 
     //return typeweight_ * selectionweight_ * angweight_ * posweight_ * eneweight_;
     return selectionweight_ * angweight_ * posweight_ * eneweight_;
  }

  const double GetTotalWeight() const
  {
     return GetTotalPrimaryWeight() * propweight_ * intposweight_ * intweight_;
  }

  // getters and setters

  inline const I3Position & GetVertex() const {return vertex_;}
  inline const double GetVertexTime() const {return vertexTime_;}
  inline const InteractionType GetInteractionType() const {return intType_;}
  inline const double GetInteractionColumnDepthCGS()const {return columnDepthCGS_;}
  inline const double GetLengthInVolume()           const {return lengthInVolume_;}
  inline const double GetInteractionXsecCGS()       const {return crosssectionCGS_;}
  inline const double GetTotalXsecCGS()             const {return total_crosssectionCGS_;}
  inline const double GetBjorkenX()                 const {return bjorken_x_;}
  inline const double GetBjorkenY()                 const {return bjorken_y_;}
  inline const double GetTotalColumnDepthCGS()      const {return total_column_depthCGS_;}
  inline const double GetEnergyLost()               const {return energyloss_;}
  inline const double GetTypeWeight()               const {return typeweight_;}
  inline const double GetPrimarySelectionWeight()   const {return selectionweight_;}
  inline const double GetPositionWeight()      const {return posweight_;}
  inline const double GetDirectionWeight()     const {return angweight_;}
  inline const double GetEnergyWeight()        const {return eneweight_;}
  inline const double GetPropagationWeight()   const {return propweight_;}
  inline const double GetInteractionPositionWeight() const {return intposweight_;}
  inline const double GetInteractionWeight()   const {return intweight_;}

  inline void SetVertex(const I3Position &x) {vertex_ = x;}
  inline void SetVertexTime(double t)        {vertexTime_ = t;}
  inline void SetInteractionType(InteractionType t){intType_ = t;}
  inline void SetInteractionColumnDepthCGS(double c)  {columnDepthCGS_ = c;}
  inline void SetLengthInVolume(double l)    {lengthInVolume_ = l;}
  inline void SetInteractionXsecCGS(double c){crosssectionCGS_ = c;}
  inline void SetBjorkenX(double c)          {bjorken_x_ = c;}
  inline void SetBjorkenY(double c)          {bjorken_y_ = c;}
  inline void SetTotalXsecCGS(double c)      {total_crosssectionCGS_ = c;}
  inline void SetTotalColumnDepthCGS(double c){total_column_depthCGS_  = c;}
  inline void SetEnergyLost(double ene)      {energyloss_ = ene;}
  inline void SetTypeWeight(double w)        {typeweight_ = w;}
  inline void SetPrimarySelectionWeight(double w) {selectionweight_= w;}
  inline void SetPositionWeight(double w)    {posweight_ = w;}
  inline void SetDirectionWeight(double w)   {angweight_ = w;}
  inline void SetEnergyWeight(double w)      {eneweight_ = w;}
  inline void SetPropagationWeight(double w) {propweight_ = w;}
  inline void SetInteractionPositionWeight(double w) {intposweight_ = w;}
  inline void SetInteractionWeight(double w) {intweight_ = w;}

 private:

  /**
   * @brief interaction position (vertex)
   */
  I3Position vertex_;

  /**
   * @brief interaction time
   */
  double vertexTime_;

  /**
   * @brief Column depth in [mwe]
   * from front edge of detection volume to interaction vertex 
   */
  double columnDepthCGS_;

  /**
   * @brief length from front edge of detection volume to 
   * interaction veretex 
   */
  double lengthInVolume_;

  /**
   * @brief crosssection for this interaction type at vertex
   * Xsec in log_10(sigma[mb])
   */
  double crosssectionCGS_;

  /**
   * @brief interaction type, 0 for decay
   */
  InteractionType intType_;

  /**
   * @brief total crosssection to calculate interaction probability
   */
  double total_crosssectionCGS_;

  /**
   * @brief selected final state, Bjorken x-value
   */
  double bjorken_x_;

  /**
   * @brief selected final state, Bjorken y-value
   */
  double bjorken_y_;

  /**
   * @brief total column depth (w.m.e) within detection volume
   */
  double total_column_depthCGS_;

  /**
   * @brief energy loss 
   */
  double energyloss_;

  /**
   * @brief particle type weight.
   * ratio of this particle's type to all injected neutrino.
   * If the the typeweight is 0.2 and the particle is NuMuBar,
   * that means 25% of injected neutrinos is NuMuBar.
   */
  double typeweight_;

  /**
   * @brief particle selection weight
   */
  double selectionweight_;

  /**
   * @brief generation position weight.
   * Default value should be 1.
   */
  double posweight_;

  /**
   * @brief directional (zenith and azimuth) weight.
   * Default value should be 1.
   */
  double angweight_;

  /**
   * @brief energy weight.
   * See SourceUtils::CalcEnergyFactor()
   */
  double eneweight_;

  /**
   * @brief for weighted propagation (e.g. forbid CC etc.)
   */
  double propweight_;

  /**
   * @brief interaction position weight
   */
  double intposweight_;

  /**
   * @brief interaction weight
   */
  double intweight_;

  SET_LOGGER("I3NuG");

};

I3_POINTER_TYPEDEFS(InteractionInfo);

}

#endif 
