/** copyright  (C) 2004
    the icecube collaboration
    $Id: I3OpheliaParticle.cxx 7382 2005-05-08 18:43:18Z syoshida $

    @version $Revision: 1.1 $
    @date $Date: 2005-05-08 13:43:18 -0500 (Sun, 08 May 2005) $
    @author K.Hoshina

    @todo

*/

#include <icetray/serialization.h>
#include "recclasses/I3OpheliaRecoResult.h"

//_________________________________________________________________________________
void I3OpheliaRecoResult::DumpOut(std::string indent) const
{
   log_info("%s --------------------------------",indent.c_str());
   log_info("%s * I3OpheliaRecoResult::DumpOut ", indent.c_str());
   log_info("%s * FirstGuessTrack    : ", indent.c_str());
   firstGuessTrack_.DumpOut(indent + "    ");
   log_info("%s * StochasticCascade  : ", indent.c_str());
   stochasticCascade_.DumpOut(indent + "    ");
}

template <class Archive>
void I3OpheliaRecoResult::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3OpheliaParticle", 
                base_object< I3OpheliaParticle >(*this));
  ar & make_nvp("firstGuessTrack", firstGuessTrack_);
  ar & make_nvp("stochasticCascade", stochasticCascade_);
}

I3_SERIALIZABLE(I3OpheliaRecoResult);

bool I3OpheliaRecoResult::operator==(const I3OpheliaRecoResult& x) const
{
  return
    I3OpheliaParticle::operator==(x) &&
    firstGuessTrack_ == x.firstGuessTrack_ &&
    stochasticCascade_ == x.stochasticCascade_;
}
