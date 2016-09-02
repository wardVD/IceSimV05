/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Jakob van Santen <vansanten@wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include "WaveCalibrator/I3WaveCalibrator.h"
#include "icetray/I3Units.h"

#define OLD_TOROID(calibration) (calibration.GetToroidType()==I3DOMCalibration::OLD_TOROID)

/*

 * Intermediate convolution kernel that approximately maps the lower-gain
 * channels onto the higher-gain ones The kernel is derived from Chris Wendt's
 * pulse templates:
 * http://www.icecube.wisc.edu/~chwendt/dom-spe-waveform-shape/
 *
 * The code to determine the templates can be found here:
 * http://code.icecube.wisc.edu/svn/sandbox/jvansanten/pulsefitter/resources/examples/fit_intermediate_kernels.py
 *
 * FIXME: These normalizations assume that the relative gains of the
 * ATWD and FADC are properly calibrated. If this is not the case
 * (and it is off by up to 15% in some DOMs), the patched FADC
 * waveform will over- or under-estimate the true input voltage,
 * and droop correction will fail.
 */

namespace I3WaveCalibratorDroopCorrection {

struct SPETemplate { double c, x0, b1, b2; };

const SPETemplate ATWDIntermediateKernelNT_0_1 = {
	1.264408,
	-0.080267,
	0.801379,
	15.469398
};
const SPETemplate ATWDIntermediateKernelNT_0_2 = {
	3.703271,
	-2.623006,
	2.737699,
	17.279238
};
const SPETemplate ATWDIntermediateKernelNT_1_2 = {
	9.345833,
	-4.231835,
	2.481484,
	11.030858
};
const SPETemplate ATWDIntermediateKernelOT_0_1 = {
	0.901431,
	-0.244405,
	0.667990,
	17.548444
};
const SPETemplate ATWDIntermediateKernelOT_0_2 = {
	0.584898,
	-2.303411,
	0.970086,
	26.540349
};
const SPETemplate ATWDIntermediateKernelOT_1_2 = {
	5.647132,
	-4.359512,
	1.992141,
	12.033357
};
const SPETemplate FADCIntermediateKernelNT_0 = {
	0.307512,
	8.604226,
	27.379807,
	187.404273
};
const SPETemplate FADCIntermediateKernelNT_1 = {
	0.312819,
	12.195611,
	27.791027,
	187.577871
};
const SPETemplate FADCIntermediateKernelNT_2 = {
	0.318635,
	11.384870,
	27.968044,
	186.714228
};
const SPETemplate FADCIntermediateKernelOT_0 = {
	0.314336,
	9.271928,
	27.818278,
	187.254423
};
const SPETemplate FADCIntermediateKernelOT_1 = {
	0.321027,
	12.854169,
	28.311563,
	187.414187
};
const SPETemplate FADCIntermediateKernelOT_2 = {
	0.326916,
	12.237234,
	28.611787,
	187.034238
};

static const SPETemplate*
GetIntermediateKernel(const I3Waveform &ref, const I3Waveform &target, bool old_toroid)
{
	const SPETemplate *templ;
	
	if (target.GetDigitizer() == ref.GetDigitizer()) {
		/* Case 0: ATWD->ATWD correction */
		if (target.GetChannel() == 0) {
			if (ref.GetChannel() == 1)
				templ = (old_toroid ? &ATWDIntermediateKernelOT_0_1 : &ATWDIntermediateKernelNT_0_1);
			else
				templ = (old_toroid ? &ATWDIntermediateKernelOT_0_2 : &ATWDIntermediateKernelNT_0_2);
		} else {
				templ = (old_toroid ? &ATWDIntermediateKernelOT_1_2 : &ATWDIntermediateKernelNT_1_2);
		}
	} else {
		/* Case 1: ATWD->FADC correction */
		switch (ref.GetChannel()) {
			case 0:
				templ = (old_toroid ? &FADCIntermediateKernelOT_0 : &FADCIntermediateKernelNT_0);
				break;
			case 1:
				templ = (old_toroid ? &FADCIntermediateKernelOT_1 : &FADCIntermediateKernelNT_1);
				break;
			default:
				templ = (old_toroid ? &FADCIntermediateKernelOT_2 : &FADCIntermediateKernelNT_2);
		}
	}
	
	return templ;
}

static double
IntermediateDroopKernel(const SPETemplate *p, double t)
{
	return p->c*pow(exp(-(t - p->x0)/p->b1) + exp((t - p->x0)/p->b2),-8);
}

} // namespace I3WaveCalibratorDroopCorrection

inline const I3Waveform *
GetUnsaturatedWaveform(const std::vector<I3Waveform> &atwds)
{
	if (atwds.size() == 0)
		return NULL;
	
	std::vector<I3Waveform>::const_iterator it = atwds.begin();
	for ( ; it != atwds.end(); it++)
		if (!(I3Waveform::GetStatus(it->GetWaveformInformation())
		    & I3Waveform::SATURATED))
			return (&*it);
	
	return &atwds.back();
}

/*
 * Dual-tau droop correction parameters copied from DOMcalibrator.
 */
I3WaveCalibrator::DroopParams
I3WaveCalibrator::GetDroopParams(const I3DOMCalibration &calib, const double dt)
{
	DroopParams params;
	const double temperature = calib.GetTemperature() - 273.0; /* Parameterization is in Celcius */
	const TauParam &tp = calib.GetTauParameters();

	const double timeConstant1 = (tp.P1/(1+exp(-(temperature/tp.P2))) + tp.P0)*I3Units::ns;
	const double timeConstant2 = (tp.P4/(1+exp(-(temperature/tp.P5))) + tp.P3)*I3Units::ns;

	const double Tau1OverSampleWidth = timeConstant1/dt;
	const double Tau2OverSampleWidth = timeConstant2/dt;

	const double Exp1 = exp(-1./Tau1OverSampleWidth);
	const double Exp2 = exp(-1./Tau2OverSampleWidth);

	const double A1 = Tau1OverSampleWidth*(1.- Exp1);
	const double A2 = Tau2OverSampleWidth*(1.- Exp2);

	const double C1 = (1. - tp.TauFrac) * Tau1OverSampleWidth;
	const double C2 = tp.TauFrac * Tau2OverSampleWidth;

	const double w1 = C1/(C1+C2);
	const double w2 = C2/(C1+C2);

	params.coef0 = 1./(w1*A1 + w2*A2);
	params.coef1 = w1*A1*A1/Tau1OverSampleWidth;
	params.coef2 = w2*A2*A2/Tau2OverSampleWidth;
	params.exp1 = Exp1;
	params.exp2 = Exp2;
	params.timeConstant1 = timeConstant1;
	params.timeConstant2 = timeConstant2;
	
	return params;
}

void
I3WaveCalibrator::CorrectPedestalDroop(std::vector<std::vector<I3Waveform> > &atwds,
    std::vector<I3Waveform> &fadcs, I3TimeWindowSeries &errata, const I3DOMCalibration &calib)
{
	std::vector<std::vector<I3Waveform> >::iterator atwd_bundle_it = atwds.begin();
	std::vector<I3Waveform>::iterator fadc_it = fadcs.begin();
	DroopRemainder remains;
	
	for ( ; fadc_it != fadcs.end(); atwd_bundle_it++, fadc_it++) {
		/* 
		 * Since CorrectFADC modifies the droop remainder, we
		 * need a copy for the ATWD calibration.
		 */
		const DroopRemainder tmp_remains = remains; 
		const I3Waveform *unsat_ptr =
		    GetUnsaturatedWaveform(*atwd_bundle_it); // Copy before correction
		I3Waveform unsat;
		if (unsat_ptr != NULL) {
			unsat = *unsat_ptr;
			unsat_ptr = &unsat;
		}
		
		CorrectDroop(*fadc_it, unsat_ptr, remains, errata, calib);
		
		std::vector<I3Waveform>::iterator atwd_it = atwd_bundle_it->begin();
		for ( ; atwd_it != atwd_bundle_it->end(); atwd_it++) {
			DroopRemainder tmp2_remains = tmp_remains;
			CorrectDroop(*atwd_it, unsat_ptr,
			    tmp2_remains, errata, calib);
		}
	}
}

void
I3WaveCalibrator::CorrectDroop(I3Waveform &wf, const I3Waveform *ref_wf,
    DroopRemainder &leftovers, I3TimeWindowSeries &errata, const I3DOMCalibration &calib)
{
	if (wf.GetWaveform().size() == 0)
		return;
	
	const DroopParams params = GetDroopParams(calib, wf.GetBinWidth());
	
	/*
	 * Let reaction voltages measured from the previous FADC waveform
	 * decay over the gap between the two waveforms. For the first launch,
	 * we assume that the toroid has had sufficient time to recover and
	 * set these initial reaction voltages to zero.
	 */
	const double scale = 25.0*I3Units::ns/wf.GetBinWidth(); /* Account for sampling rate */
	double S1 = scale*leftovers.react1*exp(-(wf.GetStartTime()-leftovers.time)/params.timeConstant1);
	double S2 = scale*leftovers.react2*exp(-(wf.GetStartTime()-leftovers.time)/params.timeConstant2);
	double X = 0.;
	
	if (leftovers.time != 0)
		log_trace("starting droop correction with S1: %g mV S2: %g mV (dt: %g ns)",
		    S1/I3Units::mV, S2/I3Units::mV, wf.GetStartTime()-leftovers.time);
	
	std::vector<double> &wave = wf.GetWaveform();
	const unsigned nbins = wave.size();
	std::vector<I3Waveform::StatusCompound> &winfo = 
	    wf.GetWaveformInformation();
	std::vector<I3Waveform::StatusCompound>::iterator status_it = winfo.begin();
	
	for ( ; status_it != winfo.end(); status_it++) {
		const unsigned start = status_it->GetInterval().first;
		const unsigned end = status_it->GetInterval().second;
		bool unrecoverable = false;
		
		if (status_it->GetStatus() == I3Waveform::SATURATED && ref_wf != NULL) {
			/*
			 * The digitizer is topped out at its maximum value.
			 * In order to properly account for the undershoot
			 * later, we reconstruct this waveform from a
			 * reference [ATWD] waveform that is the least saturated.
			 */
			const std::vector<double> &ref = ref_wf->GetWaveform();
			const double ref_dt = ref_wf->GetBinWidth();
			const double ref_t0 = ref_wf->GetStartTime();
			const double wf_dt = wf.GetBinWidth();
			const double wf_t0 = wf.GetStartTime();
			
			const I3WaveCalibratorDroopCorrection::SPETemplate
			    *intermediate_kernel =
			    I3WaveCalibratorDroopCorrection::GetIntermediateKernel(*ref_wf,
			    wf, OLD_TOROID(calib));
			
			for (unsigned i = start; i < end; i++) {
				const double t0 = wf_t0-ref_t0 + (i+1)*wf_dt;
				const unsigned jmax = std::min(ref_wf->GetWaveform().size(), size_t(t0/ref_dt - 1));
				double patched = 0;
				
				for (unsigned j = 0; j < jmax; j++) {
					const double t = t0 - (j+1)*ref_dt;
					patched += ref_dt*ref[j]*IntermediateDroopKernel(
					    intermediate_kernel, t);
				}
				
			
				S1 = X + params.exp1*S1;
				S2 = X + params.exp2*S2;
				X = params.coef0*(patched + params.coef1*S1 + params.coef2*S2);
				
				/* 
				 * Add the correction to the saturated waveform.
				 * Since droop mimics a time-dependent bias voltage,
				 * it effectively allows the digitizer to record larger
				 * voltages than would otherwise be in range.
				 */
				wave[i] += X-patched;
			}
			
			/* 
			 * Mark the waveform as unrecoverable if saturation extends
			 * past the ATWD.
			 */
			const unsigned patch_start = std::max(start,
			    (unsigned)ceil((ref_t0-wf_t0)/wf_dt));
			const unsigned patch_end = std::min(end,
			    	(unsigned)ceil((ref_t0+ref_wf->GetWaveform().size()*ref_dt-wf_t0)/wf_dt));
			if (patch_start > start || patch_end < end ||
			    (ref_wf->GetStatus() | I3Waveform::SATURATED) ||
			    (ref_wf->GetStatus() | I3Waveform::UNDERSHOT))
				unrecoverable = true;

		} else if (status_it->GetStatus() == I3Waveform::UNDERSHOT) {
			/* 
			 * The digitizer is floored at zero. Assume zero input
			 * voltage and allow the reaction voltages to decay
			 * for the duration of the undershoot.
			 */
			X = 0.;
			const double gap = (end-start)*wf.GetBinWidth();
			S1 *= exp(-gap/params.timeConstant1);
			S2 *= exp(-gap/params.timeConstant2);
			for (unsigned bin = start; bin < end; bin++)
				wave[bin] = 0.;
				
			unrecoverable = true;
		} else {
			/* 
			 * Sanity prevails; the digitizer is in range.
			 * Proceed with bog-standard droop correction.
			 */
			for (unsigned bin = start; bin < end; bin++) {
				S1 = X + params.exp1*S1;
				S2 = X + params.exp2*S2;
				X = params.coef0*(wave[bin] + params.coef1*S1 + params.coef2*S2);
				wave[bin] = X;
			}

			/*
			 * Small remaining possibility of insanity:
			 * saturation with no reference waveform will
			 * end up here. Do what we can (above) and then
			 * pronounce the patient dead.
			 */

			if (status_it->GetStatus() == I3Waveform::SATURATED)
				unrecoverable = true;
		}
		
		if (unrecoverable) {
			/*
			 * Mark the readout as permanently corrupted at
			 * the end of the reference waveform (if any) or
			 * the beginning of the block, whichever
			 * comes later.
			 */
			double valid_end = wf.GetStartTime() + start*wf.GetBinWidth();
			double ref_end = ref_wf ? ref_wf->GetStartTime() +
			    ref_wf->GetWaveform().size()*ref_wf->GetBinWidth()
			    : -std::numeric_limits<double>::infinity();
			    
			errata.push_back(I3TimeWindow(std::max(valid_end, ref_end),
			    std::numeric_limits<double>::infinity()));
		}
			

	}
	
	/* Save remaining reaction voltages for the next launch. */
	leftovers.react1 = S1;
	leftovers.react2 = S2;
	leftovers.time = wf.GetStartTime() + nbins*wf.GetBinWidth();
}

