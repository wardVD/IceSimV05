/**
 *  $Id$
 *  
 *  Copyright (C) 2012
 *  Jakob van Santen <vansanten@wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *  Calculate and store the earliest and latest possible waveform bin
 *  edge times for a given event. This standalone version can be used
 *  for e.g. SuperDST events where waveform metadata were discarded 
 *  at the Pole.
 */

#include "WaveCalibrator/I3WaveCalibrator.h"

class I3WaveformTimeRangeCalculator : public I3ConditionalModule {
public:
	I3WaveformTimeRangeCalculator(const I3Context &c) : I3ConditionalModule(c)
	{
		waveform_span_name_ = "CalibratedWaveformRange";
		AddParameter("WaveformRange", "Where to store the time of first and last possible waveform "
		    "bins for each event", waveform_span_name_);
		
		waveform_shift_range_ = std::make_pair(0., 0.);
		
		AddOutBox("OutBox");
	}
	
	void Configure()
	{
		GetParameter("WaveformRange", waveform_span_name_);
	}
	
	void
	Calibration(I3FramePtr frame)
	{
		calibration_ = frame->Get<I3CalibrationConstPtr>();
		if (calibration_ && status_)
			waveform_shift_range_ = I3WaveCalibrator::GetWaveformOffset(calibration_, status_);
		PushFrame(frame);
	}

	void
	DetectorStatus(I3FramePtr frame)
	{
		status_ = frame->Get<I3DetectorStatusConstPtr>();
		if (calibration_ && status_)
			waveform_shift_range_ = I3WaveCalibrator::GetWaveformOffset(calibration_, status_);
		PushFrame(frame);
	}
	
	void
	DAQ(I3FramePtr frame)
	{
		frame->Put(waveform_span_name_,
		    I3WaveCalibrator::GetWaveformRange(frame, waveform_shift_range_));
		
		PushFrame(frame);
	}

private:
	I3CalibrationConstPtr calibration_;
	I3DetectorStatusConstPtr status_;
	std::string waveform_span_name_;
	std::pair<double, double> waveform_shift_range_;	
};

I3_MODULE(I3WaveformTimeRangeCalculator);
