#ifndef I3PHOTONICSSERVICEFACTORY_H
#define I3PHOTONICSSERVICEFACTORY_H
/**
 *@file
 *@brief I3PhotonicsServiceFactory
 *       A class for creating an I3PhotonicsService object and making it available for use. 
 *@author Klas Hultqvist
 *@version $Id$
 *@date $Date$
 *(c) the IceCube Collaboration
 *
 */

#include "icetray/I3ServiceFactory.h"
#include <boost/shared_ptr.hpp>
#include "photonics-service/I3PhotonicsService.h"
#include "photonics-service/I3DummyPhotonicsService.h"


#include <string>

using boost::shared_ptr;
using std::string;

class I3Context;
class I3Services;

/**@brief Initialise an I3PhotonicsService and add it to the tray*/
class I3PhotonicsServiceFactory : public I3ServiceFactory
{

 SET_LOGGER("I3PhotonicsServiceFactory");

 public:

  /**
   * @brief Constructor
   * @param context IceTray context
   */
  I3PhotonicsServiceFactory(I3Context const &context);

  /**
   * @brief Destructor
   */
  virtual ~I3PhotonicsServiceFactory();

/**
 *@brief Initialise photonics service and add it to the tray
 *@param services    Reference to service context
 */
  virtual bool InstallService(I3Context& services);

  /**
   * @brief Get parameters from IceTray
   */
  void Configure();
  
 private:

  /**
   * @brief default constructor declared private and unimplemented
   */
  I3PhotonicsServiceFactory();

  /**
   * @brief copy constructor declared private and unimplemented
   */
  I3PhotonicsServiceFactory
    (I3PhotonicsServiceFactory const &rhs);
  
  /**
   * @brief assignment operator declared private and unimplemented
   */
  I3PhotonicsServiceFactory operator=
    (I3PhotonicsServiceFactory const &rhs);

  /** Level 1 driver file name */
  std::string level1DriverFile_;
  /** Level 2 driver file name */
  std::string level2DriverFile_;
  /** Interpolation mode (bit mask) 
   */
  int interpolMode_;
  /** Verbosity */
  int verbosity_;
  /** Minimum zenith angle to load tables for */
  double angularSelectLow_;
  /** Maximum zenith angle to load tables for */
  double angularSelectHigh_;
 /** Minimum z to load tables for */
  double zSelectLow_;
  /** Maximum z to load tables for */
  double zSelectHigh_;
  /** Specifies wanted table format (level 1, level 2 or both (0))*/
  int photonicsTableSelection_;
  /** Flags dummy photonics service to be used */
  bool useDummyService_;

  /**
   * pointer to PhotonicsService instance
   */
  I3PhotonicsServicePtr pxs_;

  /** Directory holding photonics driver files */
  std::string driverFileDirectory_;
  /** Directory holding "tables" subdirectory with photonics tables */
  std::string photonicsTopLevelDirectory_;
  /** Deprecated alias for photonicsTopLevelDirectory_ */
  std::string tablesDirectory_;
  /** Name of the photonics service */
  std::string serviceName_;
  /** Whether the table to load is a photorec table */
  bool isPhotorecTable_;

};

#endif
