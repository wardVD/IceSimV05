#ifndef I3TRANDOMSERVICEFACTORY_H
#define I3TRANDOMSERVICEFACTORY_H
/*
 * class: I3TRandomServiceFactory
 *
 * Version $Id: I3TRandomServiceFactory.h 23629 2006-09-13 22:27:34Z tschmidt $
 *
 * Date: 17 Feb 2004
 *
 * (c) IceCube Collaboration
 */

// Header files

#include <string>

// Forward declarations

class I3Context;

// Superclasses

#include "icetray/I3ServiceFactory.h"
#include "phys-services/I3RandomService.h"

/**
 * @brief This class installs a I3TRandomService.
 *
 * I3TRandomService supports two parameters: <VAR>Seed</VAR>,
 * <VAR>InstallServiceAs</VAR>.
 * @version $Id: I3TRandomServiceFactory.h 23629 2006-09-13 22:27:34Z tschmidt $
 * @author pretz
 */
class I3TRandomServiceFactory
: public I3ServiceFactory
{
 public:

  // Constructors and destructor

  I3TRandomServiceFactory(const I3Context& context);

  virtual ~I3TRandomServiceFactory();

  // public member functions

  /**
   * Installed this objects service into the specified services object.
   *
   * @param services the I3Context into which the service should be installed.
   * @return true if the services is successfully installed.
   */
  virtual bool InstallService(I3Context& services);

  /**
   * Configure service prior to installing it. 
   */
  virtual void Configure();

 private:

  // private constructors, destructor and assignment

  I3TRandomServiceFactory
    (const I3TRandomServiceFactory& rhs); // stop default
  I3TRandomServiceFactory operator=
    (const I3TRandomServiceFactory& rhs); // stop default

  // instance member data

	int seed_;
	I3RandomServicePtr random_;
	std::string installServiceAs_;

   SET_LOGGER("I3TRandomServiceFactory");
};

#endif 
