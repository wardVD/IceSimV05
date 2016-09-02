
#include "recclasses/I3Veto.h"
#include <icetray/serialization.h>
#include <algorithm>
#include <vector>
#include <set>

/*
namespace I3VetoDetail {
    struct DOMInfo {
        OMKey key;
        double charge;
        double firstPulseTime;

        bool operator<(const DOMInfo& other) const{
            return this->firstPulseTime < other.firstPulseTime;
        }
    };
}
*/

I3Veto::I3Veto() :
    nUnhitTopLayers(-1),
    nLayer(-1),
    earliestLayer(-1),
    earliestOM(-1),
    earliestContainment(-1),
    latestLayer(-1),
    latestOM(-1),
    latestContainment(-1),
    mostOuterLayer(-1),
    depthHighestHit(-HUGE_VAL),
    depthFirstHit(-HUGE_VAL),
    maxDomChargeLayer(-1),
    maxDomChargeString(-1),
    maxDomChargeOM(-1),
    nDomsBeforeMaxDOM(-1),
    maxDomChargeLayer_xy(-1),
    maxDomChargeLayer_z(-1),
    maxDomChargeContainment(-1)
{
}

I3Veto::~I3Veto() {
}


template <class Archive>
void I3Veto::serialize(Archive& ar, unsigned version) {
  ar & make_nvp("I3FrameObject",           base_object<I3FrameObject>(*this));
  ar & make_nvp("earliestContainment",     earliestContainment );
  ar & make_nvp("maxDomChargeContainment", maxDomChargeContainment );
  ar & make_nvp("nUnhitTopLayers",         nUnhitTopLayers );
  ar & make_nvp("nLayer",                  nLayer );
  ar & make_nvp("earliestLayer",           earliestLayer );
  ar & make_nvp("earliestOM",              earliestOM );
  ar & make_nvp("latestLayer",             latestLayer );
  ar & make_nvp("latestOM",                latestOM );
  ar & make_nvp("latestContainment",       latestContainment );
  ar & make_nvp("mostOuterLayer",          mostOuterLayer );
  ar & make_nvp("depthHighestHit",         depthHighestHit );
  ar & make_nvp("depthFirstHit",           depthFirstHit );
  ar & make_nvp("maxDomChargeLayer",       maxDomChargeLayer  );
  ar & make_nvp("maxDomChargeLayer_xy",    maxDomChargeLayer_xy );
  ar & make_nvp("maxDomChargeLayer_z",     maxDomChargeLayer_z );
  ar & make_nvp("maxDomChargeString",      maxDomChargeString  );
  ar & make_nvp("maxDomChargeOM",          maxDomChargeOM  );
  ar & make_nvp("nDomsBeforeMaxDOM",       nDomsBeforeMaxDOM  );
}

BOOST_CLASS_VERSION(I3Veto, 0);
I3_SERIALIZABLE(I3Veto);


// The short version of the I3Veto container

I3VetoShort::I3VetoShort() : 
  earliestContainment(-1),
  maxDomChargeContainment(-1) {
}

I3VetoShort::~I3VetoShort() {
}

template <class Archive> void I3VetoShort::serialize(Archive& ar, unsigned version) {
  ar & make_nvp("I3FrameObject",           base_object<I3FrameObject>(*this));
  ar & make_nvp("earliestContainment",     earliestContainment );
  ar & make_nvp("maxDomChargeContainment", maxDomChargeContainment );
}

BOOST_CLASS_VERSION(I3VetoShort, 0);
I3_SERIALIZABLE(I3VetoShort);

