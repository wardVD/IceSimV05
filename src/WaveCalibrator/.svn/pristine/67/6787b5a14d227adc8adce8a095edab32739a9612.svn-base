/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Jakob van Santen <vansanten@wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include "WaveCalibrator/I3WaveCalibrator.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/physics/I3EventHeader.h"
#include "dataclasses/I3TimeWindow.h"

#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

I3WaveCalibrator::I3WaveCalibrator(const I3Context &ctx) : I3ConditionalModule(ctx)
{
	launch_name_ = "InIceRawData";
	AddParameter("Launches", "Where to find the raw data in the frame", launch_name_);
	
	waveform_name_ = "CalibratedWaveforms";
	AddParameter("Waveforms", "Where to store the calibrated waveforms", waveform_name_);
	
	errata_name_ = "CalibrationErrata";
	AddParameter("Errata", "Where to store a list of OMKeys where calibration failed.", errata_name_);
	
	waveform_span_name_ = "CalibratedWaveformRange";
	AddParameter("WaveformRange", "Where to store the time of first and last possible waveform "
	    "bins for each event", waveform_span_name_);
	
	atwd_saturation_margin_ = 123;
	AddParameter("ATWDSaturationMargin", "Saturation threshold of the ATWD, "
	    "in ADC counts below the maximum value", atwd_saturation_margin_);
	
	fadc_saturation_margin_ = 0;
	AddParameter("FADCSaturationMargin", "Saturation threshold of the FADC, "
	    "in ADC counts below the maximum value", fadc_saturation_margin_);
	
	fadc_undershoot_margin_ = 2;
	
	correct_droop_ = true;
	AddParameter("CorrectDroop", "Attempt to compensate for transformer droop.", correct_droop_);
	
	waveform_shift_range_ = std::make_pair(0., 0.);
	
	AddOutBox("OutBox");
}

void
I3WaveCalibrator::Configure()
{
	GetParameter("Launches", launch_name_);
	GetParameter("Waveforms", waveform_name_);
	GetParameter("Errata", errata_name_);
	GetParameter("WaveformRange", waveform_span_name_);
	GetParameter("ATWDSaturationMargin", atwd_saturation_margin_);
	GetParameter("FADCSaturationMargin", fadc_saturation_margin_);
	GetParameter("CorrectDroop", correct_droop_);
}

void
I3WaveCalibrator::Geometry(I3FramePtr frame)
{
	geometry_ = frame->Get<I3GeometryConstPtr>();
	PushFrame(frame);
}

void
I3WaveCalibrator::Calibration(I3FramePtr frame)
{
	calibration_ = frame->Get<I3CalibrationConstPtr>();
	if (calibration_ && status_)
		waveform_shift_range_ = GetWaveformOffset(calibration_, status_);
	PushFrame(frame);
}

void
I3WaveCalibrator::DetectorStatus(I3FramePtr frame)
{
	status_ = frame->Get<I3DetectorStatusConstPtr>();
	if (calibration_ && status_)
		waveform_shift_range_ = GetWaveformOffset(calibration_, status_);
	PushFrame(frame);
}

void
I3WaveCalibrator::DAQ(I3FramePtr frame)
{
	log_trace("Starting WaveCalibrator::DAQ");
	
	I3DOMLaunchSeriesMapConstPtr launches
	    = frame->Get<I3DOMLaunchSeriesMapConstPtr>(launch_name_);

	/* Be a dear and check our inputs. */
	if (!launches || !geometry_ || !calibration_ || !status_) {
		if (!launches)
			log_error("No DOMLaunches called '%s' exist in the frame!", launch_name_.c_str());
		if (!geometry_)
			log_error("We don't have a geometry yet!");
		if (!calibration_)
			log_error("We don't have a calibration yet!");
		if (!status_)
			log_error("We don't have a detector status yet!");
		
		PushFrame(frame);
		return;
	}
	
	I3WaveformSeriesMapPtr waveform_map = boost::make_shared<I3WaveformSeriesMap>();
	I3TimeWindowSeriesMapPtr errata = boost::make_shared<I3TimeWindowSeriesMap>();
	
	I3DOMLaunchSeriesMap::const_iterator map_it = launches->begin();
	for ( ; map_it != launches->end(); map_it++) {
		const OMKey &om = map_it->first;
		const I3DOMLaunchSeries &launches = map_it->second;
		
		std::map<OMKey, I3OMGeo>::const_iterator
		    geo = geometry_->omgeo.find(om);
		std::map<OMKey, I3DOMCalibration>::const_iterator
		    calib = calibration_->domCal.find(om);
		std::map<OMKey, I3DOMStatus>::const_iterator
		    status = status_->domStatus.find(om);
		if (geo == geometry_->omgeo.end()) {
			log_error("Can't find geometry for OM(%d,%u)!",
			    om.GetString(), om.GetOM());
			continue;
		}
		if (calib == calibration_->domCal.end()) {
			log_error("Can't find calibration for OM(%d,%u)!",
			    om.GetString(), om.GetOM());
			continue;
		}
		if (status == status_->domStatus.end()) {
			log_error("Can't find status for OM(%d,%u)!",
			    om.GetString(), om.GetOM());
			continue;
		}
		
		CalibrateLaunches(om, launches, geo->second, calib->second,
			    status->second, *waveform_map, *errata);
	}
	
	frame->Put(waveform_name_, waveform_map);
	
	if (waveform_span_name_.size() > 0)
		frame->Put(waveform_span_name_,
		    GetWaveformRange(frame, waveform_shift_range_));
			
	if (errata_name_.size() > 0 && errata->size() > 0)
		frame->Put(errata_name_, errata);
	
	PushFrame(frame);
}

I3TimeWindowPtr
I3WaveCalibrator::GetWaveformRange(I3FramePtr frame, const std::pair<double,double> &offsets)
{
	if (I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>()) {
		double event_length = header->GetEndTime()-header->GetStartTime();
		return boost::make_shared<I3TimeWindow>(
			offsets.first, offsets.second + event_length + 256*25.);
	} else {
		log_error("Can't calculate an event length without an event header!");
		return boost::make_shared<I3TimeWindow>();
	}	
}

std::pair<double, double>
I3WaveCalibrator::GetWaveformOffset(I3CalibrationConstPtr calibration,
    I3DetectorStatusConstPtr status)
{
	double min_time = 0;
	double max_time = 0;
	I3DOMLaunch dl;
	dl.SetStartTime(0.);
	for (std::map<OMKey, I3DOMCalibration>::const_iterator cal
	    = calibration->domCal.begin(); cal != calibration->domCal.end(); cal++) {
		
		std::map<OMKey, I3DOMStatus>::const_iterator stat =
		    status->domStatus.find(cal->first);
		
		if (stat == status->domStatus.end() ||
		    !std::isfinite(stat->second.pmtHV) || stat->second.pmtHV <= 0)
			continue;

		double offset;
		offset = GetStartTime(dl, cal->second, stat->second, false);
		if (offset < min_time)
			min_time = offset;
		if (offset > max_time)
			max_time = offset;
		
		offset = GetStartTime(dl, cal->second, stat->second, true);
		if (offset < min_time)
			min_time = offset;
		if (offset > max_time)
			max_time = offset;
	}
	
	return std::make_pair(min_time, max_time);
}

I3_MODULE(I3WaveCalibrator);

