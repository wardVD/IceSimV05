#include "I3OpheliaFirstGuessTrackConverter.h"
#include <icetray/I3Units.h>
#include <dataclasses/physics/I3Particle.h>

I3TableRowDescriptionPtr
I3OpheliaFirstGuessTrackConverter::CreateDescription(const I3OpheliaFirstGuessTrack& fgtrack) 
{
  I3TableRowDescriptionPtr desc(new I3TableRowDescription());

  desc->AddField<double>("cosZenith", "",
			 "cosine of zenith obtained by the EHE ophelia linefit");
  desc->AddField<double>("azimuth", "rad",
			 "azimuth obtained by the EHE ophelia linefit");
  desc->AddField<double>("cobX", "m", "center of brightness x position");
  desc->AddField<double>("cobY", "m", "center of brightness y position");
  desc->AddField<double>("cobZ", "m", "center of brightness z position");
  desc->AddField<double>("brightestDOMX", "m", "the brightest DOM x position");
  desc->AddField<double>("brightestDOMY", "m", "the brightest DOM y position");
  desc->AddField<double>("brightestDOMZ", "m", "the brightest DOM z position");
  desc->AddField<double>("vX", "m/ns", "EHE linefit velocity x component");
  desc->AddField<double>("vY", "m/ns", "EHE linefit velocity y component");
  desc->AddField<double>("vZ", "m/ns", "EHE linefit velocity z component");
  desc->AddField<double>("fitQuality", "", "chi square of the EHE linefit");

  return desc;
}

size_t 
I3OpheliaFirstGuessTrackConverter::FillRows(const I3OpheliaFirstGuessTrack& fgtrack,
				 I3TableRowPtr rows) 
{ 

  const I3Particle& track =  fgtrack.GetConstI3Particle();
  
  rows->Set<double>("cosZenith"    , cos( track.GetZenith())); // cos(reco Zenith)
  rows->Set<double>("azimuth"    , track.GetAzimuth());  // reconstucted Azimuth
  rows->Set<double>("cobX"    , fgtrack.GetCenterOfBrightnessX()/I3Units::m); // COB-X
  rows->Set<double>("cobY"    , fgtrack.GetCenterOfBrightnessY()/I3Units::m); // COB-Y
  rows->Set<double>("cobZ"    , fgtrack.GetCenterOfBrightnessZ()/I3Units::m); // COB-Z
  rows->Set<double>("brightestDOMX" , fgtrack.GetLargestNPEDOMpositionX()/I3Units::m); 
                                                        // the brightest DOM pos-x
  rows->Set<double>("brightestDOMY" , fgtrack.GetLargestNPEDOMpositionY()/I3Units::m); 
                                                        // the brightest DOM pos-y
  rows->Set<double>("brightestDOMZ" , fgtrack.GetLargestNPEDOMpositionZ()/I3Units::m); 
                                                        // the brightest DOM pos-z
  rows->Set<double>("vX" , fgtrack.GetVelocityX()); // Velocity x
  rows->Set<double>("vY" , fgtrack.GetVelocityY()); // Velocity y
  rows->Set<double>("vZ" , fgtrack.GetVelocityZ()); // Velocity z
  rows->Set<double>("fitQuality" , fgtrack.GetFitQuality()); // chi^2 of the EHE linefit
  
  return 1;
}
