#include <icetray/I3ConditionalModule.h>
#include <dataclasses/TankKey.h>
#include <dataclasses/physics/I3Waveform.h>

#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/geometry/I3OMGeo.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/calibration/I3DOMCalibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/status/I3DOMStatus.h>

#include <dataclasses/I3DOMFunctions.h>

#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/I3DOMFunctions.h>
#include <simclasses/I3MCPE.h>

#include <boost/make_shared.hpp>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>

using namespace std;

/**
 * \brief I3TopAddComponentWaveforms module adds "waveforms" and "pulses" for each air shower component.
 *
 * The "waveforms" are just histograms of photoelectron times with a binning that matches the actual waveforms' sampling. The "pulses" are just the integral of the waveforms.
 */

namespace {
  struct Binning {
    Binning(OMKey o, double s, double e, int l):
      om(o), start(s), end(e), n(l)
    {}
    OMKey om;
    double start;
    double end;
    int n;
  };

}

class I3TopAddComponentWaveforms : public I3ConditionalModule
{

public:
  I3TopAddComponentWaveforms(const I3Context& context): I3ConditionalModule(context)
  {
    AddParameter("Launches", "Input", "IceTopRawData");
    AddParameter("WaveformsIn", "Input", "");
    AddParameter("PESeriesMap", "Input", "MCTopPESeriesMap");
    AddParameter("AirShowerComponents", "Map of component names -> ID", "AirShowerComponents");
    AddParameter("Waveforms", "Output", "IceTopComponentWaveforms_");
    AddParameter("Pulses", "Output", "IceTopComponentPulses_");
    AddOutBox("OutBox");
  }

  ~I3TopAddComponentWaveforms(){}

  void Configure()
  {
    GetParameter("Launches", launches_);
    GetParameter("WaveformsIn", waveforms_in_);
    GetParameter("PESeriesMap", pe_series_map_);
    GetParameter("AirShowerComponents", components_);
    GetParameter("Waveforms", waveforms_);
    GetParameter("Pulses", pulses_);
  }

  void DAQ(I3FramePtr frame)
  {
    I3MapStringIntConstPtr components = frame->Get<I3MapStringIntConstPtr>(components_);
    I3DOMLaunchSeriesMapConstPtr launches = frame->Get<I3DOMLaunchSeriesMapConstPtr>(launches_);
    I3WaveformSeriesMapConstPtr waveforms_in = frame->Get<I3WaveformSeriesMapConstPtr>(waveforms_in_);
    I3MCPESeriesMapConstPtr pe_series_map = frame->Get<I3MCPESeriesMapConstPtr>(pe_series_map_);

    if (!launches && !waveforms_in) {
      log_info("No raw data and no launches in frame. Not adding component waveforms.");
      PushFrame(frame);
      return;
    }
    if (!components || !pe_series_map) {
      log_info("Either no components or no PEs in frame. Not adding component waveforms.");
      PushFrame(frame);
      return;
    }

    // create the output map of I3WaveformSeriesMap (one I3WaveformSeriesMap per component in the frame)
    std::map<int, I3WaveformSeriesMapPtr> waveforms_out;
    for (I3MapStringInt::const_iterator it = components->begin(); it != components->end(); ++it) {
      waveforms_out[it->second] = boost::make_shared<I3WaveformSeriesMap>();
    }

    geometry_ = frame->Get<I3GeometryConstPtr>("I3Geometry");
    calibration_ = frame->Get<I3CalibrationConstPtr>("I3Calibration");
    detector_status_ = frame->Get<I3DetectorStatusConstPtr>("I3DetectorStatus");

    std::vector<Binning> binning;
    if (launches)
      binning = GetBinnings(launches);
    else if (waveforms_in)
      binning = GetBinnings(launches);

    for(unsigned int j = 0; j != binning.size(); ++j) {
      const OMKey& om = binning[j].om;

      const std::map<OMKey, I3OMGeo>::const_iterator geo = (geometry_->omgeo.find(om));
      // const std::map<OMKey, I3DOMCalibration>::const_iterator calib = (calibration_->domCal.find(om));
      // const std::map<OMKey, I3DOMStatus>::const_iterator status = (detector_status_->domStatus.find(om));

      if (geo->second.omtype == I3OMGeo::IceCube)
	continue;

      for (I3MapStringInt::const_iterator it = components->begin(); it != components->end(); ++it) {
	if (waveforms_out[it->second]->find(om) == waveforms_out[it->second]->end())
	  (*waveforms_out[it->second])[om] = I3WaveformSeries();
      }
      I3MCPESeries pe_series = pe_series_map->find(om)->second;
      const double start = binning[j].start;
      const double end = binning[j].end;
      const double width = (end - start)/binning[j].n;

      // create an empty list of arrays for each component
      std::map<int, std::vector<double> > w;
      for (I3MapStringInt::const_iterator it = components->begin(); it != components->end(); ++it) {
	w[it->second] = std::vector<double>(binning[j].n, 0);
      }

      // iterate over PEs filling the arrays
      double npe = 0;
      for (unsigned int pei = 0; pei != pe_series.size(); ++pei) {
	const I3MCPE pe = pe_series[pei];
	if (pe.time < end && pe.time >= start) {
	  const unsigned int index = floor((pe.time - start)/width);
	  assert(index < w[pe.ID.minorID].size());
	  w[pe.ID.minorID][index] += pe.npe;
	  npe += pe.npe;
	  }
      }
      if (npe <= 0.) {// WTF? I have seen empty imput waveforms...
	continue;
      }

      // turn the arrays into waveforms
      for (std::map<int, std::vector<double> >::iterator wit = w.begin(); wit != w.end(); ++wit) {
	const int component = wit->first;
	I3Waveform waveform;
	waveform.SetWaveform(wit->second);
	//waveform.SetHLC(waveform_in.IsHLC());
	waveform.SetStartTime(start);
	waveform.SetBinWidth(width);
	//waveform.SetDigitizer(waveform_in.GetDigitizer());
	(*waveforms_out[component])[om].push_back(waveform);
      }
    } // iteration over binning ranges

    // create the output map of RecoPulseSeriesMap (one RecoPulseSeriesMap per component in the frame)
    std::map<int, I3RecoPulseSeriesMapPtr> pulses_out;
    for (I3MapStringInt::const_iterator it = components->begin(); it != components->end(); ++it) {
      pulses_out[it->second] = boost::make_shared<I3RecoPulseSeriesMap>();
      for (I3WaveformSeriesMap::const_iterator waveform_it = waveforms_out[it->second]->begin(); waveform_it != waveforms_out[it->second]->end(); ++waveform_it) {
	for (unsigned int i = 0; i != waveform_it->second.size(); ++i) {
	  const std::vector<double>& w = waveform_it->second[i].GetWaveform();
	  I3RecoPulse p;
	  p.SetTime(waveform_it->second[i].GetStartTime());
	  p.SetCharge(std::accumulate(w.begin(), w.end(), 0));
	  (*pulses_out[it->second])[waveform_it->first].push_back(p);
	}
      }
    }

    if (waveforms_ != "") {
      for (I3MapStringInt::const_iterator it = components->begin(); it != components->end(); ++it) {
	std::string name = waveforms_ + it->first;
	frame->Put(name, waveforms_out[it->second]);
      }
    }
    if (pulses_ != "") {
      for (I3MapStringInt::const_iterator it = components->begin(); it != components->end(); ++it) {
	std::string name = pulses_ + it->first;
	frame->Put(name, pulses_out[it->second]);
      }
    }
    PushFrame(frame);
  }

private:

  std::vector<Binning> GetBinnings(I3WaveformSeriesMapConstPtr waveforms_in) const;
  std::vector<Binning> GetBinnings(I3DOMLaunchSeriesMapConstPtr launches) const;

  double GetStartTime(const I3DOMLaunch &launch,
		      const I3DOMCalibration &calib,
		      const I3DOMStatus &status,
		      bool fadc) const;

  std::string launches_;
  std::string waveforms_in_;
  std::string pe_series_map_;
  std::string components_;
  std::string waveforms_;
  std::string pulses_;

  I3GeometryConstPtr geometry_;
  I3CalibrationConstPtr calibration_;
  I3DetectorStatusConstPtr detector_status_;

  SET_LOGGER("I3TopAddComponentWaveforms");
};


double I3TopAddComponentWaveforms::GetStartTime(const I3DOMLaunch &launch,
						const I3DOMCalibration &calib,
						const I3DOMStatus &status,
						bool fadc)
  const
{
  const LinearFit &fit = calib.GetTransitTime();
  const double pmtHV = status.pmtHV/I3Units::V;
  const double offset = calib.GetATWDDeltaT(
					    (launch.GetWhichATWD() == I3DOMLaunch::ATWDa) ? 0 : 1);
  const double transit_time = fit.slope/sqrt(pmtHV) + fit.intercept + offset;
  if (fadc)
    return (launch.GetStartTime() - transit_time + calib.GetFADCDeltaT());
  else
    return (launch.GetStartTime() - transit_time);
}


std::vector<Binning>
I3TopAddComponentWaveforms::GetBinnings(I3WaveformSeriesMapConstPtr waveforms_in) const
{
  std::vector<Binning> binning;

  for (I3WaveformSeriesMap::const_iterator waveform_it = waveforms_in->begin(); waveform_it != waveforms_in->end(); ++waveform_it) {
    const OMKey& om = waveform_it->first;

    for (unsigned int i = 0; i != waveform_it->second.size(); ++i) {
      const I3Waveform &waveform_in = waveform_it->second[i];
      const double start = waveform_in.GetStartTime();
      const double width = waveform_in.GetBinWidth();
      const int n = waveform_in.GetWaveform().size();

      bool skip = false;
      for (unsigned int t = 0; t != binning.size(); ++t) {
	if ((binning[t].om == om) &&
	    (std::fabs(binning[t].start - start) < 500*I3Units::ns)) { // skip according to some time tolerance...
	  skip = true;
	  break;
	}
      }
      if (skip) continue;
      binning.push_back(Binning(om, start, start + n*width, n));

    } // iteration over waveform_it->second
  } // iteration over waveforms_in

  return binning;
}

/**
   This function replicates things done in WaveCalibrator
*/
std::vector<Binning>
I3TopAddComponentWaveforms::GetBinnings(I3DOMLaunchSeriesMapConstPtr launches) const
{
  std::vector<Binning> binning;
  for (I3DOMLaunchSeriesMap::const_iterator launch_it = launches->begin(); launch_it != launches->end(); ++launch_it) {
    const OMKey om = launch_it->first;
    const std::map<OMKey, I3DOMCalibration>::const_iterator calib = (calibration_->domCal.find(om));
    const std::map<OMKey, I3DOMStatus>::const_iterator status = (detector_status_->domStatus.find(om));

    for (unsigned int i = 0; i != launch_it->second.size(); ++i) {
      const I3DOMLaunch launch = launch_it->second[i];
      const unsigned atwd_id = (launch.GetWhichATWD() == I3DOMLaunch::ATWDa) ? 0 : 1;
      const double start = GetStartTime(launch, calib->second, status->second, false /* fadc */);
      unsigned int n = 0;
      double width;
      if (launch.GetLCBit()) {
	for (unsigned j = 0; j < 3; j++) {
	  if (launch.GetRawATWD(j).size() > n) {
	    n = launch.GetRawATWD(j).size();
	  }
	}
	width = 1./ATWDSamplingRate(atwd_id, status->second, calib->second);
      }
      else {
	n = 1;
	width = 128/ATWDSamplingRate(atwd_id, status->second, calib->second);
      }
      binning.push_back(Binning(om, start, start + n*width, n));
    }
  }
  return binning;
}



I3_MODULE(I3TopAddComponentWaveforms);
