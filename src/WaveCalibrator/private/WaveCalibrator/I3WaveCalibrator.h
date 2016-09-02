
#ifndef I3_WAVE_CALIBRATOR_H
#define I3_WAVE_CALIBRATOR_H

/**
 *  $Id: I3WaveCalibrator.h 136293 2015-08-17 14:38:06Z jbraun $
 *  
 *  Copyright (C) 2011
 *  Jakob van Santen <vansanten@wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include "icetray/I3ConditionalModule.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/physics/I3Waveform.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/I3TimeWindow.h"

class I3WaveCalibrator : public I3ConditionalModule {
public:
	I3WaveCalibrator(const I3Context&);
	
	void Configure();
	void Geometry(I3FramePtr frame);
	void Calibration(I3FramePtr frame);
	void DetectorStatus(I3FramePtr frame);
	void DAQ(I3FramePtr frame);
	
	/*
	 * Find the minimum and maximum difference between the time
	 * of an I3DOMLaunch and the start time of the corresponding
	 * waveform.
	 */ 
	static std::pair<double, double>
	GetWaveformOffset(I3CalibrationConstPtr, I3DetectorStatusConstPtr);
	
	/*
	 * Find the times of the earliest and latest possible waveform
	 * bins in this event.
	 */
	static I3TimeWindowPtr
	GetWaveformRange(I3FramePtr frame, const std::pair<double,double> &offsets);	
	
private:
	
	/* A simple struct for holding the intermediate representation of a waveform. */
	struct WaveformBin {
		double counts; /* Store as a double to avoid casting inside loops */
		I3Waveform::Status status;
		int8_t channel;
		
		WaveformBin() : counts(0), status(I3Waveform::SATURATED), channel(0) {}
	};
	
	/* A simple struct for propagating droop correction from one waveform to the next */ 
	struct DroopRemainder {
		double react1, react2;
		double time;
		DroopRemainder() : react1(0.), react2(0.), time(0.) {};
	};
	
	/* Parameters used during dual-tau droop correction */
	struct DroopParams {
		double timeConstant1, timeConstant2;
		double coef0, coef1, coef2;
		double exp1, exp2;
	};
		
	/* Find the size of the given ATWD channel waveform, or the longest one. */
	size_t GetATWDSize(const I3DOMLaunch&, int channel = -1);
	
	/* 
	 * Extract and calibrate waveforms from a single OM. Return true if the
	 * results should be considered unreliable.
	 */
	void CalibrateLaunches(const OMKey &om, const I3DOMLaunchSeries &launches,
	    const I3OMGeo &geo, const I3DOMCalibration &calib, const I3DOMStatus &status,
	    I3WaveformSeriesMap &waveform_map, I3TimeWindowSeriesMap &errata);
	
	/* Calibrate the full ATWD readout from an HLC launch. */
	bool CalibrateATWD(const OMKey &om, const I3DOMLaunch &launch,
	    const I3DOMCalibration &calib, const I3DOMStatus &status,
	    I3Waveform &atwd, int channel);
	
	/* "Calibrate" the sum of all ATWD bins (IceTop-style SLC readout) */
	bool CalibrateATWD_SLC(const OMKey &om, const I3DOMLaunch &launch,
	    const I3DOMCalibration &calib, const I3DOMStatus &status,
	    I3Waveform &atwd);
	
	/* Calibrate the full FADC readout from an HLC launch. */
	bool CalibrateFADC(const OMKey &om, const I3DOMLaunch &launch,
	    const I3DOMCalibration &calib, const I3DOMStatus &status,
	    I3Waveform &fadc);
	
	/* Calibrate 3-bin sample from the FADC (InIce-style SLC readout) */
	bool CalibrateFADC_SLC(const OMKey &om, const I3DOMLaunch &launch,
	    const I3DOMCalibration &calib, const I3DOMStatus &status,
	    I3Waveform &fadc);
	
	/* Encode waveform status and channel info by run length. */
	void ExtractWaveformStatus(const std::vector<WaveformBin> &bins,
	    std::vector<I3Waveform::StatusCompound> &stati);
	
	/* Remove VIRGINAL/channel-0 runs from the waveform status. */
	void CompactifyWaveform(I3Waveform &waveform);
	
	/* Apply transit-time correction to the start time of a launch. */
	static double GetStartTime(const I3DOMLaunch &launch, const I3DOMCalibration &calib,
	    const I3DOMStatus &status, bool fadc = false);
	
	/* Read in raw digitizer counts from one channel of the ATWD */
	void ReadATWD(std::vector<WaveformBin> &bins, const I3DOMLaunch &launch,
	    const I3DOMCalibration &calib, const I3DOMStatus &status, int channel);
	
	/* Read in raw digitizer counts from the FADC. */
	void ReadFADC(std::vector<WaveformBin> &bins, const I3DOMLaunch &launch,
	    const I3DOMCalibration &calib, const I3DOMStatus &status);
	
	/* 
	 * Apply droop correction to a series of DOMLaunches, propagating
	 * droop correction from one launch to the next. Return true if droop
	 * correction is unreliable due to saturation or undershoot.
	 */
	void CorrectPedestalDroop(std::vector<std::vector<I3Waveform> > &atwds,
	    std::vector<I3Waveform> &fadcs, I3TimeWindowSeries &errata, const I3DOMCalibration &calib);
	
	/* 
	 * Apply droop correction to a full readout, patching with another channel
	 * where necessary and updating the droop remainder for use with the
	 * next launch. Return true if the waveform is unrecoverable; i.e.
	 * if it unrecoverably saturates or undershoots anywhere.
	 */
	void CorrectDroop(I3Waveform &wf, const I3Waveform *ref_wf,
	    DroopRemainder &leftovers, I3TimeWindowSeries &errata,
	    const I3DOMCalibration &calib);
	
	/*
	 * Apply simpler droop correction to an SLC charge stamp. This mostly
	 * just subtracts off the remaining reaction voltage from a previous
	 * launch.
	 */
	void CorrectFADC_SLC(I3Waveform &fadc,
	    DroopRemainder &leftovers, const I3DOMCalibration &calib);
	
	/* 
	 * Apply simpler droop correction to an ATWD readout. The droop remainder
	 * isn't modified, since the FADC is still on. Return true if the waveform
	 * is unrecoverable; i.e. saturates or undershoots anywhere.
	 */
	bool CorrectATWD(I3Waveform &atwd,
	    const DroopRemainder &leftovers, const I3DOMCalibration &calib);
	
	DroopParams GetDroopParams(const I3DOMCalibration &calib, const double dt);
	
	I3GeometryConstPtr geometry_;
	I3CalibrationConstPtr calibration_;
	I3DetectorStatusConstPtr status_;
	std::pair<double, double> waveform_shift_range_;
	std::string waveform_span_name_, errata_name_;
	std::string launch_name_, waveform_name_;
	int atwd_saturation_margin_, fadc_saturation_margin_;
	int fadc_undershoot_margin_;
	bool correct_droop_;
	
	SET_LOGGER("I3WaveCalibrator");
};

#endif // I3_WAVE_CALIBRATOR_H
