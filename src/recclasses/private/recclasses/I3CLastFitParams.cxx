#include <icetray/serialization.h>
#include "recclasses/I3CLastFitParams.h"
#include "recclasses/Utility.h"

template <class Archive>
void I3CLastFitParams::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("mineval",  mineval );
  ar & make_nvp("evalratio", evalratio );
  ar & make_nvp("eval2", eval2 );
  ar & make_nvp("eval3", eval3 );
}

bool I3CLastFitParams::operator==(const I3CLastFitParams& other) const
{
  return nan_aware_equality(mineval, other.mineval) &&
         nan_aware_equality(evalratio, other.evalratio) &&
         nan_aware_equality(eval2, other.eval2) &&
         nan_aware_equality(eval3, other.eval3);
}

I3_SERIALIZABLE(I3CLastFitParams);
