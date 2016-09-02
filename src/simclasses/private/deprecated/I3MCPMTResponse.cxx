#include <icetray/serialization.h>
#include "icetray/I3FrameObject.h"
#include "icetray/I3Tray.h"
#include "dataclasses/I3Map.h"
#include "icetray/OMKey.h"

/**
 * @brief Implementation class for PMT response, simulated by ROMEO
 *
 * This class contains the PMT-level (pre-DAQ/readout) response to the
 * hits in the event.  It represents the voltage produced at the PMT output.
 *
 */
class I3MCPMTResponse : public I3FrameObject{
  std::vector<double> waveform_;
  double binSize_;
  double startTime_;
  double endTime_;

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);

public:
  virtual ~I3MCPMTResponse(){};

};
typedef I3Map<OMKey, I3MCPMTResponse> I3MCPMTResponseMap;

template <class Archive>
    void I3MCPMTResponse::serialize(Archive& ar, unsigned version){
    ar & make_nvp("Waveform",waveform_);
    ar & make_nvp("BinSize",binSize_);
    ar & make_nvp("startTime",startTime_);
    ar & make_nvp("endTime",endTime_);
  }

I3_SERIALIZABLE(I3MCPMTResponse);
I3_SERIALIZABLE(I3MCPMTResponseMap);
