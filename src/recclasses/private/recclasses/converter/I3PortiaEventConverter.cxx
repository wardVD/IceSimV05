#include "I3PortiaEventConverter.h"

I3TableRowDescriptionPtr
I3PortiaEventConverter::CreateDescription(const I3PortiaEvent& evt) 
{
  I3TableRowDescriptionPtr desc(new I3TableRowDescription());

  desc->AddField<double>("bestNPE", "pe",
			 "number of photoelectrons");
  desc->AddField<double>("atwdNPE", "pe",
			 "number of photoelectrons");
  desc->AddField<double>("fadcNPE", "pe",
			 "number of photoelectrons");

  desc->AddField<double>("bestNPEbtw", "pe",
			 "number of photoelectrons");
  desc->AddField<double>("atwdNPEbtw", "pe",
			 "number of photoelectrons");
  desc->AddField<double>("fadcNPEbtw", "pe",
			 "number of photoelectrons");

  desc->AddField<double>("NCHbtw", "1",
			 "number of hit channels");
  desc->AddField<double>("atwdNCHbtw", "1",
			 "number of hit channels");
  desc->AddField<double>("fadcNCHbtw", "1",
			 "number of hit channels");

  desc->AddField<double>("NCH", "1",
			 "number of hit channels");
  desc->AddField<double>("atwdNCH", "1",
			 "number of hit channels");
  desc->AddField<double>("fadcNCH", "1",
			 "number of hit channels");


  return desc;
}

size_t 
I3PortiaEventConverter::FillRows(const I3PortiaEvent& evt,
				 I3TableRowPtr rows) 
{ 
  rows->Set<double>("bestNPE"    , evt.GetTotalBestNPE());
  rows->Set<double>("atwdNPE"    , evt.GetTotalAtwdNPE());
  rows->Set<double>("fadcNPE"    , evt.GetTotalFadcNPE());
  rows->Set<double>("bestNPEbtw" , evt.GetTotalBestNPEbtw());
  rows->Set<double>("atwdNPEbtw" , evt.GetTotalAtwdNPEbtw());
  rows->Set<double>("fadcNPEbtw" , evt.GetTotalFadcNPEbtw());
  rows->Set<double>("atwdNCH"    , evt.GetAtwdNch());
  rows->Set<double>("fadcNCH"    , evt.GetFadcNch());
  rows->Set<double>("atwdNCHbtw" , evt.GetAtwdNchbtw());
  rows->Set<double>("fadcNCHbtw" , evt.GetFadcNchbtw());
  rows->Set<double>("NCH"        , evt.GetTotalNch());
  rows->Set<double>("NCHbtw"     , evt.GetTotalNchbtw());
  
  return 1;
}
