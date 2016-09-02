#ifndef I3XMLSUMMARYSERVICEFACTORY_H
#define I3XMLSUMMARYSERVICEFACTORY_H
/*
 * class: I3XMLSummaryServiceFactory
 *
 * (c) 2007 IceCube Collaboration
 * @version $Revision$
 * @date $Date$
 * @author juancarlos@icecube.wisc.edu
 */

// Header files
#include <string>
#include <phys-services/I3XMLSummaryService.h>

// Forward declarations

class I3Context;

// Superclasses

#include "icetray/I3ServiceFactory.h"

/**
 * @brief This class installs a I3XMLSummaryService.
 *
 */
class I3XMLSummaryServiceFactory
: public I3ServiceFactory
{
 public:

  // Constructors and destructor

  I3XMLSummaryServiceFactory(const I3Context& context);

  ~I3XMLSummaryServiceFactory();

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

  I3XMLSummaryServiceFactory
    (const I3XMLSummaryServiceFactory& rhs); // stop default
  I3XMLSummaryServiceFactory operator=
    (const I3XMLSummaryServiceFactory& rhs); // stop default

  // instance member data
  std::string inputFile_;
  std::string outputFile_;
  std::string installServiceAs_;

  I3XMLSummaryServicePtr summary_;

  SET_LOGGER("I3XMLSummaryServiceFactory");
};

#endif
