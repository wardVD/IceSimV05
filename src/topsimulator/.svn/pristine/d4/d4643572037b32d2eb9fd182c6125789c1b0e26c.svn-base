/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3InjectorService.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#ifndef _TOPSIMULATOR_I3INJECTORSERVICE_H_
#define _TOPSIMULATOR_I3INJECTORSERVICE_H_


#include <icetray/I3ServiceBase.h>
#include <icetray/I3Context.h>
#include <icetray/I3FrameObject.h>
#include <icetray/I3DefaultName.h>
#include <dataclasses/physics/I3Particle.h>
#include <topsimulator/interface/I3IceTopResponseService.h>
#include <topsimulator/ExtendedI3Particle.h>

#include <map>
#include <string>

/**
 * \brief This is the base class/interface for all particle injectors
 */

class I3InjectorService: public I3ServiceBase
{
public:
  I3InjectorService(const I3Context& context): I3ServiceBase(context) {}
  
  virtual ~I3InjectorService() {};
  
  virtual void Configure() {};
  
  virtual bool NextEvent(int& runID, int& evtID, I3Particle& primary, I3FrameConstPtr frame) = 0;
  
  virtual bool NextParticle(ExtendedI3Particle& primary) = 0;
  
  virtual I3FrameObjectConstPtr GetEventInfo() {return I3FrameObjectConstPtr();};

  void SetResponseService(I3IceTopResponseServicePtr response) {responseService_ = response;};

  // This method is required to document in the i3 file
  virtual std::map<std::string, int> GetAirShowerComponentNameMap() const = 0;

protected:
  
  template <class ParamType>
  void AddParameter(const std::string& name,
		    const std::string& description,
		    const ParamType& defaultValue)
  { I3ServiceBase::AddParameter<ParamType>(name, description, defaultValue); }

  template <class ParamType>
  void GetParameter(const std::string& name, ParamType& value) const
  { I3ServiceBase::GetParameter<ParamType>(name, value); }

  const I3Context& GetContext()
  { return I3ServiceBase::GetContext(); }

  I3IceTopResponseServicePtr responseService_;

  SET_LOGGER("I3InjectorService");
};

I3_DEFAULT_NAME(I3InjectorService);
I3_POINTER_TYPEDEFS(I3InjectorService);


#endif
