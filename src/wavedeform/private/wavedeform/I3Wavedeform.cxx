#include <icetray/I3ConditionalModule.h>
#include <icetray/I3SingleServiceFactory.h>
#include <icetray/I3ServiceBase.h>

#include <dataclasses/I3DOMFunctions.h>
#include <dataclasses/I3TimeWindow.h>
#include <icetray/I3Units.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/calibration/I3DOMCalibration.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3Waveform.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/status/I3DOMStatus.h>

#include <string>
#include <vector>
#include <float.h>

#include <cholmod.h>


extern "C" {
cholmod_dense *
nnls_lawson_hanson(cholmod_sparse *A, cholmod_dense *y, double tolerance,
    unsigned min_iterations, unsigned max_iterations, unsigned npos,
    int normaleq, int solve_with_normaleq, int verbose, cholmod_common *c);
}

/* Simple class to hold together ATWD and FADC
 * templates along with a validity flag and waveform features */
struct WaveformTemplate {

    std::vector<double> atwd_template[3];
    std::vector<double> fadc_template;
    double atwdFWHMStart[3];
    double fadcFWHMStart;
    double atwdFWHMStop[3];
    double fadcFWHMStop;
    bool filled;
};

class I3Wavedeform : public I3ConditionalModule
{
	public:
		I3Wavedeform(const I3Context &);
		virtual ~I3Wavedeform();

		void Configure();
		void Calibration(I3FramePtr frame);
		void DAQ(I3FramePtr frame);
	private:

		I3RecoPulseSeriesPtr GetPulses(
        const I3WaveformSeries::const_iterator firstWF,
        const I3WaveformSeries::const_iterator lastWF,
        const WaveformTemplate& wfTemplate,
        const I3DOMCalibration& calibration,
        const double spe_charge);

		std::string waveforms_name_;
		std::string waveform_range_name_;
		std::string output_name_;

		double spes_per_bin_;
		double tolerance_;
		double noise_threshold_;
		double basis_threshold_;

		int atwd_templ_bins_;
		double atwd_templ_bin_spacing_;

		int fadc_templ_bins_;
		double fadc_templ_bin_spacing_;

		bool deweight_fadc_;
		bool apply_spe_corr_;

		// Template for old-toroid and new-toroid DOMs, respectively
		WaveformTemplate oldTemplate_;
		WaveformTemplate newTemplate_;

		void FillTemplate(WaveformTemplate& wfTemplate,
		    const I3DOMCalibration& calibration);

    cholmod_common c;
};

I3_MODULE(I3Wavedeform);

// End time (ns) of pulse template
inline double PulseWidth(int source) {
  return (source & I3RecoPulse::FADC) ? 170 : 50;
}

// Beginning time (ns) of pulse template
inline double PulseMin(int source) {
  return -2;
}

I3Wavedeform::I3Wavedeform(const I3Context &context) :
    I3ConditionalModule(context)
{
	AddParameter("SPEsPerBin",
	    "Number of basis functions to unfold per waveform bin", 4.);
	AddParameter("Tolerance",
	    "Stopping tolerance, in units of bin mV^2/PE", 9.);
	AddParameter("NoiseThreshold",
	    "Consider bins with amplitude below this number of counts as noise", 2.);
	AddParameter("BasisThreshold",
	    "Require a bin with amplitude at least this number of counts "
	    "within the FWHM of the template waveform in order to include "
	    "a given start time in the basis set", 3.);
	AddParameter("Waveforms", "Name of input waveforms",
	    "CalibratedWaveforms");
	AddParameter("WaveformTimeRange", "Name of maximum time range of "
	    "calibrated waveforms for this event", "CalibratedWaveformRange");
	AddParameter("Output", "Name of output pulse series",
	    "WavedeformPulses");
	AddParameter("DeweightFADC", "Whether to reduce the influence of early FADC"
	    " bins. This option is intended to expert use only; most users"
	    " should leave it set to the default value.", true);
	AddParameter("ApplySPECorrections", "Whether to apply DOM-by-DOM"
	    " corrections to the pulse charge scaling if available", false);

	cholmod_l_start(&c);
}

void
I3Wavedeform::Configure()
{
	GetParameter("Waveforms", waveforms_name_);
	GetParameter("WaveformTimeRange", waveform_range_name_);
	GetParameter("Output", output_name_);
	GetParameter("SPEsPerBin", spes_per_bin_);
	GetParameter("Tolerance", tolerance_);
	GetParameter("NoiseThreshold", noise_threshold_);
	GetParameter("BasisThreshold", basis_threshold_);
	GetParameter("DeweightFADC", deweight_fadc_);
	GetParameter("ApplySPECorrections", apply_spe_corr_);

	double range;

	/* Determine the number of bins and the bin resolution when creating
	 * the ATWD template waveform cache.  Since the ATWD bin size is ~3.3 ns,
	 * we will need to sample our cached template at a resolution of
	 * ~3.3 / spes_per_bin_.  Build the cached waveform templates with a
	 * factor 10 better resolution to minimize binning artifacts when
	 * sampling it.
	 */
	atwd_templ_bin_spacing_ = 3.3 / spes_per_bin_ / 10;
	range = PulseWidth(I3RecoPulse::ATWD) - PulseMin(I3RecoPulse::ATWD);

	atwd_templ_bins_ = (int)ceil(range / atwd_templ_bin_spacing_);

	// Same as the above, but for the FADC, which has a bin width of ~25 ns.
	fadc_templ_bin_spacing_ = 25. / spes_per_bin_ / 10;
	range = PulseWidth(I3RecoPulse::FADC) - PulseMin(I3RecoPulse::FADC);

	fadc_templ_bins_ = (int)ceil(range / fadc_templ_bin_spacing_);

	oldTemplate_.filled = false;
	for (unsigned i = 0; i < 3; ++i) {
		oldTemplate_.atwd_template[i].resize(atwd_templ_bins_);
	}
	oldTemplate_.fadc_template.resize(fadc_templ_bins_);

	newTemplate_.filled = false;
	for (unsigned i = 0; i < 3; ++i) {
		newTemplate_.atwd_template[i].resize(atwd_templ_bins_);
	}
	newTemplate_.fadc_template.resize(fadc_templ_bins_);
}

I3Wavedeform::~I3Wavedeform()
{
	cholmod_l_finish(&c);
}

void
I3Wavedeform::Calibration(I3FramePtr frame) {

	/* Void the waveform templates since they could possibly
	 * change frame-by-frame.
	 */
	oldTemplate_.filled = false;
	newTemplate_.filled = false;
	PushFrame(frame, "OutBox");
}

void
I3Wavedeform::DAQ(I3FramePtr frame)
{
	if (!frame->Has(waveforms_name_)) {
		PushFrame(frame);
		return;
	}

	const I3Calibration& calibration = frame->Get<I3Calibration>();
	const I3DetectorStatus& status = frame->Get<I3DetectorStatus>();
	const I3WaveformSeriesMap& waveforms =
	    frame->Get<I3WaveformSeriesMap>(waveforms_name_);
	boost::shared_ptr<I3RecoPulseSeriesMap> output(new I3RecoPulseSeriesMap);

	// Unfold each set of waveforms into a pulse series
	for (I3WaveformSeriesMap::const_iterator wfs = waveforms.begin();
	     wfs != waveforms.end(); wfs++) {
		std::map<OMKey, I3DOMCalibration>::const_iterator calib =
		    calibration.domCal.find(wfs->first);
		std::map<OMKey, I3DOMStatus>::const_iterator stat =
		    status.domStatus.find(wfs->first);

		// Get/create the appropriate template
		bool newToroid = (calib->second.GetToroidType()==I3DOMCalibration::NEW_TOROID);
		WaveformTemplate& wfTemplate = newToroid ? newTemplate_ : oldTemplate_;
		if (!wfTemplate.filled) {
			FillTemplate(wfTemplate, calib->second);
		}

		(*output)[wfs->first] = *GetPulses(wfs->second.begin(),
		    wfs->second.end(), wfTemplate, calib->second,
		    SPEMean(stat->second, calib->second) *
		        calib->second.GetFrontEndImpedance());

	}

	frame->Put(output_name_, output);

	// Add a time window for the earliest possible pulse in the event
	I3TimeWindowConstPtr waveform_range =
	    frame->Get<I3TimeWindowConstPtr>(waveform_range_name_);
	if (!waveform_range)
		log_fatal("Waveform time range \"%s\" not found in frame",
		    waveform_range_name_.c_str());

	// XXX: Assume FADC has the widest binning
	I3TimeWindowPtr pulse_range(new I3TimeWindow(
	    waveform_range->GetStart() - 25*I3Units::ns,
	    waveform_range->GetStop()
	));
	frame->Put(output_name_ + "TimeRange", pulse_range);

	PushFrame(frame, "OutBox");
}

/*
 *  GetPulses: Solve for the best fit pulse series to a set
 *  of DOM waveforms using NNLS.  We do the following:
 *  1.  Load waveform data (FADC + ATWD, if available) into a linear vector
 *  2.  Eliminate data bins below the set noise threshold
 *  3.  Determine the least-squares weights for each data bin.  Weights
 *          are reduced or set to zero for data we trust less or known
 *          bad data.
 *  4.  Create the set of possible pulses given the time range of nonzero data.
 *          These pulses are defined entirely by their start times.
 *  5.  Remove data bins with zero weight and those outside the support
 *          of all possible pulses.
 *  6.  Build the response matrix A, such Ax = y, where x is the set of
 *          amplitudes corresponding to each pulse and y is the data.
 *  7.  Solve the above for x using NNLS, yielding the pulse amplitudes.
 */
I3RecoPulseSeriesPtr I3Wavedeform::GetPulses(
    const I3WaveformSeries::const_iterator firstWF,
    const I3WaveformSeries::const_iterator lastWF,
    const WaveformTemplate& wfTemplate,
    const I3DOMCalibration& calibration,
    const double spe_charge)
{
	boost::shared_ptr<I3RecoPulseSeries> output(new I3RecoPulseSeries);
	I3WaveformSeries::const_iterator wf;
	cholmod_triplet *basis_trip;
	cholmod_sparse *basis;
	cholmod_dense *data, *unfolded;
	unsigned j, k;
	int nbins = 0;

	for (wf = firstWF; wf != lastWF; wf++)
		nbins += wf->GetWaveform().size();

	// If we have no data, nothing to do
	if (nbins == 0 || !std::isfinite(spe_charge) || spe_charge == 0)
		return output;

	std::vector<int> sources(nbins); // Bitmask of I3RecoPulse::PulseFlags
	std::vector<unsigned> channels(nbins,0.0);
	std::vector<double> redges(nbins+1);
	std::vector<double> weights(nbins);
	std::vector<bool> passBasisThresh(nbins,false);
	// Channel 0 unless otherwise noted
	data = cholmod_l_zeros(nbins, 1, CHOLMOD_REAL, &c);

	// Find and mark low-gain ATWD channels
	for (j = 0, wf = firstWF; wf != lastWF; wf++) {
		for (k = 0; k < wf->GetWaveformInformation().size(); k++) {
			const I3Waveform::StatusCompound &status =
			    wf->GetWaveformInformation()[k];
			unsigned channel = status.GetChannel();
			if (channel > 0 && channel < 3) {
				for (uint64_t a = status.GetInterval().first;
				    a < status.GetInterval().second; a++)
					channels[j+a] = channel;
			}
		}
		j += wf->GetWaveform().size();
	}

	// Fill data vector
	for (wf = firstWF, j = 0; wf != lastWF; wf++) {
		// Set pulse flags to use for this bin
		if (wf->GetDigitizer() == I3Waveform::ATWD)
			sources[j] = I3RecoPulse::ATWD;
		else if (wf->GetDigitizer() == I3Waveform::FADC)
			sources[j] = I3RecoPulse::FADC;
		else {
			log_error("Unknown waveform source (%d), assuming FADC "
			    "pulse template", wf->GetSource());
			sources[j] = I3RecoPulse::FADC;
		}
		if (wf->IsHLC())
			sources[j] |= I3RecoPulse::LC;

		/* Slightly increase the weight of the FADC data where we have
		 * overlap with the ATWD.  Although the history of this is unclear,
		 * it almost certainly is well-motivated and used by the calibration
		 * group in addition to the DeweightFADC flag.
		 */
		double base_weight;
		if (sources[j] & I3RecoPulse::FADC)
			base_weight = 3;
		else
			base_weight = 1;

		// If this waveform is shorter than a pulse width,
		// increase the per-bin weights so the aggregate weight
		// is closer to what it should be
		if (wf->GetWaveform().size() * wf->GetBinWidth() <
		    PulseWidth(sources[j]))
			base_weight *= PulseWidth(sources[j]) /
			    (wf->GetWaveform().size() * wf->GetBinWidth());

		// Calculate channel noise amplitude
		double gainFac = 0;
		if (sources[j] & I3RecoPulse::ATWD)
			gainFac = calibration.GetATWDBinCalibSlope(0, channels[j], 0) /
			    calibration.GetATWDGain(channels[j]);
		if (sources[j] & I3RecoPulse::FADC)
			gainFac = calibration.GetFADCGain();
		if (!(sources[j] & I3RecoPulse::LC))
			gainFac = 0;
		gainFac /= I3Units::mV;
		double noise = noise_threshold_*gainFac;
		double basisThreshmV = basis_threshold_*gainFac;

		// Read waveform
		for (k = 0; k < wf->GetWaveform().size(); k++) {
			redges[j+k] = wf->GetStartTime() + (1. + k) * wf->GetBinWidth();
			sources[j+k] = sources[j];
			((double *)(data->x))[j+k] = wf->GetWaveform()[k] / I3Units::mV;

			weights[j+k] = base_weight;
			if (sources[j+k] & I3RecoPulse::ATWD)
				weights[j+k] /= (1.0 + channels[j+k]);

			/* Deweight first few FADC bins when we expect the ATWD
			 * to also be on. This is motivated by a study from 2014
			 * showing the ATWD template is generally much more accurate,
			 * and additionally using the less-accurate FADC template often
			 * results in split pulses
			 */
			if (deweight_fadc_){
				if (k < 16 && ((sources[j+k] &
				    (I3RecoPulse::FADC | I3RecoPulse::LC)) ==
				    (I3RecoPulse::FADC | I3RecoPulse::LC)))
					weights[j+k] /= 20.;
			}

			// Remove waveform bins that were crazy for some reason
			if (!std::isfinite(((double *)(data->x))[j+k])) {
				((double *)(data->x))[j+k] = 0;
				weights[j+k] = 0;
			}

			// Deweight and zero below noise-floor bins
			if (fabs(((double *)(data->x))[j+k]) < noise) {
				((double *)(data->x))[j+k] = 0;
				weights[j+k] /= 4.;
			} else if (fabs(((double *)(data->x))[j+k]) > basisThreshmV) {
				passBasisThresh[j+k] = true;
			}

			// Ignore high ATWD channels too close to baseline
			if ((sources[j+k] & I3RecoPulse::ATWD) &&
			    channels[j+k] > 0 &&
			    fabs(((double *)(data->x))[j+k]) < 10*noise) {
				((double *)(data->x))[j+k] = 0;
				weights[j+k] = 0;
			}
		}
		j += k;
	}

	// Remove saturated bins
	for (j = 0, wf = firstWF; wf != lastWF; wf++) {
		for (k = 0; k < wf->GetWaveformInformation().size(); k++) {
			const I3Waveform::StatusCompound &status =
			    wf->GetWaveformInformation()[k];
			if (status.GetStatus() & I3Waveform::SATURATED) {
				for (uint64_t a = status.GetInterval().first;
				    a < status.GetInterval().second; a++)
					weights[j+a] = 0;
			}
		}
		j += wf->GetWaveform().size();
	}

	// Apply weights
	for (int i = 0; i < nbins; i++) {
		((double *)(data->x))[i] *= weights[i];
	}

	// Precalculate the max number of basis functions to avoid reallocation
	int maxspes = 0;
	for (wf = firstWF; wf != lastWF; wf++) {
		maxspes += int(spes_per_bin_*(wf->GetWaveform().size()));
	}

	std::vector<double> start_times;
	start_times.reserve(maxspes);
	double min_spe_spacing = DBL_MAX;
	for (j = 0, wf = firstWF; wf != lastWF; wf++) {

		// Start, end two bins early
		double present = wf->GetStartTime() - 2.*wf->GetBinWidth();
		double max = present + wf->GetWaveform().size()*wf->GetBinWidth();
		double spacing = wf->GetBinWidth() / spes_per_bin_;
		if (spacing < min_spe_spacing) {
			min_spe_spacing = spacing;
		}

		// Get the peak FWHM start, stop times for this waveform
		double fwhmStart = wfTemplate.fadcFWHMStart;
		double fwhmStop = wfTemplate.fadcFWHMStop;
		if (sources[j] & I3RecoPulse::ATWD) {
			fwhmStart = wfTemplate.atwdFWHMStart[channels[j]];
			fwhmStop = wfTemplate.atwdFWHMStop[channels[j]];
		}

		// Generate the set of pulse start times that have reasonable
		// non-zero data within the FWHM of the corresponding pulse
		for (k = 0; k < wf->GetWaveform().size(); ++k) {
			if (((double *)(data->x))[j+k] != 0. && passBasisThresh[j+k]) {

				// Move the present time forward if necessary
				double binTime = redges[j+k];
				// Don't jump if we're moving less than the basis spacing
				if (present < (binTime - fwhmStop - spacing)) {
					present = binTime - fwhmStop;
				}

				// Add start times to the set
				while (present < (binTime - fwhmStart) && present < max) {
					start_times.push_back(present);
					present += spacing;
				}
			}
		}

		j += wf->GetWaveform().size();
	}

	int nspes = start_times.size();
	if (nspes == 0) {
		// We have no nonzero data left
		return output;
	}
	std::sort(start_times.begin(), start_times.end());

	// Deduplicate SPE start times to improve basis matrix conditioning.
	// Due to the superposition of multiple basis function rows, multiple
	// basis functions can become interleaved or even duplicate each other.
	// This can slow down the solution and lead to pulse splitting, and
	// is at best redundant.
	{
		int i, j;
		for (i = 0, j = 0; i < nspes-1; i++) {
			// NB: i is always >= j, so the below is always safe
			// min_spe_spacing is inflated slightly here to avoid
			// round-off error
			if (start_times[i+1] - start_times[j] >=
			    0.9 * min_spe_spacing)
				start_times[++j] = start_times[i+1];
		}
		nspes = j+1;
	}
	start_times.resize(nspes);

	// Don't use data bins that are not in the support of any basis function
	for (j = 0, wf = firstWF; wf != lastWF; wf++) {
		double start = DBL_MIN;
		double end = DBL_MIN;

		if (wf->GetWaveform().size() == 0) {
			continue;
		}

		k = 0;
		for (std::vector<double>::const_iterator it = start_times.begin();
		    it != start_times.end(); ++it) {
			start = *it + PulseMin(sources[j+k]);
			end = *it + PulseWidth(sources[j+k]);

			// Evaluate bins up until we pass the end of the current time range
			for (; k < wf->GetWaveform().size() && redges[j+k] < end; ++k) {
				// Check if bin time is inside the max of
				// the last range and the min of the current range
				if (redges[j+k] < start) {
					weights[j+k] = 0.;
				}
			}

			if (k == wf->GetWaveform().size()) {
				break;
			}
		}

		while (k < wf->GetWaveform().size()) {
			// Get rid of bins with times later than the end of the support of the
			// last basis function
			weights[j+k] = 0.;
			++k;
		}
		j += wf->GetWaveform().size();
	}

	// Chop out the data bins that we don't use since we don't fit them
	j = 0;
	for (int i = 0; i < nbins; ++i) {
		if (weights[i] > 0.) {
			weights[j] = weights[i];
			redges[j] = redges[i];
			sources[j] = sources[i];
			channels[j] = channels[i];
			((double *)(data->x))[j] = ((double *)(data->x))[i];
			++j;
		}
	}
	nbins = j;
	data->nrow = nbins;

	// Compute a reasonable upper bound on the number of non-zero matrix
	// elements.
	long nzmax = 0;
	int first_spe = 0;
	double last_t = 0;
	for (int i = 0; i < nbins; i++) {
		// See below for comments about how this loop works
		if (redges[i] < last_t)
			first_spe = 0;
		last_t = redges[i];
		while (first_spe < nspes-1 && redges[i] -
		    start_times[first_spe] > PulseWidth(sources[i]))
			first_spe++;
		double templ_bin_spacing;
		if (sources[i] & I3RecoPulse::ATWD)
			templ_bin_spacing = atwd_templ_bin_spacing_;
		else
			templ_bin_spacing = fadc_templ_bin_spacing_;
		for (int j = first_spe; j < nspes; j++) {
			if (((redges[i] - start_times[j]) -
			    PulseMin(sources[i])) < -templ_bin_spacing)
				break;
			nzmax++;
		}
	}

	// Create model matrix
	basis_trip = cholmod_l_allocate_triplet(nbins, nspes, nzmax, 0,
	    CHOLMOD_REAL, &c);
	basis_trip->nnz = 0;

	first_spe = 0;
	last_t = 0;
	std::vector<int> col_counts(nspes,0);
	std::vector<int> pflags(nspes,0);

	for (int i = 0; i < nbins; i++) {
		// SPEs are sorted in time, as are the bins (though in blocks)
		// Exploit this to minimize the amount of the matrix through
		// which we have to loop.
		if (redges[i] < last_t)
			first_spe = 0;
		last_t = redges[i];

		// The earliest pulse influencing this bin is PULSE_WIDTH
		// in the past
		while (first_spe < nspes && redges[i] -
		    start_times[first_spe] > PulseWidth(sources[i]))
			first_spe++;
		if (first_spe == nspes) {
			continue;
		}

		// Precompute the multiplicative term in the basis
		double weighted_charge = (spe_charge/I3Units::mV) * weights[i];
		if (weighted_charge == 0) // Removed from fit, so ignore
			continue;

		// Precache which pulse template we're using
		double templ_bin_spacing_inv = 1./fadc_templ_bin_spacing_;
		const std::vector<double>* pulse_templ = &(wfTemplate.fadc_template);
		if (sources[i] & I3RecoPulse::ATWD) {
			templ_bin_spacing_inv = 1./atwd_templ_bin_spacing_;
			pulse_templ = &(wfTemplate.atwd_template[channels[i]]);
		}

		// The last pulse for this bin is 2 ns in the future
		for (int j = first_spe; j < nspes; j++) {
			int templ_bin = int(((redges[i] - start_times[j]) -
			    PulseMin(sources[i]))*templ_bin_spacing_inv);
			if (templ_bin < 0)
				break;

			((long *)(basis_trip->i))[basis_trip->nnz] = i;
			((long *)(basis_trip->j))[basis_trip->nnz] = j;
			((double *)(basis_trip->x))[basis_trip->nnz] =
			    (*pulse_templ)[templ_bin] * weighted_charge;
			pflags[j] |= sources[i];

			basis_trip->nnz++;
			assert(basis_trip->nnz <= basis_trip->nzmax);
			col_counts[j]++;
		}
	}

	//  Convert to column-ordered sparse matrix
	//  Note: This is handrolled instead of using
	//  cholmod_l_triplet_to_sparse() in order to exploit some of the
	//  structure of our specific triplet matrix, which lets this
	//  run in less than one third the time of cholmod_l_triplet_to_sparse.
	basis = cholmod_l_allocate_sparse(basis_trip->nrow, basis_trip->ncol,
	    basis_trip->nnz, true, true, 0, CHOLMOD_REAL, &c);
	int accum = 0;
	for (int i = 0; i < nspes; i++) {
		((long *)(basis->p))[i] = accum;
		accum += col_counts[i];
	}
	// Need to set data end pointer for the last column.  Otherwise
	// SuiteSparse will ignore the last column.
	((long *)(basis->p))[nspes] = accum;
	std::vector<long> col_indices(nspes,0);

	for (unsigned i = 0; i < basis_trip->nnz; i++) {
		long col = ((long *)(basis_trip->j))[i];
		long index = ((long *)(basis->p))[col] + col_indices[col]++;
		((long *)(basis->i))[index] = ((long *)(basis_trip->i))[i];
		((double *)(basis->x))[index] = ((double *)(basis_trip->x))[i];
	}
	cholmod_l_free_triplet(&basis_trip, &c);

	// Solve for SPE heights
	unfolded = nnls_lawson_hanson(basis, data, tolerance_,
	    0, 1000, nspes, 0, 1, 0, &c);

	cholmod_l_free_sparse(&basis, &c);
	cholmod_l_free_dense(&data, &c);

	// Load the SPE corrections
	double atwdSPECorrection = 1.;
	double fadcSPECorrection = 1.;
	if (apply_spe_corr_) {
		if (calibration.IsMeanATWDChargeValid()) {
			atwdSPECorrection = 1. / calibration.GetMeanATWDCharge();
		}
		if (calibration.IsMeanFADCChargeValid()) {
			fadcSPECorrection = 1. / calibration.GetMeanFADCCharge();
		}
	}

	// Convert to pulse series
	for (int i = 0; i < nspes; i++) {
		if (((double *)(unfolded->x))[i] == 0)
			continue;

		I3RecoPulse pulse;

		pulse.SetTime(start_times[i]);
		double speCorrection = pflags[i] & I3RecoPulse::ATWD ?
		    atwdSPECorrection : fadcSPECorrection;
		pulse.SetCharge((((double *)(unfolded->x))[i]) * speCorrection);

		double leftdiff = HUGE_VAL, rightdiff = HUGE_VAL;
		if (i+1 < nspes)
			rightdiff = start_times[i+1] - start_times[i];
		if (i > 0)
			leftdiff = start_times[i] - start_times[i-1];
		pulse.SetWidth((leftdiff < rightdiff) ? leftdiff : rightdiff);
		pulse.SetFlags(pflags[i]);
		output->push_back(pulse);
	}
	cholmod_l_free_dense(&unfolded, &c);
	return output;
}

/* Simple routine to calculate FWHM start and stop given a waveform template */
void FillFWHM(double& start, double& stop,
    const std::vector<double>& data, double spacing, double min) {

	// Find the maximum
	double max = DBL_MIN;
	for (unsigned i = 0; i < data.size(); ++i) {
		if (data[i] > max) {
			max = data[i];
		}
	}

	double hm = max*0.5;
	unsigned hmbin = 0;
	for (unsigned i = 0; i < data.size(); ++i) {
		if (data[i] > hm) {
			hmbin = i;
			break;
		}
	}

	// Be conservative: choose the bin before the first rise above hm
	if (hmbin > 0) {
		--hmbin;
	}

	start = min + hmbin*spacing;

	for (int i = data.size() - 1; i >= 0; --i) {
		if (data[i] > hm) {
			hmbin = i;
			break;
		}
	}

	if (hmbin < data.size() - 1) {
		++hmbin;
	}

	stop = min + hmbin*spacing;
	if (stop == start) {
		stop += spacing;
	}
}

void I3Wavedeform::FillTemplate(WaveformTemplate& wfTemplate,
    const I3DOMCalibration& calibration) {
	for (int channel = 0; channel < 3; channel++) {
		I3DOMCalibration::DroopedSPETemplate chan_template =
		    calibration.ATWDPulseTemplate(channel);
		wfTemplate.atwd_template[channel].resize(atwd_templ_bins_);
		for (int i = 0; i < atwd_templ_bins_; i++) {
			wfTemplate.atwd_template[channel][i] =
			    chan_template(PulseMin(I3RecoPulse::ATWD) +
			    i*atwd_templ_bin_spacing_);
		}

		FillFWHM(wfTemplate.atwdFWHMStart[channel],
		    wfTemplate.atwdFWHMStop[channel],
		    wfTemplate.atwd_template[channel],
		    atwd_templ_bin_spacing_, PulseMin(I3RecoPulse::ATWD));
	}

	I3DOMCalibration::DroopedSPETemplate fadc_dcal_template =
	    calibration.FADCPulseTemplate();
	wfTemplate.fadc_template.resize(fadc_templ_bins_);
	for (int i = 0; i < fadc_templ_bins_; i++) {
		wfTemplate.fadc_template[i] = fadc_dcal_template(
		    PulseMin(I3RecoPulse::FADC) +
		    i*fadc_templ_bin_spacing_);
	}

	FillFWHM(wfTemplate.fadcFWHMStart, wfTemplate.fadcFWHMStop,
	    wfTemplate.fadc_template, fadc_templ_bin_spacing_,
	    PulseMin(I3RecoPulse::FADC));

	wfTemplate.filled = true;
}
