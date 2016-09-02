#include "recclasses/I3DipoleFitParams.h"
#include "recclasses/Utility.h"
#include <icetray/serialization.h>

template <class Archive>
void I3DipoleFitParams::serialize(Archive& archive, unsigned version)
{
  archive & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  archive & make_nvp("magnet", magnet);
  archive & make_nvp("magnetX", magnetX);
  archive & make_nvp("magnetY", magnetY);
  archive & make_nvp("magnetZ", magnetZ);
  archive & make_nvp("ampSum", ampSum);
  archive & make_nvp("nHits", nHits);
  archive & make_nvp("nPairs", nPairs);
  archive & make_nvp("maxAmp", maxAmp);
}

I3_SERIALIZABLE(I3DipoleFitParams);

bool I3DipoleFitParams::operator==(const I3DipoleFitParams& other) const
{
  return nan_aware_equality(magnet, other.magnet) &&
         nan_aware_equality(magnetX, other.magnetX) &&
         nan_aware_equality(magnetY, other.magnetY) &&
         nan_aware_equality(magnetZ, other.magnetZ) &&
         nan_aware_equality(ampSum, other.ampSum) &&
         nHits == other.nHits &&
         nPairs == other.nPairs &&
         nan_aware_equality(maxAmp, other.maxAmp);
}

std::ostream& operator<<(std::ostream& os,
                         const I3DipoleFitParams& x)
{
  os << "[ I3DipoleFitParams::"
        "\n  magnet : " << x.magnet <<
        "\n  magnetX: " << x.magnetX <<
        "\n  magnetY: " << x.magnetY <<
        "\n  magnetZ: " << x.magnetZ <<
        "\n  ampSum : " << x.ampSum <<
        "\n  nHits  : " << x.nHits <<
        "\n  nPairs : " << x.nPairs <<
        "\n  maxAmp : " << x.maxAmp <<
        " ]";
  return os;
}
