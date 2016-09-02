#include "DOMLauncher/PMTResponseSimulator.h"

#include <cmath>
#include <cassert>
#include <numeric>
#include <limits>
#include <algorithm>
#include <stack>

#include <boost/make_shared.hpp>
#include <boost/math/constants/constants.hpp>

#include <icetray/I3Units.h>
#include <simclasses/I3MCPE.h>
#include <simclasses/I3MCPulse.h>
#include <simclasses/I3ParticleIDMap.hpp>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/calibration/I3Calibration.h>

#include "discreteDistribution.h"

namespace constants = boost::math::constants;

I3_MODULE(PMTResponseSimulator)

PMTResponseSimulator::PMTResponseSimulator(const I3Context& context):
I3ConditionalModule(context),
inputHitsName_("I3MCPESeriesMap"),
outputHitsName_("I3MCPulseSeriesMap"),
useSPEDistribution_(true),
usePMTJitter_(true),
prePulseProbability_(0.003), //http://icecube.wisc.edu/~chwendt/talks/PMTChargeSpectrumVsTime-Ghent2007.pdf
latePulseProbability_(0.035),
afterPulseProbability_(0.0593), //http://icecube.wisc.edu/~chwendt/afterpulse-notes/
applySaturation_(true),
mergeHits_(true),
lowMem_(false),
randomServiceName_("I3RandomService")
{
	//For use if module description strings become supported in icetray
	/*SetDescription("A module which simulates the behaviour of a PMT\n"
	               "\n"
	               "This module converts 'raw' MC PEs (representing integer numbers of \n"
	               "photoelectrons ejected from the photocathode of the PMT) into 'processed' \n"
	               "MC Hits whose weights are proportional to the amount of current produced \n"
	               "by the PMT at the anode and with times randomly shifted according to \n"
	               "appropriate probability distributions. The hits produced by this module \n"
	               "are sorted in time order. \n"
	               "\n"
	               "The weights of the processed hits are still in units of photoelectrons, \n"
	               "and so must be multiplied by the PMT gain when convolved with digitizer \n"
	               "pulse templates to produce electronics readouts. Similarly, the times of \n"
	               "the hits do not include the 'transit time' of the DOM (which includes \n"
	               "both the PMT transit time and delays in the mainboard) so this time must \n"
	               "be added when computing digitizer outputs. \n"
	               "\n"
	               "Note: This module is intended for use with the I3PhotonicsHitMaker and \n"
	               "      DOMLauncher modules; it is not compatible with I3HitMaker and \n"
	               "      I3DOMsimulator. ");*/

	AddParameter("Input",
	             "The name of the I3MCPESeriesMap to process.",
	             inputHitsName_);
	AddParameter("Output",
	             "The name of the I3MCPESeriesMap to be produced",outputHitsName_);
	AddParameter("UseSPEDistribution",
	             "Whether hit charges should be drawn from the proper charge distribution or left ideal",
	             useSPEDistribution_);
	AddParameter("UsePMTJitter",
	             "Whether the times of pulses should be randomly perturbed",usePMTJitter_);
	AddParameter("PrePulseProbability",
	             "The probability that a pulse arrives early",prePulseProbability_);
	AddParameter("LatePulseProbability",
	             "The probability that a pulse arrives late",latePulseProbability_);
	AddParameter("AfterPulseProbability",
	             "The probability that a pulse produces an accompanying afterpulse",
	             afterPulseProbability_);
	AddParameter("ApplySaturation",
	             "Whether the weights of the hits should be modified to account for saturation",
	             applySaturation_);
	AddParameter("MergeHits", "Whether hits very near in time should be merged in the output",
	             mergeHits_);
	AddParameter("LowMem", "If true, attempt to reduce peak memory use by repeatedly merging hits "
	             "as they are generated. Use of this option requires that MergeHits be true.\n"
	             "WARNING: Use of this option may slightly reduce precision and drastically "
	             "increase running time. It is potentially useful for very bright events, and "
	             "probably harmful for very long events.\n"
	             "Please only use this if you know what you are doing, which probably requires "
	             "benchmarking both runtime and peak memory use.",
	             lowMem_);
	AddParameter("RandomServiceName",
		     "Name of the random service in the context.",
		     randomServiceName_);
	AddOutBox("OutBox");
}

PMTResponseSimulator::~PMTResponseSimulator(){}

void PMTResponseSimulator::Configure(){
	GetParameter("Input",inputHitsName_);
	GetParameter("Output",outputHitsName_);
	GetParameter("UseSPEDistribution",useSPEDistribution_);
	GetParameter("UsePMTJitter",usePMTJitter_);
	GetParameter("PrePulseProbability",prePulseProbability_);
	GetParameter("LatePulseProbability",latePulseProbability_);
	GetParameter("AfterPulseProbability",afterPulseProbability_);
	GetParameter("ApplySaturation",applySaturation_);
	GetParameter("MergeHits",mergeHits_);
	GetParameter("LowMem",lowMem_);
	GetParameter("RandomServiceName",randomServiceName_);
	randomService_ = context_.Get<I3RandomServicePtr>(randomServiceName_);
	if(!randomService_)
		log_fatal("No random service available");
	if(prePulseProbability_<0.0 || prePulseProbability_>1.0)
		log_fatal_stream("Invalid probability of prepulses: " << prePulseProbability_);
	if(latePulseProbability_<0.0 || latePulseProbability_>1.0)
		log_fatal_stream("Invalid probability of late pulses: " << latePulseProbability_);
	if(afterPulseProbability_<0.0 || afterPulseProbability_>1.0)
		log_fatal_stream("Invalid probability of afterpulses: " << afterPulseProbability_);
    if(afterPulseProbability_==1.0)
        log_fatal("An afterpulses probability of 1 will lead to generation of an infinite number of afterpulses");
	if((prePulseProbability_+latePulseProbability_)>1.0)
		log_fatal_stream("Combined probablity of prepulses (" << prePulseProbability_
		                 << ") and late pulses (" << latePulseProbability_ << ") is greater than 1");
	if(lowMem_){
		if(!mergeHits_)
			log_fatal("Specifying 'LowMem' for PMTResponseSimulator will not work without also specifying 'MergeHits'");
		log_warn("Enabling 'low memory' mode, which will attempt to reduce memory use at the cost of longer run time");
	}

	//TODO: store these constants somewhere else
	//Also, use constants derived from data rather than lifted from pmt-simulator
	genericChargeDistribution_ = boost::make_shared<I3SumGenerator>(randomService_,
	    pmtChargeDistribution(0.5057, //exponential width
	                          1., //gaussian mean
	                          0.2916, //gaussian width
	                          1.624367847034316), //amplitude ratio
	    0, //minimum value
	    3, //maximum value
	    1000, //number of bins
	    12 //when to switch to gaussian sampling
	    );
}

bool earlierHit(const I3MCPE& h1, const I3MCPE& h2){ return(h1.time < h2.time); }

void PMTResponseSimulator::DAQ(I3FramePtr frame){
	if(!frame->Has(inputHitsName_)){
		log_info_stream("Input '" << inputHitsName_ << "' not found; ignoring frame");
		PushFrame(frame);
		return;
	}
	boost::shared_ptr<const I3Map<OMKey,std::vector<I3MCPE> > > inputHits
	  = frame->Get<boost::shared_ptr<const I3Map<OMKey,std::vector<I3MCPE> > > >(inputHitsName_);
	if(!inputHits){
		log_error_stream("Input '" << inputHitsName_ << "' does not exist or is not an I3Map<OMKey,std::vector<I3MCPE>>");
		PushFrame(frame);
		return;
	}
	boost::shared_ptr<const I3DetectorStatus> detStatus
	  = frame->Get<boost::shared_ptr<const I3DetectorStatus> >();
	if(!detStatus)
		log_fatal("Could not find detector status data");
	boost::shared_ptr<const I3Calibration> calibration
	  = frame->Get<boost::shared_ptr<const I3Calibration> >();
	if(!calibration)
		log_fatal("Could not find calibration data");
	if(calibration!=lastCalibration_){
		chargeDistributions_.clear();
		lastCalibration_=calibration;
	}

	I3ParticleIDMapPtr outputPIDMap(new I3ParticleIDMap());
	I3MCPulseSeriesMapPtr outputPulses(new I3MCPulseSeriesMap());

	//iterate over DOMs
	for(I3Map<OMKey,std::vector<I3MCPE> >::const_iterator domIt=inputHits->begin(), domEnd=inputHits->end();
	    domIt!=domEnd; domIt++){
		std::map<OMKey,I3DOMStatus>::const_iterator omStatus=detStatus->domStatus.find(domIt->first);
		if(omStatus==detStatus->domStatus.end()){
			log_debug_stream("No detector status record for " << domIt->first);
			continue;
		}

		const double pmtVoltage=omStatus->second.pmtHV;
		if(pmtVoltage==0.0){
			log_debug_stream("Ignoring hits in DOM with voltage set to zero (" << domIt->first << ')');
			continue;
		}

		std::map<OMKey, I3DOMCalibration>::const_iterator omCalibration=calibration->domCal.find(domIt->first);
		if(omCalibration==calibration->domCal.end()){
			log_warn_stream("No calibration record for " << domIt->first);
			continue;
		}

		if(pmtVoltage<0.0 || std::isnan(pmtVoltage)){
			log_warn_stream("Ignoring hits in DOM with nonsensical voltage (" << pmtVoltage << ')');
			continue;
		}
		
		if(domIt->second.empty()){
			log_trace_stream("Ignoring DOM with zero hits but with a PE map entry (" << domIt->first << ')');
			continue;
		}

		std::pair<std::vector<I3MCPulse>, ParticlePulseIndexMap> pulses=
		processHits(domIt->second, domIt->first, omCalibration->second, omStatus->second);
		outputPulses->insert(std::make_pair(domIt->first,pulses.first));
		outputPIDMap->insert(std::make_pair(domIt->first,pulses.second));
	} //end of iteration over DOMs

	frame->Put(outputHitsName_,outputPulses);
	frame->Put(outputHitsName_+"ParticleIDMap",outputPIDMap);

	uint64_t nPulses=0;
	for(I3Map<OMKey, std::vector<I3MCPulse> >::const_iterator domIt=outputPulses->begin(), domEnd=outputPulses->end(); domIt!=domEnd; domIt++)
		nPulses+=domIt->second.size();

	PushFrame(frame);
}

std::pair<std::vector<I3MCPulse>, ParticlePulseIndexMap>
PMTResponseSimulator::processHits(const std::vector<I3MCPE>& inputHits, OMKey dom,
                                  const I3DOMCalibration& calibration, const I3DOMStatus& status){
	//std::cout << dom << " has " << inputHits.size() << " input hits" << std::endl;
	std::pair<std::vector<I3MCPulse>, ParticlePulseIndexMap> result;
	std::vector<I3MCPulse>& outputHits=result.first;
	ParticlePulseIndexMap& particleMap=result.second;
	//std::vector<I3MCPE> outputHits;

	const double pmtVoltage=status.pmtHV;
	//iterate over hits
	uint32_t pulseIndex=0;
	uint32_t roughCounter=0;
	for(std::vector<I3MCPE>::const_iterator hitIt=inputHits.begin(), hitEnd=inputHits.end();
		hitIt!=hitEnd; hitIt++){
		const unsigned int nHits = hitIt->npe;
		log_trace_stream("splitting a hit with weight " << nHits);
		I3ParticleID pid=hitIt->ID;

		//decompose multi-photon hits into separate, single-photon hits
		//TODO: it might be a good idea to turn this off and fall back on
		//average behavior for very high weight hits
		for(unsigned int i=0; i<nHits; i++){
			I3MCPulse pulse;
			pulse.time=hitIt->time;
			//decide randomly which kind of hit to make
			double ran=randomService_->Uniform(1);
			if(ran<(1.-prePulseProbability_-latePulseProbability_)){ //regular pulse
				log_trace("creating an SPE");
				pulse.source=I3MCPulse::PE;
				pulse.charge=normalHitWeight(1,dom);
				pulse.time+=PMTJitter();
			}
			else if(ran<(1.-latePulseProbability_)){ //prepulse
				log_trace("creating a prepulse");
				pulse.source=I3MCPulse::PRE_PULSE;
				pulse.charge=prePulseWeight(pmtVoltage);
				pulse.time+=PMTJitter()+prePulseTimeShift(pmtVoltage);
			}
			else{ //late pulse
				log_trace("creating a late pulse");
				createLatePulse(pulse,dom,pmtVoltage);
			}
			outputHits.push_back(pulse);
			particleMap[pid].push_back(pulseIndex++);

			//maybe also add one or more afterpulses
			//each additional afterpulse should have its time be defined relative
			//to the hit which generates it, rather than raltive to the original hit
			double afterpulseBaseTime=hitIt->time;
			while(randomService_->Uniform(1)<afterPulseProbability_){
				log_trace("adding an afterpulse");
				I3MCPulse afterPulse;
				afterPulse.time=afterpulseBaseTime; //use the parent hit's time
				createAfterPulse(afterPulse,dom,pmtVoltage);
				afterpulseBaseTime=afterPulse.time; //set the base time for a possible subsidiary afterpulse
				outputHits.push_back(afterPulse);
				particleMap[pid].push_back(pulseIndex++);
			}
			roughCounter++;
			if(lowMem_ && roughCounter>=10000){
				sortMCPulses(outputHits.begin(),outputHits.end(), particleMap);
				timeMergeHits(outputHits,particleMap);
				pulseIndex=outputHits.size();
				roughCounter=0;
			}
		} //end of iteration over single photons
	} //end of iteration over hits

	//ensure time ordering
	sortMCPulses(outputHits.begin(),outputHits.end(), particleMap);
	//applying saturation before time-merging is more precise, but slower
	if(applySaturation_)
		saturate(outputHits,pmtVoltage,calibration);
	//std::cout << dom << " has " << outputHits.size() << " output hits" << std::endl;
	if(mergeHits_){
		timeMergeHits(outputHits,particleMap);
		//std::cout << dom << " has " << outputHits.size() << " output hits after merging" << std::endl;
	}
	return(result);
}

double PMTResponseSimulator::normalHitWeight(unsigned int w, OMKey om){
	if(!useSPEDistribution_)
		return(w);
	std::map<OMKey, boost::shared_ptr<I3SumGenerator> >::const_iterator dist=chargeDistributions_.find(om);
	if(dist==chargeDistributions_.end()){ //did not find this OM
		//TODO: use actual calibration data for the DOM in question
		dist=chargeDistributions_.insert(std::make_pair(om,genericChargeDistribution_)).first;
	}
	return(dist->second->Generate(w));
}

//Taken directly from the I3HitMaker code
/**
 * Fisher-Tippett variables for well behaved time
 * distribtutions.
 * These values were obtained by fits to Bricktop running
 * at 1345 [V] during DFL studies by C. Wendt. The fits were
 * performed by R. Porrata.
 */
double PMTResponseSimulator::PMTJitter(){
	if(!usePMTJitter_)
		return(0.0);

	const double mu=0.15304; //ns
	const double beta=1.9184; //ns
	const double ln_time_upper_bound=0.999998;
	const double ln_time_lower_bound=1e-7;

	return(fisherTippett(mu,beta,ln_time_lower_bound,ln_time_upper_bound));
}

//Time shift formula taken from HitMaker
double PMTResponseSimulator::prePulseTimeShift(double voltage){
	const double reference_time=31.8; //ns
	const double reference_voltage=1345.*I3Units::volt; //V
	return(-reference_time*sqrt(reference_voltage/voltage));
}

double PMTResponseSimulator::prePulseWeight(double voltage){
	if(!useSPEDistribution_)
		return(1.0);
	return(1/20.);
}

//Weighting formula taken from pmt-simulator
double PMTResponseSimulator::earlyAfterPulseWeight(){
	if(!useSPEDistribution_)
		return(1.0);
	//Fisher-Tippett fit to Early Afterpulse Charge distribtion, peaks 2 and 3.
	//Data from C. Wendt, fit by R. Porrata
	//https://wiki.icecube.wisc.edu/index.php/Early_Afterpulse_Data
	//The peak of the distribution
	const double peak_charge=13.31;
	//The width of the distribution; the sign is important because the distribution is asymmetric
	const double charge_spread=-3.386;
	//Upper bound of charge distribution corresponding to 26.28 PE
	const double ln_charge_lower_bound=1e-20;
	//Lower bound of charge distribution corresponding to 3.33 PE
	const double ln_charge_upper_bound=0.94883;
	return(fisherTippett(peak_charge,charge_spread,ln_charge_lower_bound,ln_charge_upper_bound));
}

struct pulseComponent{
	I3MCPulse::PulseSource source;
	double amp;
	double scale;
	double location;

	double weight() const{ return(amp*std::abs(scale)); }
};

//This suggests to the compiler that var is 'used', suppressing unhelpful warnings
#define used_var(var) while(0){ var++; }

void PMTResponseSimulator::createLatePulse(I3MCPulse& hit, OMKey om, double voltage){
	//data for different types of late pulses
	const static unsigned int nComponents=5;
	const static pulseComponent pulseTypes[nComponents]={
		//source,                         amp,    scale,                     location
		{I3MCPulse::INELASTIC_LATE_PULSE, 150.0 , -3.0*I3Units::nanosecond ,  37.0*I3Units::nanosecond},
		{I3MCPulse::INELASTIC_LATE_PULSE, 306.0 , -9.0*I3Units::nanosecond ,  52.6*I3Units::nanosecond},
		{I3MCPulse::ELASTIC_LATE_PULSE  , 530.62,  3.47*I3Units::nanosecond,  66.0*I3Units::nanosecond},
		{I3MCPulse::INELASTIC_LATE_PULSE,  25.0 , 20.0*I3Units::nanosecond , 107.0*I3Units::nanosecond},
		{I3MCPulse::ELASTIC_LATE_PULSE  ,  53.0 ,  2.75*I3Units::nanosecond, 141.0*I3Units::nanosecond}
	};
	static double probabilities[nComponents];
	//compute the prbability for each distribution component,
	//and make a store to a static dummy variable to force the computation to be done only once
	static double* dummy = std::transform(pulseTypes,pulseTypes+nComponents,probabilities,std::mem_fun_ref(&pulseComponent::weight));
	used_var(dummy); //suppress compiler warnings about the dummy varaible being unused
	static discrete_distribution dist(probabilities,probabilities+nComponents);

	//decide which type of pulse to generate, randomly
	random_adapter gen(randomService_.get());
	unsigned int index=dist(gen);
	const pulseComponent& pulseType=pulseTypes[index];
	hit.source=pulseType.source;

	//constants for the time delay calculation
	const double referenceVoltage = 1345*I3Units::V;
	const double ln_time_lower_bound=1e-6;
	const double ln_time_upper_bound=0.999998;

	double timeDelay=0.0;
	timeDelay = fisherTippett(pulseType.location,pulseType.scale,ln_time_lower_bound,ln_time_upper_bound);
	timeDelay *= sqrt(referenceVoltage/voltage);
	hit.time+=timeDelay;

	hit.charge=normalHitWeight(1,om);
}

void PMTResponseSimulator::createAfterPulse(I3MCPulse& hit, OMKey om, double voltage){
	const static unsigned int nComponents=11;
	//From hit-maker; should be derived from https://wiki.icecube.wisc.edu/index.php/Afterpulse_Data
	//Note that early after pulse components produce multiple p.e. of charge, so
	//their amplitudes have been divided by the average number of p.e. produced
	//to maintain the total generated charge.
	const static pulseComponent pulseTypes[nComponents]={
		//source,                      amp,           width,                      mean
		{I3MCPulse::AFTER_PULSE      , 10.0         , 200.0*I3Units::nanosecond ,  500.0*I3Units::nanosecond},
		{I3MCPulse::EARLY_AFTER_PULSE,  2.0254382825,  20.0*I3Units::nanosecond ,  540.0*I3Units::nanosecond},
		{I3MCPulse::EARLY_AFTER_PULSE,  3.9628140310,  20.0*I3Units::nanosecond ,  660.0*I3Units::nanosecond},
		{I3MCPulse::AFTER_PULSE      ,  6.5         , 100.0*I3Units::nanosecond , 1100.0*I3Units::nanosecond},
		{I3MCPulse::AFTER_PULSE      ,  4.5         , 200.0*I3Units::nanosecond , 1300.0*I3Units::nanosecond},
		{I3MCPulse::AFTER_PULSE      ,  8.5         , 225.0*I3Units::nanosecond , 1650.0*I3Units::nanosecond},
		{I3MCPulse::AFTER_PULSE      ,  8.95        , 300.0*I3Units::nanosecond , 2075.0*I3Units::nanosecond},
		{I3MCPulse::AFTER_PULSE      ,  5.15        , 500.0*I3Units::nanosecond , 2850.0*I3Units::nanosecond},
		{I3MCPulse::AFTER_PULSE      ,  3.0         , 700.0*I3Units::nanosecond , 4750.0*I3Units::nanosecond},
		{I3MCPulse::AFTER_PULSE      ,  5.0         , 400.0*I3Units::nanosecond , 6150.3*I3Units::nanosecond},
		{I3MCPulse::AFTER_PULSE      , 17.442       , 944.59*I3Units::nanosecond, 7833.4*I3Units::nanosecond}
	};
	static double probabilities[nComponents];
	//compute the probability for each distribution component,
	//and make a store to a static dummy variable to force the computation to be done only once
	static double* dummy = std::transform(pulseTypes,pulseTypes+nComponents,probabilities,std::mem_fun_ref(&pulseComponent::weight));
	used_var(dummy); //suppress compiler warnings about the dummy varaible being unused
	static discrete_distribution dist(probabilities,probabilities+nComponents);

	//decide which type of pulse to generate, randomly
	random_adapter gen(randomService_.get());
	unsigned int index=dist(gen);
	const pulseComponent& pulseType=pulseTypes[index];
	hit.source=pulseType.source;

	const double referenceVoltage = 1345*I3Units::V;
	double timeDelay=0.0;
	while(timeDelay<=0.0)
		timeDelay=randomService_->Gaus(pulseType.location,pulseType.scale);
	timeDelay *= sqrt(referenceVoltage/voltage);
	hit.time+=timeDelay;

	if(pulseType.source==I3MCPulse::EARLY_AFTER_PULSE)
		hit.charge=earlyAfterPulseWeight();
	else
		hit.charge=normalHitWeight(1,om);
}

namespace{
	struct indexRange{
		std::vector<uint32_t>::iterator cur, end;
		indexRange(std::vector<uint32_t>& container):
		cur(container.begin()),end(container.end()){}
	};
}

void PMTResponseSimulator::timeMergeHits(std::vector<I3MCPulse>& hits, ParticlePulseIndexMap& aux){
	//This is the time span in which adjacent hits will be merged
	//The chosen value, 200 picoseconds, is based on the advice of
	//N. Whitehorn with regard to the highest resolution which wavedeform
	//achieves in unfolding pulses
	const double timeWindow = .2; //ns

	assert(!hits.empty());

	std::vector<indexRange> ranges;
	for(ParticlePulseIndexMap::iterator it=aux.begin(), end=aux.end(); it!=end; it++)
		ranges.push_back(indexRange(it->second));

	uint32_t minOldIndex=0, maxOldIndex=1, newIndex=0, removed=0;
	std::vector<I3MCPulse>::iterator it1=hits.begin(), it2=hits.begin();
	std::vector<I3MCPulse> newHits;
	bool madeChanges=false;
	it2++;
	while(it1!=hits.end()){
		bool doMerge=false;
		double weightSum=it1->charge;
		double avgTime=it1->time;
		log_trace_stream("merge base time now " << avgTime);
		//collect all hits which can be merged
		while(it2!=hits.end() && (it2->time-it1->time)<=timeWindow){
			doMerge=true;
			log_trace_stream("will merge hit " << (it2->time-it1->time) << " ns later");
			avgTime=(weightSum*avgTime + it2->charge*it2->time)/(weightSum + it2->charge);
			weightSum+=it2->charge;
			it2++;
			maxOldIndex++;
		}
		//Merge the set of hits if necessary
		if(doMerge){
			removed+=maxOldIndex-minOldIndex-1;
			if(!madeChanges){ //copy all of the unmerged hits that were skipped previously
				std::copy(hits.begin(),it1,std::back_inserter(newHits));
				newIndex+=std::distance(hits.begin(),it1);
				//advance the aux iterators
				for(std::vector<indexRange>::iterator ait=ranges.begin(), aitEnd=ranges.end(); ait!=aitEnd; ait++)
					ait->cur=std::lower_bound(ait->cur,ait->end,newIndex);
				madeChanges=true;
			}
			log_trace_stream("merged hit time is " << avgTime << " and weight is " << weightSum);
			newHits.push_back(*it1);
			newHits.back().time=avgTime;
			newHits.back().charge=weightSum;
			//update aux, replacing all indices in [minOldIndex,maxOldIndex) with newIndex
			for(std::vector<indexRange>::iterator ait=ranges.begin(), aitEnd=ranges.end(); ait!=aitEnd; ait++){
				while(ait->cur!=ait->end && *ait->cur<maxOldIndex){
					*(ait->cur)=newIndex; //overwrite the index with the index of the merged pulse
					ait->cur++; //advance to the next index
				}
			}
			newIndex++;
		}
		else if(madeChanges){
			std::copy(it1,it2,std::back_inserter(newHits));
			for(std::vector<indexRange>::iterator ait=ranges.begin(), aitEnd=ranges.end(); ait!=aitEnd; ait++){
				while(ait->cur!=ait->end && *ait->cur<maxOldIndex){
					*(ait->cur)-=removed; //decrease the index by the number of pulses removed
					ait->cur++; //advance to the next index
				}
			}
			newIndex+=std::distance(it1,it2);
		}
		it1=it2++; //skip to the next possible group
		minOldIndex=maxOldIndex++;
	}
	if(madeChanges){
		hits.swap(newHits);
		//deduplicate the contents of aux
		for(ParticlePulseIndexMap::iterator it=aux.begin(), end=aux.end(); it!=end; it++){
			std::vector<uint32_t> temp;
			std::unique_copy(it->second.begin(),it->second.end(),std::back_inserter(temp));
			it->second.swap(temp);
		}
	}
}

struct saturationParams{
private:
	double gain; //the PMT gain
	double A, B, Ap; //the parameters for the saturation
	//where A and B are defined as by T. Waldenmeier and Ap = A/I_s

public:
	saturationParams(double log10Gain, bool newParameterization=true){
		gain=pow(10.0,log10Gain)/I3Units::milliampere;
		if(newParameterization){ //improved fits by T. Feusels
			A = pow(10,-.364+log10Gain*(.4247-.005765*log10Gain));
			B = pow(10,-1.696+log10Gain*(.9985-.06699*log10Gain));
			Ap = A/pow(10,-1.103+log10Gain*(.7194-.0372*log10Gain));
		}
		else{ //original fits by T. Waldenmeier
			A = pow(10,-1.008+log10Gain*(.818-.043*log10Gain));
			B = pow(10,-1.473+log10Gain*(1.008-.067*log10Gain));
			Ap = A/pow(10,-.933+log10Gain*(.713-.037*log10Gain));
		}
	}

	//Calculate the factor by which an ideal PMT output current should be reduced,
	//due to saturation. This is just the 'inverse' parametrization by Tilo Waldenmeier
	double saturate(double I){
		if(I<=0.0)
			return(1.0);
		//I is the current of photoelectrons per ns
		//but we need the current at the anode, in mA (since this unit is assumed by Tilo's formula)
		I*=gain;
		double ln = log(1+A/I);
		return(ln/(ln+Ap*exp(-B/I)));
	}
};

void PMTResponseSimulator::saturate(std::vector<I3MCPulse>& hits, double pmtVoltage, const I3DOMCalibration& cal){
	if(hits.empty())
		return;
	const double log10Gain = cal.GetHVGainFit().slope*log10(pmtVoltage/I3Units::V)+cal.GetHVGainFit().intercept;
	saturationParams params(log10Gain);

	//To determine the saturation factor for each hit we need an estimate of
	//the the current at the anode. We can compute this by summing up the
	//contributions from all of the hits. The usual template for this is a
	//gaussian, but if we use a form like exp(-abs(t)) the calculation can be
	//made much more efficient with little change in the result. Using such a
	//template, the current is just the sum of the 'past current', from hits
	//in the past and the 'future current' from hits in the future, and each
	//of these components can be updated simply for each new hit encountered.
	//In the case of the 'future current', finding the initial value requires
	//iterating through the hits in reverse, and due to limited floating
	//point precision this caclulation cannot be accurately run in reverse
	//(forward in time), so we just store the values encountered on a stack.
	const double anodePulseTau=2.2; //ns
	double currentNormalization=1./(2.*anodePulseTau);
	std::stack<double> futureCurrents;
	double pastCurrent=0.0;
	//run through the pulses in reverse to compute the future current
	double lastTime=hits.back().time;
	futureCurrents.push(0.0);
	for(std::vector<I3MCPulse>::reverse_iterator hit=hits.rbegin(), end=hits.rend(); hit!=end; hit++){
		double time=hit->time;
		double futureCurrent=futureCurrents.top();
		futureCurrent*=exp((time-lastTime)/anodePulseTau);
		futureCurrent+=currentNormalization*hit->charge;
		futureCurrents.push(futureCurrent);
		lastTime=time;
	}
	//run through the pulses in order computing the past current, rewinding
	//the future current computation from before, and applying saturation
	lastTime=hits.front().time;
	for(std::vector<I3MCPulse>::iterator hit=hits.begin(), end=hits.end(); hit!=end; hit++){
		double time=hit->time;
		double weight=hit->charge;
		pastCurrent*=exp((lastTime-time)/anodePulseTau);
		pastCurrent+=currentNormalization*weight;
		double futureCurrent=futureCurrents.top();
		futureCurrents.pop();
		futureCurrent-=currentNormalization*weight; //subtract the hit weight to avoid double counting
		hit->charge=weight*params.saturate(pastCurrent+futureCurrent);
		lastTime=time;
	}
}

double PMTResponseSimulator::fisherTippett(double location, double scale,
                                           double logLowerBound, double logUpperBound){
	return(location - scale * log(-log(randomService_->Uniform(logLowerBound,logUpperBound))));
}

PMTResponseSimulator::pmtChargeDistribution::pmtChargeDistribution(double exp_width_,
                                                                   double gaus_mean_,
                                                                   double gaus_width_,
                                                                   double amp_ratio_):
exp_width(exp_width_),
gaus_mean(gaus_mean_),
gaus_width(gaus_width_),
amp_ratio(amp_ratio_),
normalization(1./(exp_width_ + amp_ratio_*constants::root_two_pi<double>()*gaus_width_))
{
	assert(exp_width>0.0); assert(gaus_mean>0.0);
	assert(gaus_width>0.0); assert(amp_ratio>0.0);
}

double PMTResponseSimulator::pmtChargeDistribution::operator()(double q) const{
	double e=(q-gaus_mean)/gaus_width;
	return(normalization*(exp(-q/exp_width) + amp_ratio*exp(-0.5*e*e)));
}
