#ifndef ROSENCRANTZ_ROSENCRANTZ_H_INCLUDED
#define ROSENCRANTZ_ROSENCRANTZ_H_INCLUDED

#include "icetray/I3ConditionalModule.h"
#include "icetray/OMKey.h"
#include "phys-services/I3RandomService.h"
#include "sim-services/I3SumGenerator.h"
#include <simclasses/I3MCPE.h>
#include <simclasses/I3MCPulse.h>
#include <dataclasses/physics/I3ParticleID.h>
#include <simclasses/I3ParticleIDMap.hpp>

class I3Calibration;
class I3DOMCalibration;
class I3DOMStatus;
/// \class: PMTResponseSimulator (Rosencrantz)
///
/// \author Chris Weaver <chris.weaver@icecube.wisc.edu>
///
/// \brief A module which simulates the behaviour of a PMT
///
///This module converts 'raw' MC PEs (representing integer numbers of
///photoelectrons ejected from the photocathode of the PMT) into MC Pulses
///whose weights are proportional to the amount of current produced
///by the PMT at the anode and with times randomly shifted according to
///appropriate probability distributions. The hits produced by this module
///are sorted in time order.
///
///The weights of the processed hits are still in units of photoelectrons,
///and so must be multiplied by the PMT gain when convolved with digitizer
///pulse templates to produce electronics readouts. Similarly, the times of
///the hits do not include the 'transit time' of the DOM (which includes
///both the PMT transit time and delays in the mainboard) so this time must
///be added when computing digitizer outputs.
///
///Note: This module is intended for use with the I3PhotonicsHitMaker and
///      DOMLauncher modules; it is not compatible with I3HitMaker and
///      I3DOMsimulator.

class PMTResponseSimulator : public I3ConditionalModule{
public:
	PMTResponseSimulator(const I3Context& context);
	~PMTResponseSimulator();
	void Configure();
	void DAQ(I3FramePtr frame);
private:
	///The name of the I3MCPESeriesMap to process.
	std::string inputHitsName_;
	///The name of the I3MCPESeriesMap to be produced.
	std::string outputHitsName_;

	///Whether the charges of hits should be randomized
	bool useSPEDistribution_;
	///Whether the times of hits should be randomly perturbed
	bool usePMTJitter_;
	///The probability that a pulse arrives early
	double prePulseProbability_;
	///The probability that a pulse arrives late
	double latePulseProbability_;
	///The probability that a pulse produces an accompanying afterpulse
	double afterPulseProbability_;
	///Whether the weights of the hits should be modified to account for saturation
	bool applySaturation_;
	///Whether hits very close in time should be merged
	bool mergeHits_;
	///Whether to merge hits repeatedly during generation to attempt to limit transient memory use
	bool lowMem_;
	///The ID of the random service to use
	std::string randomServiceName_;

	///The random service fetched from the tray
	boost::shared_ptr<I3RandomService> randomService_;
	///A generic distribution of charges produced by single photon hits
	boost::shared_ptr<I3SumGenerator> genericChargeDistribution_;
	///The charge distributions to be used for each DOM for single photon hits
	std::map<OMKey, boost::shared_ptr<I3SumGenerator> > chargeDistributions_;

	///The most recently seen calibration data
	boost::shared_ptr<const I3Calibration> lastCalibration_;

	///Computes the amount of charge, relative to the charge produced by an ideal, single
	///  photoelectron, produced by a hit whose weight is an integer number of initial
	///  photoelectrons
	///
	///\param w The weight of the hit in photons
	///\param om The OM for which the charge is to be computed
	///\return  The amount of charge produced by the PMT for this hit, in units of the ideal charge
	///         produced by a single photoelectron
	double normalHitWeight(unsigned int w, OMKey om);

	///Generates a random time jitter value for a hit
	///\return A random time offset, in nanoseconds
	double PMTJitter();

	///Computes the amount by which prepulses are early at a given voltage
	///\param voltage The operating voltage of the DOM
	///\return A time offset, in nanoseconds
	double prePulseTimeShift(double voltage);

	///Computes the charge of a prepulse, relative to an ideal s.p.e. at a given voltage
	///\param voltage The operating voltage of the DOM
	///\return The amount of charge produced by the PMT for a prepulse, in units of the ideal
	///        charge produced by a single photoelectron
	double prePulseWeight(double voltage);

	///Computes the charge of an early afterpulse, relative to an ideal s.p.e. at a given voltage
	///\return The amount of charge produced by the PMT for an early afterpulse, in units of the ideal
	///        charge produced by a single photoelectron
	double earlyAfterPulseWeight();

	///Alters the properies of the given hit to describe a late pulse
	///\param hit The existing raw hit to be turned into a late pulse hit
	///\param om The DOM in which the hit is being detected
	///\param voltage The operating voltage of the PMT
	void createLatePulse(I3MCPulse& hit, OMKey om, double voltage);

	///Alters the properies of the given hit to describe an afterpulse
	///\param hit The existing raw hit to be turned into an afterpulse hit
	///\param om The DOM in which the hit is being detected
	///\param voltage The operating voltage of the PMT
	void createAfterPulse(I3MCPulse& hit, OMKey om, double voltage);

public:
	///Applies all transformations to a set of hits on a single DOM
	///\param inputHits The raw hits on the DOM
	///\param dom The DOM on which the hits occur
	///\param cal The current calibration information for this DOM
	///\param status The current status information for this DOM
	///\return A new set of time ordered hits with weighting applied, alternate
	///pulse types generated, saturation applied, and time merging applied
	std::pair<std::vector<I3MCPulse>, ParticlePulseIndexMap>
	processHits(const std::vector<I3MCPE>& inputHits, OMKey dom,
	            const I3DOMCalibration& cal, const I3DOMStatus& status);

	///Reweights the hits in the given series to mimic the effects of saturation in the PMT.
	///  The 'inverse' saturation parameterization from T. Waldenmeier is used.
	///\param hits The hit series to be reweighted
	///\param pmtVoltage The operating voltage of the PMT
	///\param cal The calibration data for the DOM
	///\pre The input hits must be time ordered
	void saturate(std::vector<I3MCPulse>& hits, double pmtVoltage, const I3DOMCalibration& cal);

	///Merges together pulses which are within some small time window, compacting the hit series
	///  if it is densely filled with hits.
	///  (Here, 'small' is intended to be relative to the resolution of the feature extractor.)
	///\param hits The hit series to be compacted
	///\pre The input hits must be time ordered
	static void timeMergeHits(std::vector<I3MCPE>& hits);
	static void timeMergeHits(std::vector<I3MCPulse>& hits, ParticlePulseIndexMap& aux);

	//Simple accessors and mutators for using this object directly, rather than as part of a tray

	std::string getInputHitsName() const{ return(inputHitsName_); }
	void setInputHitsName(const std::string& in){ inputHitsName_=in; }

	std::string getOutputHitsName() const{ return(outputHitsName_); }
	void setOutputHitsName(const std::string& out){ outputHitsName_=out; }

	bool getUseSPEDistribution() const{ return(useSPEDistribution_); }
	void setUseSPEDistribution(bool use){ useSPEDistribution_=use; }

	bool getUseJitter() const{ return(usePMTJitter_); }
	void setUseJitter(bool use){ usePMTJitter_=use; }

	double getPrePulseProbability() const{ return(prePulseProbability_); }
	void setPrePulseProbability(double prob){ prePulseProbability_=prob; }

	double getLatePulseProbability() const{ return(latePulseProbability_); }
	void setLatePulseProbability(double prob){ latePulseProbability_=prob; }

	double getAfterPulseProbability() const{ return(afterPulseProbability_); }
	void setAfterPulseProbability(double prob){ afterPulseProbability_=prob; }

	bool getApplySaturation() const{ return(applySaturation_); }
	void setApplySaturation(bool apply){ applySaturation_=apply; }

	bool getMergeHits() const{ return(mergeHits_); }
	void setMergeHits(bool merge){ mergeHits_=merge; }

	bool getLowMem() const{ return(lowMem_); }
	void setLowMem(bool lowMem){ lowMem_=lowMem; }

	boost::shared_ptr<I3RandomService> getRandomService() const{ return(randomService_); }
	void setRandomService(boost::shared_ptr<I3RandomService> randomService){ randomService_=randomService; }

private:
	///Generate a Fisher-Tippet distributed (alias Gumbel distributed)  random variate
	///\param location The location parameter of the distribution from which to sample
	///                 (mean = location + scale * euler_mascheroni)
	///\param scale The scale parameter of the distribution from which to sample
	///              (variance = (pi * scale)**2 / 6)
	///\param logLowerBound Indirectly determines the lower cutoff of the distribution
	///\param logUpperBound Indirectly determines the upper cutoff of the distribution
	double fisherTippett(double location, double scale, double logLowerBound, double logUpperBound);

	///A helper object for storing and evaluating pulse charge distributions with the form of the
	///sum of an exponential and a gaussian component.
	struct pmtChargeDistribution{
	private:
		///The width parameter for the exponential component, with units of the ideal s.p.e. result charge
		double exp_width;
		///The location of the peak of the gaussian component, with units of the ideal s.p.e. result charge
		double gaus_mean;
		///The width parameter for the gaussian component, with units of the ideal s.p.e. result charge
		double gaus_width;
		///The amplitude of the gaussian component divided by the amplitude of the exponential component
		double amp_ratio;
		///Overall multiplicative factor to ensure that the function has integral 1
		double normalization;

	public:
		pmtChargeDistribution(double exp_width_, double gaus_mean_,
		                      double gaus_width_, double amp_ratio_);
		///Evaulates normalization * (exp(-q/exp_width) + amp_ratio * exp(-.5((q-gaus_mean)/gaus_width)^2))
		double operator()(double q) const;
	};
	SET_LOGGER("PMTResponseSimulator");
	friend class PMTResponseSimulatorTestSetup;
};

#endif
