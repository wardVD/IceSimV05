#include "recclasses/I3FillRatioInfo.h"
#include "recclasses/Utility.h"
#include <icetray/serialization.h>

template <class Archive>
void I3FillRatioInfo:: serialize(Archive& archive, unsigned version)
{
  archive & make_nvp("I3FrameObject",base_object<I3FrameObject>(*this));

  archive & make_nvp("MeanDistance",meanDistance_);
  archive & make_nvp("RMSDistance",rmsDistance_);
  archive & make_nvp("NChDistance",nChDistance_);
  archive & make_nvp("EnergyDistance",energyDistance_);
  
  archive & make_nvp("FillRadius",fillRadiusFromRMS_);
  archive & make_nvp("FillRadiusFromMean",fillRadiusFromMean_);
  archive & make_nvp("FillRadiusFromEnergy",fillRadiusFromEnergy_);
  archive & make_nvp("FillRadiusFromNCh",fillRadiusFromNCh_);
  archive & make_nvp("FillRadiusFromMeanPlusRMS",fillRadiusFromMeanPlusRMS_);
  
  archive & make_nvp("FillRatio",fillRatioFromRMS_);
  archive & make_nvp("FillRatioFromMean",fillRatioFromMean_);
  archive & make_nvp("FillRatioFromMeanPlusRMS",fillRatioFromMeanPlusRMS_);
  archive & make_nvp("FillRatioFromNCh",fillRatioFromNCh_);
  archive & make_nvp("FillRatioFromEnergy",fillRatioFromEnergy_);
  archive & make_nvp("HitCount",hitCount_);
}

I3_SERIALIZABLE(I3FillRatioInfo);

bool I3FillRatioInfo::operator==(const I3FillRatioInfo& other) const
{
  return
    nan_aware_equality(meanDistance_, other.meanDistance_) &&
    nan_aware_equality(rmsDistance_, other.rmsDistance_) &&
    nan_aware_equality(nChDistance_, other.nChDistance_) &&
    nan_aware_equality(energyDistance_, other.energyDistance_) &&
    nan_aware_equality(fillRadiusFromRMS_, other.fillRadiusFromRMS_) &&
    nan_aware_equality(fillRadiusFromMean_, other.fillRadiusFromMean_) &&
    nan_aware_equality(fillRadiusFromMeanPlusRMS_, other.fillRadiusFromMeanPlusRMS_) &&
    nan_aware_equality(fillRadiusFromNCh_, other.fillRadiusFromNCh_) &&
    nan_aware_equality(fillRadiusFromEnergy_, other.fillRadiusFromEnergy_) &&
    nan_aware_equality(fillRatioFromEnergy_, other.fillRatioFromEnergy_) &&
    nan_aware_equality(fillRatioFromMean_, other.fillRatioFromMean_) &&
    nan_aware_equality(fillRatioFromMeanPlusRMS_, other.fillRatioFromMeanPlusRMS_) &&
    nan_aware_equality(fillRatioFromNCh_, other.fillRatioFromNCh_) &&
    nan_aware_equality(fillRatioFromEnergy_, other.fillRatioFromEnergy_) &&
    hitCount_ == other.hitCount_;
}
