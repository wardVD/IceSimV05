#include <icetray/serialization.h>
#include "recclasses/I3TensorOfInertiaFitParams.h"

template <class Archive>
void I3TensorOfInertiaFitParams::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("mineval",  mineval );
  ar & make_nvp("evalratio", evalratio );
  ar & make_nvp("eval2", eval2 );
  ar & make_nvp("eval3", eval3 );
}

I3_SERIALIZABLE(I3TensorOfInertiaFitParams);
