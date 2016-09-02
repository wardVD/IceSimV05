/** copyright  (C) 2004
    the icecube collaboration
    $Id: I3OpheliaParticle.cxx 7382 2005-05-08 18:43:18Z syoshida $

    @version $Revision: 1.1 $
    @date $Date: 2005-05-08 13:43:18 -0500 (Sun, 08 May 2005) $
    @author K.Hoshina

    @todo

*/

#include "recclasses/I3OpheliaFirstGuessTrack.h"
#include "recclasses/Utility.h"
#include <icetray/serialization.h>

//_________________________________________________________________________________
I3OpheliaFirstGuessTrack::I3OpheliaFirstGuessTrack()
                         :I3OpheliaParticle(), 
                          xvel_(0), yvel_(0), zvel_(0), xpos_(0), ypos_(0), zpos_(0),
                          fitsuccess_(false), fitquality_(0)
{}

//_________________________________________________________________________________
void I3OpheliaFirstGuessTrack::DumpOut(std::string indent) const
{
   log_info("%s --------------------------------",indent.c_str());
   log_info("%s * I3OpheliaFirstGuessTrack::DumpOut ", indent.c_str());
   log_info("%s * linefit velocity  : (%f, %f, %f)  ", indent.c_str(), xvel_, yvel_, zvel_); 
   log_info("%s * CenterOfBrightness: (%f, %f, %f)  ", indent.c_str(), xpos_, ypos_, zpos_);
   log_info("%s * FitSuccess        : %d", indent.c_str(), fitsuccess_);
   log_info("%s * FitQuality        : %f", indent.c_str(), fitquality_);
                                                                                         
   indent += "    ";  // two white space indent

   I3OpheliaParticle::DumpOut(indent);

}

//_________________________________________________________________________________
template <class Archive>
void I3OpheliaFirstGuessTrack::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3OpheliaParticle", base_object<I3OpheliaParticle>(*this));
  ar & make_nvp("Velocity_x", xvel_ );
  ar & make_nvp("Velocity_y", yvel_ );
  ar & make_nvp("Velocity_z", zvel_ );
  ar & make_nvp("CenterOfBrightness_x", xpos_ );
  ar & make_nvp("CenterOfBrightness_y", ypos_ );
  ar & make_nvp("CenterOfBrightness_z", zpos_ );
  ar & make_nvp("FitSuccess", fitsuccess_ );
  ar & make_nvp("FitQuality", fitquality_ );

  if(version>=2){// variables used in the IC40 analysis
    ar & make_nvp("BrightestDOM_x", brightestDOMxpos_ );
    ar & make_nvp("BrightestDOM_y", brightestDOMypos_ );
    ar & make_nvp("BrightestDOM_z", brightestDOMzpos_ );
  }

}

I3_SERIALIZABLE(I3OpheliaFirstGuessTrack);

bool I3OpheliaFirstGuessTrack::operator==(const I3OpheliaFirstGuessTrack& other) const
{
  return
    I3OpheliaParticle::operator==(other) &&
    nan_aware_equality(xvel_, other.xvel_) &&
    nan_aware_equality(yvel_, other.yvel_) &&
    nan_aware_equality(zvel_, other.zvel_) &&
    nan_aware_equality(xpos_, other.xpos_) &&
    nan_aware_equality(ypos_, other.ypos_) &&
    nan_aware_equality(zpos_, other.zpos_) &&
    fitsuccess_ == other.fitsuccess_ &&
    nan_aware_equality(fitquality_, other.fitquality_);
}
