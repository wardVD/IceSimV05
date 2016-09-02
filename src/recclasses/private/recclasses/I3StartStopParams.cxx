/**
 * @brief implementation of the I3StartStopParams class
 *
 * @file I3StartStopParams.cxx
 * @version $Revision: 54913 $
 * @date $Date: 2009-05-12 12:04:02 +0200 (Tue, 12 May 2009) $
 * @author Sebastian Euler <sebastian.euler@icecube.wisc.edu>
 *
 * The serialization for the data class
 */

#include "recclasses/I3StartStopParams.h"
#include <icetray/serialization.h>

template <class Archive>
void I3StartStopParams::serialize(Archive& ar, unsigned version){
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("LLHStartingTrack",LLHStartingTrack);
  ar & make_nvp("LLHStoppingTrack",LLHStoppingTrack);
  ar & make_nvp("LLHInfTrack",LLHInfTrack);
  if (version != 1){
    log_warn( "too high version number" );
  }
}

void I3StartStopParams::Reset(){
  LLHStartingTrack = NAN;
  LLHStoppingTrack = NAN;
  LLHInfTrack = NAN;
}

std::ostream& operator<<(std::ostream& os, const I3StartStopParams& ssp)
{
  os << "[ I3StartStopParams LLHStartingTrack : " << ssp.LLHStartingTrack << '\n'
     << "                    LLHStoppingTrack : " << ssp.LLHStoppingTrack << '\n'
     << "                         LLHInfTrack : " << ssp.LLHInfTrack  << '\n'
     << "]" ;
  return os;
}

BOOST_CLASS_VERSION(I3StartStopParams, 1);
I3_SERIALIZABLE(I3StartStopParams);
