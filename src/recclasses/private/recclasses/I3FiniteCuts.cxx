/**
 * @brief implementation of the I3FiniteCuts class
 *
 * @file I3FiniteCuts.cxx
 * @version $Revision: 142585 $
 * @date $Date: 2016-02-27 13:44:58 -0600 (Sat, 27 Feb 2016) $
 * @author Sebastian Euler <sebastian.euler@icecube.wisc.edu>
 *
 * The serialization method for the data class
 */

#include "recclasses/I3FiniteCuts.h"
#include "recclasses/Utility.h"
#include <icetray/serialization.h>

template <class Archive>
void I3FiniteCuts::serialize(Archive& ar, unsigned version){
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("Length_",Length);
  ar & make_nvp("Lend_",Lend);
  ar & make_nvp("Lstart_",Lstart);
  ar & make_nvp("Sdet_",Sdet);
  ar & make_nvp("finiteCut_",finiteCut);
  ar & make_nvp("DetectorLength_",DetectorLength);
  
  if (version > 1){
    log_warn( "too high version number" );
  }
}

void I3FiniteCuts::Reset(){
  Sdet = NAN;
  finiteCut = NAN;
  Length = NAN;
  Lend = NAN;
  Lstart = NAN;
  DetectorLength = NAN;
}

std::ostream& operator<<(std::ostream& os, const I3FiniteCuts& fc)
{
  os << "[ I3FiniteCuts Length : " << fc.Length << std::endl
     << "          endFraction : " << fc.Lend << std::endl
     << "        startFraction : " << fc.Lstart  << std::endl
     << "                 Sdet : " << fc.Sdet << std::endl
     << "            finiteCut : " << fc.finiteCut  << std::endl
     << "       DetectorLength : " << fc.DetectorLength << std::endl
     << "]" ;
  return os;
}

BOOST_CLASS_VERSION(I3FiniteCuts, 1);
I3_SERIALIZABLE(I3FiniteCuts);

bool I3FiniteCuts::operator==(const I3FiniteCuts& other) const
{
  return
    nan_aware_equality(Length, other.Length) &&
    nan_aware_equality(Lend, other.Lend) &&
    nan_aware_equality(Lstart, other.Lstart) &&
    nan_aware_equality(Sdet, other.Sdet) &&
    nan_aware_equality(finiteCut, other.finiteCut) &&
    nan_aware_equality(DetectorLength, other.DetectorLength);
}
