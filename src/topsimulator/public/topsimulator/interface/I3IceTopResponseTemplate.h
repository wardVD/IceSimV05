/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3IceTopResponseTemplate.h
 * @version $Rev: $
 * @date $Date: $
 * @author tilo
 */


#ifndef _TOPSIMULATOR_I3ICETOPRESPONSETEMPLATE_H_
#define _TOPSIMULATOR_I3ICETOPRESPONSETEMPLATE_H_


#include <icetray/I3Logging.h>
#include <dataclasses/TankKey.h>
#include <topsimulator/interface/I3IceTopResponseService.h>
#include <topsimulator/interface/I3TankResponse.h>
#include <boost/foreach.hpp>
#include <I3/name_of.h>

/**
 * \brief This class provides the interface to different I3TankResponse implementations (types)
 */

template <class T>
class I3IceTopResponseTemplate : public I3IceTopResponseService
{
 public:
  I3IceTopResponseTemplate(const I3Context& context): I3IceTopResponseService(context)
  {
    // The following statement adds the response specific parameters
    // to the context, which will be used for all tank responses
    T dummyResponse(GetConfiguration(), context, TankKey());
  };
  
  void Initialize(const I3Geometry& geometry,
		  const I3Calibration& calib,
		  const I3DetectorStatus& status)
  {
    // Get station geometry and DOM stati
    const I3StationGeoMap& stationMap = geometry.stationgeo;
    const std::map<OMKey, I3DOMStatus>& domStatusMap = status.domStatus;
    
    I3StationGeoMap::const_iterator station_iter;
    for(station_iter=stationMap.begin(); station_iter!=stationMap.end(); ++station_iter)
    {
      BOOST_FOREACH(const I3TankGeo& tankGeo, station_iter->second)
      {
	// Check if tank has correct number of DOMs
	if(tankGeo.omKeyList_.size()!=2)
	{
	  log_fatal("Invalid number of DOMs (%zu) in station %d!", tankGeo.omKeyList_.size(), station_iter->first);
	  continue;
	}
	
	// Check if tank is active (at least one DOM must be powered up!)
	bool isActive(false);
	BOOST_FOREACH(const OMKey& omKey, tankGeo.omKeyList_)
	{
	  // Get the DOMStatus of this DOM
	  std::map<OMKey, I3DOMStatus>::const_iterator status_iter = domStatusMap.find(omKey);
	  if(status_iter==domStatusMap.end()) continue;
	  
	  // Check if PMT is powered up
	  if(status_iter->second.pmtHV>0)
	  {
	    isActive = true;
	    break;
	  }
	}
	
	// If tank is active create the tank response
	if(isActive)
	{
	  const TankKey tankKey(tankGeo.omKeyList_.at(0));
	  boost::shared_ptr<T> tankResponse(new T(GetConfiguration(), GetContext(), tankKey));
	  
	  // Get logger of tank response and store logging level in variable
	  I3LogLevel logLevel = GetIcetrayLogger()->LogLevelForUnit(I3::name_of<T>());
	  
	  // Set logging level to fatal if tank is not first tank.
	  // This avoids duplicate logging statements when more 
	  // than one tanks are configured
	  if(!tankMap_->empty())
	    GetIcetrayLogger()->SetLogLevelForUnit(I3::name_of<T>(), I3LOG_FATAL);

	  tankResponse->Configure();
	  
	  // Reset to original logging level
	  GetIcetrayLogger()->SetLogLevelForUnit(I3::name_of<T>(), logLevel);
	  
	  // Initialize tank geometry and other things
	  tankResponse->Initialize(geometry, calib, status);
	  tankMap_->insert(std::make_pair(tankKey, boost::dynamic_pointer_cast<I3TankResponse>(tankResponse)));
	}
      }
    }
    
    // Initialize search grid
    InitializeGrid();
  };
  
  SET_LOGGER("I3IceTopResponseTemplate");
};

#endif
