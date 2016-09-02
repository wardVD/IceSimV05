// 
// copyright  (C) 2012
// Jakob van Santen <vansanten@wisc.edu>
// The Icecube Collaboration: http://www.icecube.wisc.edu
// 
// I3PMTSaturationFlagger finds the times during which the PMT output
// current exceeded a set threshold. This can be used to mark regions
// where the PMT output is expected to be nonlinear.
//
// $Id$
// 
// @version $Revision$
// @date $LastChangedDate$
// @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
//

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Units.h>

#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3Waveform.h>
#include <dataclasses/I3TimeWindow.h>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

class I3PMTSaturationFlagger : public I3ConditionalModule {
public:
	I3PMTSaturationFlagger(const I3Context&);
	void Configure();
	void Calibration(I3FramePtr);
	void DetectorStatus(I3FramePtr);
	void DAQ(I3FramePtr);
private:
	void CalculateThresholds();
	
	I3CalibrationConstPtr calibration_;
	I3DetectorStatusConstPtr status_;
	std::map<OMKey, double> thresholds_;
	double threshold_;
	std::string waveformName_, outputName_;
	
	SET_LOGGER("I3PMTSaturationFlagger");
};

I3_MODULE(I3PMTSaturationFlagger);

I3PMTSaturationFlagger::I3PMTSaturationFlagger(const I3Context &ctx) : I3ConditionalModule(ctx)
{
	AddOutBox("OutBox");
	
	AddParameter("Waveforms", "Name of the I3WaveformSeriesMap "
	    "to examine", "CalibratedWaveforms");
	AddParameter("Output", "Name of the I3TimeWindowSeriesMap "
	    "to put in the frame", "SaturationWindows");
	AddParameter("CurrentTreshold", "Flag any time span where the PMT "
	    "output current exceeds this value", 50*I3Units::mA);
}

void
I3PMTSaturationFlagger::Configure()
{
	GetParameter("Waveforms", waveformName_);
	GetParameter("Output", outputName_);
	GetParameter("CurrentTreshold", threshold_);
}

void
I3PMTSaturationFlagger::Calibration(I3FramePtr frame)
{
	calibration_ = frame->Get<I3CalibrationConstPtr>();
	if (calibration_ && status_)
		CalculateThresholds();
	PushFrame(frame);
}

void
I3PMTSaturationFlagger::DetectorStatus(I3FramePtr frame)
{
	status_ = frame->Get<I3DetectorStatusConstPtr>();
	if (calibration_ && status_)
		CalculateThresholds();
	PushFrame(frame);
}

void
I3PMTSaturationFlagger::CalculateThresholds()
{
	thresholds_.clear();
	BOOST_FOREACH(const I3DOMCalibrationMap::value_type &pair, calibration_->domCal) {
		I3DOMStatusMap::const_iterator stat = status_->domStatus.find(pair.first);
		if (stat == status_->domStatus.end())
			continue;
		// TODO: gain-dependent thresholds
		thresholds_[pair.first] = threshold_*pair.second.GetFrontEndImpedance();
	}
}

void
I3PMTSaturationFlagger::DAQ(I3FramePtr frame)
{	
	I3WaveformSeriesMapConstPtr wfmap =
	    frame->Get<I3WaveformSeriesMapConstPtr>(waveformName_);

	if (!wfmap) {
		log_warn("No I3WaveformSeriesMap '%s' in the frame!", waveformName_.c_str());
		PushFrame(frame);
		return;
	}
	
	I3TimeWindowSeriesMapPtr windows = boost::make_shared<I3TimeWindowSeriesMap>();
	BOOST_FOREACH(const I3WaveformSeriesMap::value_type &pair, *wfmap) {
		std::map<OMKey, double>::const_iterator threshit = thresholds_.find(pair.first);
		if (threshit == thresholds_.end())
			continue;
		
		I3TimeWindowSeries spans;
		BOOST_FOREACH(const I3Waveform &wf, pair.second) {
			double tstart = wf.GetStartTime();
			bool over = false;
			int i = 0;
			BOOST_FOREACH(double v, wf.GetWaveform()) {
				if (v > threshit->second) {
					if (!over)
						tstart = wf.GetStartTime() + i*wf.GetBinWidth();
					over = true;
				} else if (over) {
					spans.push_back(I3TimeWindow(tstart,
					    wf.GetStartTime() + i*wf.GetBinWidth()));
					tstart = wf.GetStartTime() + i*wf.GetBinWidth();
					over = false;
				}
				i++;
			}
			if (over)
				spans.push_back(I3TimeWindow(tstart,
				    wf.GetStartTime() + i*wf.GetBinWidth()));
		}
		if (spans.size() > 0)
			(*windows)[pair.first].swap(spans);
		
	}
	
	if (windows->size() > 0)
		frame->Put(outputName_, windows);
	
	PushFrame(frame);	
}
