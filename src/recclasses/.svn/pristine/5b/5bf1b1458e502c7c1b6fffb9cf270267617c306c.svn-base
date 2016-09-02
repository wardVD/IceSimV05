#include "recclasses/I3TimeCharacteristicsValues.h"
#include "icetray/serialization.h"
#include "icetray/I3Units.h"

#include <ostream>

template <class Archive>
void
I3TimeCharacteristicsValues::
serialize(Archive& ar, unsigned version)
{
    if(version > i3timecharacteristics_version_)
        log_fatal("Attempting to read version %u from file but running version "
                  "%u of I3TimeCharacteristics class.",
                  version, i3timecharacteristics_version_
        );

    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("TimeLengthFWHM",    this->timelength_fwhm_);
    ar & make_nvp("TimeLengthLastFirst",    this->timelength_last_first_);
    ar & make_nvp("TimeLengthMaxGap",    this->timelenght_maxgap_);
    ar & make_nvp("ZPattern", this->zpattern_);

}

I3_SERIALIZABLE(I3TimeCharacteristicsValues);

std::ostream&
operator<<(std::ostream& oss, const I3TimeCharacteristicsValues& rhs)
{
    oss << "I3TimeCharacteristicsValues("                                                  <<
        "TimeLengthFWHM [ns]: "    << rhs.GetTimeLengthFwhm()/I3Units::ns << ", " <<
        "TimeLengthLastFirst [ns]: "    << rhs.GetTimeLengthLastFirst()/I3Units::ns << ", " <<
        "TimeLengthMaxGap [ns]: "    << rhs.GetTimeLenghtMaxgap()/I3Units::ns << ", " <<
        "ZPattern: "    << rhs.GetZPattern() << ", " <<
        ")";
    return oss;

}
