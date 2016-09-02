/*
 * class: I3XMLSummaryServiceFactory
 *
 * Version $Id: I3XMLSummaryServiceFactory.cxx 94949 2012-11-04 16:40:30Z nwhitehorn $
 *
 * Date: 2 Apr 2005
 *
 * (c) IceCube Collaboration
 */

// Class header files

#include "phys-services/I3XMLSummaryServiceFactory.h"
#include "phys-services/I3XMLSummaryService.h"
#include <sys/stat.h>

I3_SERVICE_FACTORY(I3XMLSummaryServiceFactory);


// Constructors

I3XMLSummaryServiceFactory::I3XMLSummaryServiceFactory(const I3Context& context)
  : I3ServiceFactory(context),
  inputFile_(),
  outputFile_("i3summary.xml")
{
  AddParameter("InputFileName","Name of XML input",inputFile_);
  AddParameter("OutputFileName","Name of XML output",outputFile_);

  installServiceAs_ = I3DefaultName<I3SummaryService>::value();
  AddParameter("InstallServiceAs",
               "Install the summary service at the following location"
               "(Default value is the value according to I3_DEFAULT_NAME)",
               installServiceAs_);
}

// Destructors

I3XMLSummaryServiceFactory::~I3XMLSummaryServiceFactory()
{
}

// Member functions

bool
I3XMLSummaryServiceFactory::InstallService(I3Context& services)
{
  if (!summary_)  {
	summary_ = I3XMLSummaryServicePtr(new I3XMLSummaryService());

	// check that input file exits
	if (!inputFile_.empty()) { 
		struct stat mystat;
		if (!stat(inputFile_.c_str(), &mystat)) { 
			summary_->ReadXML(inputFile_);
		} else {
			log_fatal("cannot read input file '%s'", inputFile_.c_str()); 
		}
	}
	summary_->SetPath(outputFile_);
  }

  return services.Put<I3SummaryService>(installServiceAs_, summary_);
}

void I3XMLSummaryServiceFactory::Configure()
{
  GetParameter("InputFileName",inputFile_);
  GetParameter("OutputFileName",outputFile_);
  GetParameter("InstallServiceAs", installServiceAs_);
}


/* eof */
