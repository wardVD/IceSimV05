/**
 * (c) 2005
 * the IceCube Collaboration
 * $Id: I3XMLOMKey2MBIDFactory.cxx 94949 2012-11-04 16:40:30Z nwhitehorn $
 *
 * @file I3XMLOMKey2MBIDFactory.cxx
 * @brief implementation of the I3XMLOMKey2MBIDFactory class
 * @version $Revision: 94949 $
 * @date $Date: 2012-11-04 10:40:30 -0600 (Sun, 04 Nov 2012) $
 * @author tschmidt
 */
 
// class header file

#include <phys-services/I3XMLOMKey2MBIDFactory.h>
I3_SERVICE_FACTORY(I3XMLOMKey2MBIDFactory);

#include <icetray/I3Context.h>
#include <icetray/I3DefaultName.h>
#include <phys-services/I3XMLOMKey2MBID.h>

// namespace declarations


// implementation

I3XMLOMKey2MBIDFactory::I3XMLOMKey2MBIDFactory(const I3Context& context)
  : I3ServiceFactory(context),
    installServiceAs_(I3DefaultName<I3OMKey2MBID>::value())
{   
  AddParameter("Infile", "The name of the file to get the mainboard ID list from", infile_);
  AddParameter("InstallServiceAs",
               "Install the service at the following location",
               installServiceAs_);
}


I3XMLOMKey2MBIDFactory::~I3XMLOMKey2MBIDFactory()
{
}


bool I3XMLOMKey2MBIDFactory::InstallService(I3Context& services)
{
  if(!service_)
    service_ = I3XMLOMKey2MBIDPtr(new I3XMLOMKey2MBID(infile_));

  return(services.Put<I3OMKey2MBID>(installServiceAs_, service_));
}


void I3XMLOMKey2MBIDFactory::Configure()
{
  GetParameter("Infile", infile_); 
  GetParameter("InstallServiceAs", installServiceAs_);
}
