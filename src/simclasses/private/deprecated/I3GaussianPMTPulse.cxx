#include <vector>
#include <icetray/serialization.h>
#include <icetray/I3FrameObject.h>
#include <icetray/OMKey.h>
#include <dataclasses/I3Map.h>

/**
 * @brief Implementation of PMTPulse for Gaussian pulse shape
 * 
 * This class records the true (simulated) voltage, as a function of
 * time, produced by a single photoelectron (hit), using a Gaussian
 * model. 
 */
class I3GaussianPMTPulse : public I3FrameObject {

  double normalization_;
  double sigma_;
  double timeZero_;
  double pedestal_;

  friend class boost::serialization::access;

  template <class Archive> void serialize(Archive & ar, unsigned version);


public:
  virtual ~I3GaussianPMTPulse(){};
};

typedef I3Map<OMKey, std::vector<I3GaussianPMTPulse> > I3GaussianPMTPulseListMap;
I3_POINTER_TYPEDEFS(I3GaussianPMTPulseListMap);

template <class Archive>
    void I3GaussianPMTPulse::serialize(Archive& ar, unsigned version){
    make_nvp("Normalization",normalization_);
    make_nvp("Sigma",sigma_);
    make_nvp("TimeZero",timeZero_);
    make_nvp("Pedestal",pedestal_);
  }

I3_SERIALIZABLE(I3GaussianPMTPulseListMap);
