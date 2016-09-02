#include <boost/preprocessor.hpp>
#include "recclasses/I3Veto.h"
#include "dataclasses/physics/I3Particle.h"
#include "wrap.h"

using namespace boost::python;


void register_I3Veto() {
    #define VETOFIELDS (nUnhitTopLayers)(nLayer)(earliestLayer)(earliestOM)(earliestContainment)(latestLayer)(latestOM)(latestContainment)       \
                       (mostOuterLayer)(depthHighestHit)(depthFirstHit)(maxDomChargeLayer)(maxDomChargeLayer_xy)(maxDomChargeLayer_z)            \
                       (maxDomChargeString)(maxDomChargeOM)(nDomsBeforeMaxDOM)(maxDomChargeContainment)
    wrap::I3FrameObject<I3Veto>()
        BOOST_PP_SEQ_FOR_EACH(WRAP_RO, I3Veto, VETOFIELDS)
    ;

    #define PROPERTIES (earliestContainment)(maxDomChargeContainment)
    wrap::I3FrameObject<I3VetoShort>()
        BOOST_PP_SEQ_FOR_EACH(WRAP_RO, I3VetoShort, PROPERTIES)
    ;
}
