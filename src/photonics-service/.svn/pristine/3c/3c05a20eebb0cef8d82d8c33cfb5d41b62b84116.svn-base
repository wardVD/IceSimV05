/**
 *@file
 *@brief Implementation of class I3PhotonicsServiceFactory
 *
 *@author Klas Hultqvist
 *@version $Id$
 *@date $Date$
 *(c) the IceCube Collaboration
 */
#include "photonics-service/I3DummyPhotonicsService.h"
#include "photonics-service/I3PhotonicsTableService.h"
#include "photonics-service/I3PhotonicsServiceFactory.h"
#include "photonics-service/I3PhotonicsServiceCommons.h"

I3_SERVICE_FACTORY(I3PhotonicsServiceFactory);

#include "icetray/I3Units.h"

#include "boost/make_shared.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
using boost::filesystem::exists;
using boost::filesystem::path;
using boost::filesystem::current_path;
using boost::filesystem::remove;

I3PhotonicsServiceFactory::I3PhotonicsServiceFactory(I3Context const &context)
  : I3ServiceFactory(context)
{
  log_debug("Constructing I3PhotonicsServiceFactory");

  photonicsTopLevelDirectory_ = getenv("I3_BUILD");
  AddParameter("PhotonicsTopLevelDirectory",
	       "Path to the directory containing the "
	       "subdirectory with tables",
	       photonicsTopLevelDirectory_);

  tablesDirectory_="";
  AddParameter("TablesDirectory",
	       "Deprecated synonym for PhotonicsTopLevelDirectory ",
	       tablesDirectory_);
  

  driverFileDirectory_ = photonicsTopLevelDirectory_;
  AddParameter("DriverFileDirectory",
	       "Directory where the driver files are located ",
	       driverFileDirectory_);  

  level1DriverFile_ = "level1_table.list";
  AddParameter("PhotonicsLevel1DriverFile", "Name of level 1 driver file",
	       level1DriverFile_);

  level2DriverFile_ = "level2_table.list";
  AddParameter("PhotonicsLevel2DriverFile", "Name of level 2 driver file",
	       level2DriverFile_);

  interpolMode_ = 255;
  AddParameter("PhotonicsInterpolationMode", "Photonics interpolation bitmask 1,2,4 internal coords, 16,32 depth,zenith, 128 pivot mode",
	       interpolMode_);

  verbosity_ = 0;
  AddParameter("PhotonicsVerbosity", "***Depreciated*** use log4cplus to changing logging levels",
	       verbosity_);

  angularSelectLow_ = 0*I3Units::degree;
  AddParameter("PhotonicsAngularSelectionLow","Minimimum zenith angle"
	       " for photonics tables loading, double", angularSelectLow_);

  angularSelectHigh_ = 180*I3Units::degree;
  AddParameter("PhotonicsAngularSelectionHigh","Maximum zenith angle"
	       " for photonics tables loading, double", angularSelectHigh_);

  zSelectLow_ = -DBL_MAX;
  AddParameter("PhotonicsZSelectionLow","Minimimum z"
	       " for photonics tables loading, double", zSelectLow_);

  zSelectHigh_ = DBL_MAX;
  AddParameter("PhotonicsZSelectionHigh","Maximum z"
	       " for photonics tables loading, double", zSelectHigh_);


  photonicsTableSelection_=0;
  AddParameter("PhotonicsTableSelection",
	       "Photonics table level, or zero for both",
	       photonicsTableSelection_);

  useDummyService_=false;
  AddParameter("UseDummyService",
	       "Use dummy photonics service without photon tables",
	       useDummyService_);

  AddParameter("ServiceName",
	       "Name of Photonics Service",
	       I3DefaultName<I3PhotonicsService>::value());

  isPhotorecTable_=false;
  AddParameter("IsPhotorecTable",
               "whether the table being read is actually a photorec table",
               isPhotorecTable_);
}

I3PhotonicsServiceFactory::~I3PhotonicsServiceFactory() { }

void I3PhotonicsServiceFactory::Configure()
{
  log_debug("Configuring I3PhotonicsService");

  GetParameter("PhotonicsTopLevelDirectory", photonicsTopLevelDirectory_);
  GetParameter("TablesDirectory", tablesDirectory_);
  GetParameter("DriverFileDirectory", driverFileDirectory_);
  GetParameter("PhotonicsLevel1DriverFile",level1DriverFile_);
  GetParameter("PhotonicsLevel2DriverFile",level2DriverFile_);
  GetParameter("PhotonicsInterpolationMode",interpolMode_);
  GetParameter("PhotonicsVerbosity",verbosity_);
  GetParameter("PhotonicsAngularSelectionLow",angularSelectLow_);
  GetParameter("PhotonicsAngularSelectionHigh",angularSelectHigh_);
  GetParameter("PhotonicsZSelectionLow",zSelectLow_);
  GetParameter("PhotonicsZSelectionHigh",zSelectHigh_);
  GetParameter("PhotonicsTableSelection",photonicsTableSelection_);
  GetParameter("UseDummyService",useDummyService_);
  GetParameter("ServiceName",serviceName_);
  GetParameter("IsPhotorecTable",isPhotorecTable_);
  if(tablesDirectory_ != "") {
    log_warn("Deprecated option TablesDirectory overrides PhotonicsTopLevelDirectory\n");
    photonicsTopLevelDirectory_ = tablesDirectory_;
  }
  /**
   * Remove trailing blanks from directory names
   */
  if(*(photonicsTopLevelDirectory_.end()-1) == '/')
    photonicsTopLevelDirectory_.erase(photonicsTopLevelDirectory_.end()-1);
  if(*(driverFileDirectory_.end()-1) == '/')
    driverFileDirectory_.erase(driverFileDirectory_.end()-1);
}

bool
I3PhotonicsServiceFactory::InstallService(I3Context& services)
{
  log_debug("PhotonicsService install starting...");

  if (pxs_) {
    // only one I3PhotonicsService per factory
    log_debug("PhotonicsService already installed!");
  } else {
    // create and initialize service
    if (useDummyService_)
      {
	pxs_ = boost::make_shared<I3DummyPhotonicsService>();
      }
    else
      {
	pxs_ = boost::shared_ptr<I3PhotonicsTableService>(
                  new I3PhotonicsTableService(
		    photonicsTopLevelDirectory_, driverFileDirectory_,
		    level1DriverFile_, level2DriverFile_, interpolMode_,
		    verbosity_, angularSelectLow_, angularSelectHigh_, 
		    zSelectLow_, zSelectHigh_, photonicsTableSelection_,
		    isPhotorecTable_));
      }
  }   
  if (pxs_) {
      /**
       *When initialisation sucessful, add service to icetray context
       */
 
      log_debug("Trying to install PhotonicsService as %s", serviceName_.c_str());
      if ( services.Put<I3PhotonicsService>(pxs_,serviceName_.c_str()) )
        return true;
      else {
        log_warn("Couldn't install PhotonicsService");
        return false;
      }
  }
  else {
      log_warn("Couldn't initialize PhotonicService");
      return false;
  }
  
}
