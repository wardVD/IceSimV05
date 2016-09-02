/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3ParamTankResponse.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#ifndef _TOPSIMULATOR_I3PARAMTANKRESPONSE_H_
#define _TOPSIMULATOR_I3PARAMTANKRESPONSE_H_

#include <topsimulator/interface/I3TankResponse.h>
#include <dataclasses/physics/I3Particle.h>
#include <phys-services/I3RandomService.h>

class I3Configuration;

/**
 * \brief Generates a signal which is proportional to the 
 * extrapolared track length of a particle trajectory in the tank.

 * The conversion to VEM is done by parametrisations for each
 * particle type, see vem_yield.
 */

class I3ParamTankResponse : public I3TankResponse
{
  /**
   * \brief Mean charge per PE
   * 
   * The Constant used to be in <pmt-simulator/I3PMTConstants.h>,
   * which is now deprecated. Does it exist anywhere else now?
   *
   * This parameter needs to be updated whenever
   * the charge distribution constants change.
   * This should be caught in the tests.
   */
  static const double MEAN_NORMALIZED_PE;

  /// Perlite/Snow density ratio
  static const double PERL_SNOW_EQUIV;

public:
  I3ParamTankResponse(I3Configuration& config, const I3Context& context, const TankKey& tankKey);
  
  void Initialize(const I3Geometry& geometry,
		  const I3Calibration& calib,
		  const I3DetectorStatus& status);
  
  void Configure();
  
  
  /// X coordinate of tank center
  double GetX() const { return posX_; }
  
  /// Y coordinate of tank center
  double GetY() const { return posY_; }
  
  /// Z coordinate of tank center
  double GetZ() const { return posZ_; }
  
  /// Outer tank height
  double GetTankHeight() const { return tankHeight_; }
  
  double GetVirtualTankHeight() const { return GetTankHeight(); }
  
  /// Outer tank radius
  double GetTankRadius() const { return tankRadius_; }
  
  double GetVirtualTankRadius() const { return GetTankRadius(); }

  bool TrackParticle(const ExtendedI3Particle& particle,
                     HitHistoCollection& hitHC,
                     HitHistoCollection& cherHitCollection);
  
  
private:
  void AddParameters(I3Configuration& configuration);
  
  bool TankResponse(const ExtendedI3Particle& iceTrack,
                    HitHistoCollection& hitHC,
                    HitHistoCollection& cherHitCollection);
  
  void GenerateHits(int npe, const ExtendedI3Particle& p, HitHisto& hitHisto);
  
  double posX_;
  double posY_;
  double posZ_;
  double tankRadius_;
  double tankHeight_;
  double fillHeight_;
  double snowHeight_;
  double perlHeight_;
  
  double tauZirco_;
  double tauTyvek_;
  double decayTime_;
  bool useSnowPar_;
  
  std::map<OMKey, double> pePerVEM_;
  
  std::string randomServiceName_;
  I3RandomServicePtr randomService_;
   
  SET_LOGGER("I3ParamTankResponse");
};

#endif
