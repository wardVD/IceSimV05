#include "recclasses/I3FillRatioInfo.h"
#include "../recclasses/converter/I3FillRatioInfoConverter.h"
#include "wrap.h"
#include <tableio/converter/pybindings.h>
#include <boost/python.hpp>

void register_I3FillRatioInfo()
{
  using namespace boost::python;

  #define PROPERTIES (MeanDistance)(RMSDistance)(EnergyDistance) \
                     (FillRadiusFromMean)(FillRadiusFromMeanPlusRMS) \
                     (FillRadiusFromEnergy)(FillRatioFromRMS) \
                     (FillRatioFromMean)(FillRatioFromMeanPlusRMS) \
                     (FillRatioFromEnergy)(HitCount)

  wrap::I3FrameObject<I3FillRatioInfo>()
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3FillRatioInfo, PROPERTIES)
    // special cases
    .add_property("fill_radius_from_rms", &I3FillRatioInfo::GetFillRadiusFromRMS,
                                          &I3FillRatioInfo::SetFillRadius)
    // naming scheme translation doesn't work properly for NCh
    .add_property("nch_distance", &I3FillRatioInfo::GetNChDistance,
                                  &I3FillRatioInfo::SetNChDistance)
    .add_property("fillradius_from_nch", &I3FillRatioInfo::GetFillRadiusFromNCh,
                                         &I3FillRatioInfo::SetFillRadiusFromNCh)
    .add_property("fillratio_from_nch", &I3FillRatioInfo::GetFillRatioFromNCh,
                                         &I3FillRatioInfo::SetFillRatioFromNCh)       
  ;

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3FillRatioInfoConverter,
                             "Dumps I3FillRatioInfo parameter objects");
}
