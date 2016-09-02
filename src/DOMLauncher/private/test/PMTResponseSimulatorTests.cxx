#include <I3Test.h>
#include <vector>
#include <boost/make_shared.hpp>
#include <boost/math/constants/constants.hpp>
#include <icetray/I3Tray.h>
#include <icetray/I3ServiceFactory.h>
#include <icetray/I3Units.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <simclasses/I3MCPE.h>
#include <dataclasses/physics/I3MCHit.h>
#include <simclasses/I3MCPulse.h>
#include <DOMLauncher/PMTResponseSimulator.h>
namespace constants = boost::math::constants;

TEST_GROUP(PMTResponseSimulator);

//======================================
// Helper functions

//function for quickly building simple hits
I3MCPE makeHit(double time, unsigned int weight=1, uint64_t parentMajorID=0, int parentMinorID=-1){
	I3MCPE hit(parentMajorID,parentMinorID);
	hit.time=time;
	hit.npe=weight;
	return(hit);
}

//an arbitrary OM to use for testing, and basic calibration and status information for it
OMKey testOM(40,40);

//give the test OM a typical gain as a function of high voltage
boost::shared_ptr<I3Calibration> getTestCalibration(){
	static boost::shared_ptr<I3Calibration> testCalibration;
	if(!testCalibration){
		testCalibration=boost::make_shared<I3Calibration>();
		LinearFit hvGainCal;
		hvGainCal.slope=7.5;
		hvGainCal.intercept=-16.5;
		testCalibration->domCal[testOM].SetHVGainFit(hvGainCal);
	}
	return(testCalibration);
}

//set the test OM to the voltage necessary for a gain of 10^7
boost::shared_ptr<I3DetectorStatus> getTestStatus(){
	static boost::shared_ptr<I3DetectorStatus> testStatus;
	if(!testStatus){
		testStatus=boost::make_shared<I3DetectorStatus>();
		testStatus->domStatus[testOM].pmtHV=1.359e3*I3Units::V;
	}
	return(testStatus);
}

//put a set of hits, assigned to the test OM, into a frame along with necessary calibration and status data
boost::shared_ptr<I3Frame> frameWithData(const std::vector<I3MCPE>& hits){
	boost::shared_ptr<I3Frame> frame(new I3Frame(I3Frame::DAQ));
	frame->Put(getTestCalibration());
	frame->Put(getTestStatus());
	boost::shared_ptr<I3Map<OMKey,std::vector<I3MCPE> > > hitMap(new I3Map<OMKey,std::vector<I3MCPE> >);
	(*hitMap)[testOM]=hits;
	frame->Put("I3MCPESeriesMap",hitMap);
	return(frame);
}

//======================================
// Infrastructure for running a module without a tray

static struct config_end{} end_config;

template<typename T>
class param_setter {
	T& owner;
	I3Configuration& config;
public:
	param_setter<T>(T& t, I3Configuration& c):owner(t),config(c){}
	param_setter<T>(const param_setter<T>& rhs):owner(rhs.owner),config(rhs.config){}
	
	template <typename U>
	param_setter& operator()(const std::string& paramName, U value){
		config.Set(paramName, boost::python::object(value));
		return(*this);
	}
	void operator()(const config_end&){ owner.FinalizeConfiguration(); };
};

class ServiceFactoryWrapper{
private:
	std::string factoryClassName;
	I3Context& context;
	boost::shared_ptr<I3ServiceFactory> factory;
public:
	ServiceFactoryWrapper(I3Context& context, const std::string& factName):
	factoryClassName(factName),
	context(context),
	factory(I3::Singleton<I3ServiceFactoryFactory>::get_const_instance().Create(factoryClassName)(context))
	{}
	
	param_setter<ServiceFactoryWrapper> Configure(){
		//unfortunately this appears to be impossible without using a const_cast hack
		return(param_setter<ServiceFactoryWrapper>(*this,const_cast<I3Configuration&>(factory->GetConfiguration())));
	}
	void FinalizeConfiguration(){
		factory->Configure();
	}
	
	void installService(){
		factory->InitializeService(context);
	}
};

class PMTResponseSimulatorTestSetup{
private:
	boost::shared_ptr<std::deque<boost::shared_ptr<I3Frame> > > inbox;
protected:
	I3Context& context;
	boost::shared_ptr<PMTResponseSimulator> r;
public:
	PMTResponseSimulatorTestSetup(I3Context& context):
	inbox(new std::deque<boost::shared_ptr<I3Frame> >),
	context(context),
	r(new PMTResponseSimulator(context))
	{
#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
#endif
		r->configuration_.ClassName(I3::name_of(typeid(*r)));
#ifdef __clang__
	#pragma clang diagnostic pop
#endif
		r->inbox_=inbox;
	}
	
	param_setter<PMTResponseSimulatorTestSetup> Configure(){ return(param_setter<PMTResponseSimulatorTestSetup>(*this,r->configuration_)); }
	void FinalizeConfiguration(){
		r->Configure_();
	}
	boost::shared_ptr<I3Frame> processFrame(boost::shared_ptr<I3Frame> frame){
		boost::shared_ptr<std::deque<boost::shared_ptr<I3Frame> > > outbox = r->outboxes_["OutBox"].first;
		inbox->push_front(frame);
		r->Process_();
		if(!outbox->empty()){
			frame=outbox->back();
			outbox->pop_back();
			return(frame);
		}
		return(boost::shared_ptr<I3Frame>());
	}
	
	//behave like a smart pointer
	PMTResponseSimulator& operator*() const{ return(*r); }
	PMTResponseSimulator* operator->() const{ return(r.get()); }
};

//======================================

bool isTimeOrdered(const std::vector<I3MCPulse>& hits){
	if(hits.size()<=1)
		return(true);
	for(std::vector<I3MCPulse>::const_iterator last=hits.begin(), it=hits.begin()+1, end=hits.end(); it!=end; it++,last++){
		if(last->time>it->time)
			return(false);
	}
	return(true);
}

struct MeanVarAccumulator{
	double m,t,w;
	unsigned int n;
	
	MeanVarAccumulator():m(0),t(0),w(0),n(0){}
	
	void Add(double x, double weight=1.0){
		if(n){
			double q=x-m;
			double wnew=w+weight;
			double r=q*weight/wnew;
			t+=weight*(x-m)*(x-m-r);
			m+=r;
			w=wnew;
		}
		else{
			m=x;
			w=weight;
		}
		n++;
	}
	
	double Mean() const{
		if(!n)
			throw std::runtime_error("No mean (no samples)");
		return(m);
	}
	
	double Variance() const{
		if(!n)
			throw std::runtime_error("No variance (no samples)");
		if(n==1)
			return(0);
		return(t/w);
	}
};

//Check that distribution of hit times due to PMT jitter behaves as expected
TEST(1_JitterDistribution){
	std::vector<I3MCPE> hits;
	hits.push_back(makeHit(0.0,10000.0)); //ten thousand hits at time 0
	
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()("Seed",82365)(end_config);
	rng.installService();
	PMTResponseSimulatorTestSetup rts(context);
	rts.Configure()("UsePMTJitter",true)("PrePulseProbability",0)("LatePulseProbability",0)("AfterPulseProbability",0)("ApplySaturation",false)("MergeHits",false)(end_config);
	
	boost::shared_ptr<I3Frame> frame=frameWithData(hits);
	frame=rts.processFrame(frame);
	
	const I3Map<OMKey,std::vector<I3MCPulse> >& outputHits=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("I3MCPulseSeriesMap");
	//compute mean and variance of hit times
	MeanVarAccumulator mv;
	for(std::vector<I3MCPulse>::const_iterator it=outputHits.at(testOM).begin(), end = outputHits.at(testOM).end();              
	    it!=end; it++)
		mv.Add(it->time);
	
	double beta=sqrt(6.*mv.Variance())/constants::pi<double>();
	double mu=mv.Mean()-beta*constants::euler<double>();
	//TODO: should make a more deliberate choice of allowed ranges
	ENSURE_DISTANCE(mu,0.15304,.02,"Jitter location parameter");
	ENSURE_DISTANCE(beta,1.9184,.05,"Jitter scale parameter");
}

//Check that distribution of hit times due to PMT jitter behaves as expected
TEST(1b_JitterDistributionWithMerging){
	std::vector<I3MCPE> hits;
	hits.push_back(makeHit(0.0,10000.0)); //ten thousand hits at time 0
	
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()("Seed",82365)(end_config);
	rng.installService();
	PMTResponseSimulatorTestSetup rts(context);
	rts.Configure()("UsePMTJitter",true)("PrePulseProbability",0)("LatePulseProbability",0)("AfterPulseProbability",0)("ApplySaturation",false)("MergeHits",true)(end_config);
	
	boost::shared_ptr<I3Frame> frame=frameWithData(hits);
	frame=rts.processFrame(frame);
	
	const I3Map<OMKey,std::vector<I3MCPulse> >& outputHits=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("I3MCPulseSeriesMap");
	//compute mean and variance of hit times
	MeanVarAccumulator mv;
	for(std::vector<I3MCPulse>::const_iterator it=outputHits.at(testOM).begin(), end = outputHits.at(testOM).end();
	    it!=end; it++)
		mv.Add(it->time,it->charge);
	
	double beta=sqrt(6.*mv.Variance())/constants::pi<double>();
	double mu=mv.Mean()-beta*constants::euler<double>();
	//TODO: should make a more deliberate choice of allowed ranges
	ENSURE_DISTANCE(mu,0.15304,.02,"Jitter location parameter");
	ENSURE_DISTANCE(beta,1.9184,.05,"Jitter scale parameter");
}

//Other things to test:
// -SPE weights peak at 1?
// -prepulse beahvior with HV?
// -late pulse and afterpulse properties?
// -saturation?

//Check that data is properly sorted into time order in an artificially simple case
TEST(2_TimeOrdering){
	//a set of simple pulses that are out of order
	std::vector<I3MCPE> hits;
	hits.push_back(makeHit(1.0));
	hits.push_back(makeHit(3.0));
	hits.push_back(makeHit(2.0));
	hits.push_back(makeHit(5.0));
	hits.push_back(makeHit(4.0));
	
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()("Seed",67584)(end_config);
	rng.installService();
	PMTResponseSimulatorTestSetup rts(context);
	rts.Configure()("UsePMTJitter",false)("PrePulseProbability",0)("LatePulseProbability",0)("AfterPulseProbability",0)("ApplySaturation",false)("MergeHits",false)(end_config);
	
	boost::shared_ptr<I3Frame> frame=frameWithData(hits);
	frame=rts.processFrame(frame);
	
	const I3Map<OMKey,std::vector<I3MCPulse> >& outputHits=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("I3MCPulseSeriesMap");
	ENSURE(isTimeOrdered(outputHits.at(testOM)),"Hits should be time-ordered");
}

//Check that data is properly sorted into time order when jitter is applied and alternate pulse types are generated
TEST(2b_TimeOrdering_realistic){
	//a large number of pulses which should end up jumbled in time after jitter
	std::vector<I3MCPE> hits;
	const unsigned int nHits=100;
	hits.push_back(makeHit(0.0,nHits));
	
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()("Seed",67584)(end_config);
	rng.installService();
	PMTResponseSimulatorTestSetup rts(context);
	//use large probabilities for alternate pulse types
	rts.Configure()("UsePMTJitter",true)("PrePulseProbability",.1)("LatePulseProbability",.1)("AfterPulseProbability",.1)("ApplySaturation",false)("MergeHits",false)(end_config);
	
	boost::shared_ptr<I3Frame> frame=frameWithData(hits);
	frame=rts.processFrame(frame);
	
	const I3Map<OMKey,std::vector<I3MCPulse> >& outputHits=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("I3MCPulseSeriesMap");
	ENSURE(isTimeOrdered(outputHits.at(testOM)),"Hits should be time-ordered");
	ENSURE(outputHits.at(testOM).size()>=nHits,"Number of hits should never decrease (without merging)");
}

//Check that sorting MCPulses not only produces correctly time-ordered pulses,
//but also correctly maintains information about parent particles
TEST(3_HitSorting){
	std::vector<I3MCPulse> hits;
	ParticlePulseIndexMap aux;
	I3ParticleID p1={538,16};
	I3ParticleID p2={619,12};
	
	//generate some pulses from the two particles which will be interleaved once put in time order
	unsigned int index=0;
	//generate pulses from p1 at even times
	for(unsigned int i=0; i<10; i+=2, index++){
		hits.push_back(I3MCPulse(i));
		aux[p1].push_back(index);
	}
	//generate pulses from p2 at odd times
	for(unsigned int i=1; i<10; i+=2, index++){
		hits.push_back(I3MCPulse(i));
		aux[p2].push_back(index);
	}
	
	sortMCPulses(hits.begin(),hits.end(),aux);
	
	ENSURE(isTimeOrdered(hits),"Hits should be time-ordered");
	ENSURE(aux.size()==2,"Number of parent particles should not change");
	{
		ENSURE(aux.find(p1)!=aux.end(),"particle 1 must appear in the output");
		const std::vector<uint32_t>& indices=aux[p1];
		uint32_t last=0;
		for(std::vector<uint32_t>::const_iterator it=indices.begin(), end=indices.end(); it!=end; it++){
			ENSURE(it==indices.begin() || *it>last,"Pulse indices should be sorted and unique");
			ENSURE(*it%2==0,"Indices for pulses from particle 1 should be even");
			last=*it;
		}
	}
	{
		ENSURE(aux.find(p2)!=aux.end(),"particle 2 must appear in the output");
		const std::vector<uint32_t>& indices=aux[p2];
		uint32_t last=0;
		for(std::vector<uint32_t>::const_iterator it=indices.begin(), end=indices.end(); it!=end; it++){
			ENSURE(*it>last,"Pulse indices should be sorted and unique");
			ENSURE(*it%2==1,"Indices for pulses from particle 2 should be even");
			last=*it;
		}
	}
}

//Check that hit merging works properly:
//Hits with times with .2 ns should be merged
//The merged hit should have a weight equal to the sum of the weights of the original hits,
//and a time given by the weighted average of the times of the original hits.
//Parent particle information should also be correctly updated, in particular when two pulses
//with different parent particles are merged the index of the resulting pulse should appear
//in the index lists of both parents. 
TEST(4_HitMerging){
	std::vector<I3MCPulse> hits;
	ParticlePulseIndexMap aux;
	I3ParticleID p1={538,16};
	I3ParticleID p2={619,12};
	uint32_t hitIndex=0;
	
	//these two hits should be merged
	hits.push_back(I3MCPulse(0.0,1.0));
	aux[p1].push_back(hitIndex++);
	hits.push_back(I3MCPulse(0.1,1.0));
	aux[p1].push_back(hitIndex++);
	
	//this hit should not merge with the preceding or following hits
	hits.push_back(I3MCPulse(0.25,1.0));
	aux[p1].push_back(hitIndex++);
	
	//these two hits should be merged
	//the merged pulse should be a child of both p1 and p2
	hits.push_back(I3MCPulse(0.5,1.0));
	aux[p1].push_back(hitIndex++);
	hits.push_back(I3MCPulse(0.6,1.0));
	aux[p2].push_back(hitIndex++);
	
	//neither of these hits should merge with anything
	hits.push_back(I3MCPulse(0.75,1.0));
	aux[p1].push_back(hitIndex++);
	hits.push_back(I3MCPulse(1.0,1.0));
	aux[p2].push_back(hitIndex++);
	
	PMTResponseSimulator::timeMergeHits(hits,aux);
	
	ENSURE(hits.size()==5,"Should have 3 pulses after merging");
	ENSURE(aux.size()==2,"Number of parent particles should be unchanged");
	ENSURE(aux.find(p1)!=aux.end(),"particle 1 must appear in the output");
	ENSURE(aux.find(p2)!=aux.end(),"particle 2 must appear in the output");
	ENSURE(aux[p1].size()==4,"Particle 1 should have 4 children");
	ENSURE(aux[p2].size()==2,"Particle 2 should have 2 children");
	
	uint32_t p1Expected[4]={0,1,2,3};
	uint32_t p2Expected[2]={2,4};
	
	ENSURE(std::mismatch(aux[p1].begin(),aux[p1].end(),p1Expected)==std::make_pair(aux[p1].end(),p1Expected+4));
	ENSURE(std::mismatch(aux[p2].begin(),aux[p2].end(),p2Expected)==std::make_pair(aux[p2].end(),p2Expected+2));
}

TEST(5_AfterpulseChaining){
	std::vector<I3MCPE> hits;
	hits.push_back(makeHit(0.0));
	
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()("Seed",22000)(end_config);
	rng.installService();
	PMTResponseSimulatorTestSetup rts(context);
	rts.Configure()
	("UsePMTJitter",false)
	("PrePulseProbability",0)
	("LatePulseProbability",0)
	("AfterPulseProbability",.99) //we want a lot of afterpulses
	("ApplySaturation",false)
	("MergeHits",false)(end_config);
	
	boost::shared_ptr<I3Frame> frame=frameWithData(hits);
	frame=rts.processFrame(frame);
	
	const I3Map<OMKey,std::vector<I3MCPulse> >& outputHits=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("I3MCPulseSeriesMap");
	ENSURE(isTimeOrdered(outputHits.at(testOM)),"Hits should be time-ordered");
    ENSURE(outputHits.at(testOM).size()>10,"A number of afterpulses must be generated for this test to work");
    double averageAfterpulseDelay=outputHits.at(testOM).back().time/(outputHits.at(testOM).size()-1);
    ENSURE(averageAfterpulseDelay>5000*I3Units::ns,"Chained afterpulses should extend to later times");
}

TEST(6_SPEPeak){
	std::vector<I3MCPE> hits;
	hits.push_back(makeHit(0.0,20000.0)); //twenty thousand hits at time 0
	
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()("Seed",72873)(end_config);
	rng.installService();
	PMTResponseSimulatorTestSetup rts(context);
	rts.Configure()("UsePMTJitter",true)("PrePulseProbability",0)("LatePulseProbability",0)("AfterPulseProbability",0)("ApplySaturation",false)("MergeHits",false)(end_config);
	
	boost::shared_ptr<I3Frame> frame=frameWithData(hits);
	frame=rts.processFrame(frame);
	
	const I3Map<OMKey,std::vector<I3MCPulse> >& outputHits=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("I3MCPulseSeriesMap");
	
	//trivial histogram
	const double baseCharge=0.55;
	const double chargeStep=0.1;
	const unsigned int chargeBins=9;
	double histogram[chargeBins];
	
	for(std::vector<I3MCPulse>::const_iterator it=outputHits.at(testOM).begin(), end = outputHits.at(testOM).end();
	    it!=end; it++){
		double q=it->charge;
		int bin=(q-baseCharge)/chargeStep;
		if(q>=baseCharge && bin<chargeBins)
			histogram[bin]++;
	}
	
	std::cout << "charge histogram: " << std::endl;
	unsigned int maxBin=0;
	double maxValue=-1;
	for(unsigned int i=0; i<chargeBins; i++){
		std::cout << baseCharge+i*chargeStep << ": " << histogram[i] << std::endl;
		if(histogram[i]>maxValue){
			maxValue=histogram[i];
			maxBin=i;
		}
	}
	
	unsigned int correctMaxBin=(1.0-baseCharge)/chargeStep;
	ENSURE_EQUAL(maxBin,correctMaxBin, "SPE distribution should peak at 1");
}

TEST(7_Saturation){
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()("Seed",72873)(end_config);
	rng.installService();
	PMTResponseSimulatorTestSetup rts(context);
	rts.Configure()("UsePMTJitter",true)("PrePulseProbability",0)("LatePulseProbability",0)("AfterPulseProbability",0)("ApplySaturation",true)("UseSPEDistribution",false)(end_config);
	
	for(double charge=1; charge<=1e4; charge+=(charge<100?1:10)){
		std::vector<I3MCPE> hits;
		hits.push_back(makeHit(0.0,charge));
		boost::shared_ptr<I3Frame> frame=frameWithData(hits);
		frame=rts.processFrame(frame);
		const I3Map<OMKey,std::vector<I3MCPulse> >& outputHits=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("I3MCPulseSeriesMap");
		//This is sort of a wimpy test, but it's something
		ENSURE(outputHits.at(testOM).front().charge <= charge, "saturation should only reduce charge");
	}
}

template<typename T>
T acculumateHitPropertyForType(const I3Map<OMKey,std::vector<I3MCPulse> >& hits, T(I3MCHit::*propertyGetter)() const,I3MCPulse::PulseSource chosenType){
	T sum=0;
	for(I3Map<OMKey,std::vector<I3MCPulse> >::const_iterator om=hits.begin(), omend=hits.end(); om!=omend; om++){
		for(std::vector<I3MCPulse>::const_iterator hit=om->second.begin(), hitend=om->second.end(); hit!=hitend; hit++){
			if(hit->source==chosenType)
				sum+=(*hit.*propertyGetter)();
		}
	}
	return(sum);
}

template<typename T>
T acculumateHitPropertyForType(const I3Map<OMKey,std::vector<I3MCPulse> >& hits, T I3MCPulse::*property, I3MCPulse::PulseSource chosenType){
	T sum=0;
	for(I3Map<OMKey,std::vector<I3MCPulse> >::const_iterator om=hits.begin(), omend=hits.end(); om!=omend; om++){
		for(std::vector<I3MCPulse>::const_iterator hit=om->second.begin(), hitend=om->second.end(); hit!=hitend; hit++){
			if(hit->source==chosenType)
				sum+=*hit.*property;
		}
	}
	return(sum);
}

uint64_t countHitType(const I3Map<OMKey,std::vector<I3MCPulse> >& hits, I3MCPulse::PulseSource chosenType){
	uint64_t sum=0;
	for(I3Map<OMKey,std::vector<I3MCPulse> >::const_iterator om=hits.begin(), omend=hits.end(); om!=omend; om++){
		for(std::vector<I3MCPulse>::const_iterator hit=om->second.begin(), hitend=om->second.end(); hit!=hitend; hit++){
			if(hit->source==chosenType)
				sum++;
		}
	}
	return(sum);
}

///check whether an observation is within the 1 sigma range for a binomial process
///\param observed the number of observed successes
///\param trials the number of trials conducted
///\param p the probability of success in each trial
///\return whether the the observed number of successes is within the range [expected-sigma,expected+sigma]
bool checkBinomial(unsigned int observed, unsigned int trials, double p){
	double variance=trials*p*(1-p);
	return(std::abs(observed-p*trials)<sqrt(variance));
}

///check that an observation is in the 1 sigma range for a hybrid binomial-geometric process
///In this process we begin each trial as a standard Bernoulli trial, but upon success the trial
///continues with additional subtrials (each with the same probability of success as the original)
///until a failure occurs.
///This is useful because it models the process of normal hits giving rise to afterpulses,
///which can themselves generate additional afterpulses.
///\param observed the number of observed successes
///\param trials the number of trials conducted
///\param p the probability of success in each trial
///\return whether the the observed number of successes is within the range [expected-sigma,expected+sigma]
bool checkBinomialWithGeometric(unsigned int observed, unsigned int trials, double p){
    double variance=trials*p*(1-p)+trials*p*p/(1-p)/(1-p);
	return(std::abs(observed-p*trials/(1-p))<sqrt(variance));
}

TEST(8_WholeShebang){
	const unsigned int nTrials=5000;
	const unsigned int nHits=10000; //number of hits to generate per trial
	
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()("Seed",46256)(end_config);
	rng.installService();
	PMTResponseSimulatorTestSetup rts(context);
	const double PrePulseProbability=0.003;
	const double LatePulseProbability=0.035;
	const double AfterPulseProbability=0.0593;
	rts.Configure()("UsePMTJitter",true)
	("PrePulseProbability",PrePulseProbability)
	("LatePulseProbability",LatePulseProbability)
	("AfterPulseProbability",AfterPulseProbability)
	("ApplySaturation",false)
	("MergeHits",false)
	(end_config);
	
	unsigned int nRegularCountOK=0;
	unsigned int nPreCountOK=0;
	unsigned int nLateCountOK=0;
	unsigned int nAfterCountOK=0;
	double totalRegularCharge=0.0;
	uint64_t totalRegularHits=0;
	double totalPreCharge=0.0;
	uint64_t totalPreHits=0;
	double totalLateCharge=0.0;
	uint64_t totalLateHits=0;
	double totalEarlyAfterCharge=0.0;
	uint64_t totalEarlyAfterHits=0;
	double totalAfterCharge=0.0;
	uint64_t totalAfterHits=0;
	for(unsigned int i=0; i<nTrials; i++){
		std::vector<I3MCPE> hits(1,makeHit(0.0,nHits));
		boost::shared_ptr<I3Frame> frame=frameWithData(hits);
		frame=rts.processFrame(frame);
		
		const I3Map<OMKey,std::vector<I3MCPulse> >& outputHits=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("I3MCPulseSeriesMap");
		ENSURE(isTimeOrdered(outputHits.at(testOM)),"Hits should be time-ordered");
		ENSURE(outputHits.at(testOM).size()>=nHits,"Number of hits should never decrease (without merging)");
		uint64_t nRegular=countHitType(outputHits,I3MCPulse::PE);
		totalRegularHits+=nRegular;
		if(checkBinomial(nRegular,nHits,(1.0-PrePulseProbability-LatePulseProbability)))
			nRegularCountOK++;
		uint64_t nPre=countHitType(outputHits,I3MCPulse::PRE_PULSE);
		totalPreHits+=nPre;
		if(checkBinomial(nPre,nHits,PrePulseProbability))
			nPreCountOK++;
		uint64_t nLate=countHitType(outputHits,I3MCPulse::ELASTIC_LATE_PULSE)+countHitType(outputHits,I3MCPulse::INELASTIC_LATE_PULSE);
		totalLateHits+=nLate;
		if(checkBinomial(nLate,nHits,LatePulseProbability))
			nLateCountOK++;
		uint64_t nAfter=countHitType(outputHits,I3MCPulse::AFTER_PULSE)+countHitType(outputHits,I3MCPulse::EARLY_AFTER_PULSE);
		totalAfterHits+=countHitType(outputHits,I3MCPulse::AFTER_PULSE);
		totalEarlyAfterHits+=countHitType(outputHits,I3MCPulse::EARLY_AFTER_PULSE);
		if(checkBinomialWithGeometric(nAfter,nHits,AfterPulseProbability))
			nAfterCountOK++;
		totalRegularCharge+=acculumateHitPropertyForType(outputHits,&I3MCPulse::charge,I3MCPulse::PE);
		totalPreCharge+=acculumateHitPropertyForType(outputHits,&I3MCPulse::charge,I3MCPulse::PRE_PULSE);
		totalLateCharge+=acculumateHitPropertyForType(outputHits,&I3MCPulse::charge,I3MCPulse::ELASTIC_LATE_PULSE);
		totalLateCharge+=acculumateHitPropertyForType(outputHits,&I3MCPulse::charge,I3MCPulse::INELASTIC_LATE_PULSE);
		totalAfterCharge+=acculumateHitPropertyForType(outputHits,&I3MCPulse::charge,I3MCPulse::AFTER_PULSE);
		totalEarlyAfterCharge+=acculumateHitPropertyForType(outputHits,&I3MCPulse::charge,I3MCPulse::EARLY_AFTER_PULSE);
	}
	//check that we generate the right number of each type of pulse,
	//in that about 68% of the time the number generated is within
	//the expected 1 sigma interval
	ENSURE_DISTANCE(nRegularCountOK/double(nTrials),.68,.05,"Expected number of regular pulses");
	ENSURE_DISTANCE(nPreCountOK/double(nTrials),.68,.05,"Expected number of prepulses");
	ENSURE_DISTANCE(nLateCountOK/double(nTrials),.68,.05,"Expected number of late pulses");
	ENSURE_DISTANCE(nAfterCountOK/double(nTrials),.68,.05,"Expected number of afterpulses");
	//Check that various types of pulses have the right average charges
	const double expectedAverageRegularCharge=.85;
	const double expectedAveragePreCharge=(1./15)*(1.365e3*I3Units::V/getTestStatus()->domStatus[testOM].pmtHV);
	//The following number is _not_ 13.31-3.386*euler_gamma because the tails of the gumbel distribution are cut off
	const double expectedAverageEarlyAfterCharge=11.97275;
	ENSURE_DISTANCE(totalRegularCharge/totalRegularHits,expectedAverageRegularCharge,.02,"Expected average regular pulse charge");
	ENSURE_DISTANCE(totalPreCharge/totalPreHits,expectedAveragePreCharge,.02,"Expected average prepulse charge");
	ENSURE_DISTANCE(totalLateCharge/totalLateHits,expectedAverageRegularCharge,.02,"Expected average late pulse charge");
	ENSURE_DISTANCE(totalAfterCharge/totalAfterHits,expectedAverageRegularCharge,.02,"Expected average afterpulse charge");
	ENSURE_DISTANCE(totalEarlyAfterCharge/totalEarlyAfterHits,expectedAverageEarlyAfterCharge,.04,"Expected average early afterpulse charge");
}
