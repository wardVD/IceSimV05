/** copyright  (C) 2004
    the icecube collaboration
    $Id: I3OpheliaParticle.cxx 7382 2005-05-08 18:43:18Z syoshida $

    @version $Revision: 1.1 $
    @date $Date: 2005-05-08 13:43:18 -0500 (Sun, 08 May 2005) $
    @author K.Hoshina

    @todo

*/

#include "recclasses/I3OpheliaParticle.h"
#include <icetray/serialization.h>
#include <inttypes.h>

//_________________________________________________________________________________
I3Particle& I3OpheliaParticle::GetI3Particle()
{
   return i3particle_;
}

//_________________________________________________________________________________
const I3Particle& I3OpheliaParticle::GetConstI3Particle() const
{
   // just return current snap shot...
   return i3particle_;
}

//_________________________________________________________________________________
void I3OpheliaParticle::SetI3Particle(const I3Particle &p)
{
   i3particle_ = p;
}

//_________________________________________________________________________________
void I3OpheliaParticle::DumpOut(std::string indent) const
{
   log_info("%s --------------------------------",indent.c_str());
   log_info("%s * I3OpheliaParticle::DumpOut ", indent.c_str());
   log_info("%s * MajorID : %" PRIu64, indent.c_str(), i3particle_.GetMajorID());
   log_info("%s * MinorID : %d", indent.c_str(), i3particle_.GetMinorID());

   indent += "    ";  // two white space indent
   for (unsigned int i=0; i<daughters_.size(); i++) {
      daughters_[i]->DumpOut(indent);
   }
}

//_________________________________________________________________________________
template <class Archive>
void I3OpheliaParticle::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

	if (version == 0) {
      ar & make_nvp("i3particle", i3particle_);
      ar & make_nvp("daughters", daughters_);
   } else if (version == 1) {
      ar & make_nvp("daughters", daughters_);
      ar & make_nvp("i3particle", i3particle_);
	}
}
                                                                                
I3_SERIALIZABLE(I3OpheliaParticle);

bool I3OpheliaParticle::operator==(const I3OpheliaParticle& other) const
{
  if (!(i3particle_ == other.i3particle_))
    return false;

  if (daughters_.size() != other.daughters_.size())
    return false;

  for (unsigned i = 0; i < daughters_.size(); ++i) {
    if (!(*daughters_[i] == *other.daughters_[i]))
      return false;
  }

  return true;
}
