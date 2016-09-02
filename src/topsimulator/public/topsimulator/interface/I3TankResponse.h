/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3TankResponse.h
 * @version $Rev: $
 * @date $Date: $
 * @author tilo
 */


#ifndef _TOPSIMULATOR_I3TANKRESPONSE_H_
#define _TOPSIMULATOR_I3TANKRESPONSE_H_

#include <icetray/I3Context.h>
#include <icetray/I3Configuration.h>
#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/TankKey.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <topsimulator/HitHisto.h>
#include <topsimulator/ExtendedI3Particle.h>
#include <map>


/**
 * \brief This is the base class for the IceTop tank responses
 */

class I3TankResponse
{
 public:
  
  I3TankResponse(I3Configuration& config, const I3Context& context, const TankKey& tankKey):
    tankKey_(tankKey),
    context_(context),
    configuration_(config)
  {}
  
  virtual ~I3TankResponse() {};
  
  /**
   * This method should be used to read in optional tank parameters
   * which can be defined in the constructor (as in I3Module)
   */
  virtual void Configure() {};
  

  /**
   * This method initializes the tank geometry and other things
   */
  virtual void Initialize(const I3Geometry& geometry,
			  const I3Calibration& calib,
			  const I3DetectorStatus& status) {};
  

  const TankKey& GetTankKey() const
  { return tankKey_; }

  /**
   * X coordinate of tank center
   */
  virtual double GetX() const = 0;
  
  /**
   * Y coordinate of tank center
   */
  virtual double GetY() const = 0;
  
  /**
   * Z coordinate of tank center
   */
  virtual double GetZ() const = 0;
  
  /**
   * Outer tank height
   */
  virtual double GetTankHeight() const = 0;
  
  /**
   * Tank height plus an (optional) margin (e.g. needed for geant4 simulations)
   * This is method is called by the injector service (the default should be the actual tank height)
   */
  virtual double GetVirtualTankHeight() const = 0;
  
  /**
   * Outer tank radius
   */
  virtual double GetTankRadius() const = 0;
  
  /**
   * Tank radius plus an (optional) margin (e.g. needed for geant4 simulations)
   * This is method is called by the injector service (the default should be the actual tank radius)
   */
  virtual double GetVirtualTankRadius() const = 0;
  
  /**
   * This method is called by the I3Topsimulator to do the tracking of a particle
   * It should return True if the particla has hit a tank (generated a signal)
   */
  virtual bool TrackParticle(const ExtendedI3Particle& particle, HitHistoCollection& hitHC, HitHistoCollection& cherHitCollection) { return false; }
  
  /**
   * This method is called by the I3Topsimulator at the beginning of an event
   */
  virtual void BeginEvent(const I3Particle& primary) {};
  
  /**
   * This method is called by the I3Topsimulator at the end of an event
   */
  virtual void EndEvent(HitHistoCollection &hitHC, HitHistoCollection& cherHitCollection) {};
  
  const I3Configuration& GetConfiguration() { return configuration_; }

protected:
  
  template <class ParamType>
    void AddParameter(const std::string& name,
		    const std::string& description,
		    const ParamType& defaultValue)
  {
    if (!configuration_.Has(name))
      configuration_.Add(name, description, defaultValue);
  }

  template <class ParamType>
  void GetParameter(const std::string& name, ParamType& value)
  {
    value = configuration_.Get<ParamType>(name);
  }
  
  const I3Context& GetContext()
  { return context_; }
  

  const TankKey tankKey_;

 private:
  const I3Context& context_;
  I3Configuration& configuration_;
  
  SET_LOGGER("I3TankResponse");
};


I3_POINTER_TYPEDEFS(I3TankResponse);

typedef std::map<TankKey, I3TankResponsePtr> I3TankResponseMap;
I3_POINTER_TYPEDEFS(I3TankResponseMap);


#endif
