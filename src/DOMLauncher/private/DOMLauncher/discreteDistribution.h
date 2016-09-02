#ifndef ROSENCRANTZ_DISCRETEDISTRIBUTION_INCLUDED
#define ROSENCRANTZ_DISCRETEDISTRIBUTION_INCLUDED

///Helper code for generating discrete distributions with non-uniform probabilities, using an
///I3RandomService as the underlying random engine.
///With a new version of Boost,boost::random::discrete_distribution does all the work quite nicely.
///However, as long as icetray uses an old version where this isn't available a simplistic
///work-around is used.

#include <boost/version.hpp>
#if BOOST_VERSION>=104700
//boost::random::discrete_distribution was first available in 1.47.0
#define USE_BOOST_DISCRETE_RANDOM 1
#else
#define USE_BOOST_DISCRETE_RANDOM 0
#endif

#if USE_BOOST_DISCRETE_RANDOM
#include <boost/random/discrete_distribution.hpp>

///A simple wrapper for I3RandomService which provides the interface expected by Boost Random
///(and the standard C++11 library [rand.req.urng]).
struct I3RandomServiceBoostAdapter{
	typedef unsigned int result_type;

	I3RandomService* service; //not owned!

	result_type min() const{ return(0); }
	result_type max() const{ return(std::numeric_limits<result_type>::max()-1); }
	result_type operator()(){ return(service->Integer(std::numeric_limits<result_type>::max())); }

	I3RandomServiceBoostAdapter(I3RandomService* s):service(s){}
};

typedef I3RandomServiceBoostAdapter random_adapter;
typedef boost::random::discrete_distribution<unsigned int,double> discrete_distribution;

#else
typedef I3RandomService* random_adapter;

template<typename PairType>
bool compareFirst(PairType p1, PairType p2){
	return(p1.first<p2.first);
}

///A really dumb implementation of a discrete distribution with given probabilities which takes
///time linear in the number of entries, rather than constant like the boost implementation.
class workaround_discrete_distribution{
private:
	typedef std::vector<std::pair<double, unsigned int> > tableType;
	///The mapping of cumulative probabilities to result values
    tableType table;
public:
	template<class Iterator>
	workaround_discrete_distribution(Iterator first, Iterator last){
		unsigned int count = std::distance(first,last);
		//Build the mapping table, and normalize the probabilites in the process
		table.resize(count);
		const double sum = std::accumulate(first, last, 0u);
		for(unsigned int i=0; i<count; i++,first++){
			assert(*first>=0.0 && "Probabilities must be positive");
			table[i]=std::make_pair(*first/sum,i);
		}
		//Rather than being totally dumb, we can at least sort the results so that the most
		//likely are checked first.
		std::sort(table.begin(),table.end(),&compareFirst<tableType::value_type>);
		//Convert the probabilities to cumulative probabilities
		double s2=0.0;
		for(unsigned int i=0; i<count; i++,first++){
			s2+=table[i].first;
			table[i].first=s2;
		}
		table.back().first=1.0; //just in case of round-off
	}

	unsigned int operator()(random_adapter rand){
		//Generate a random number in [0,1] and search for the first cumulative probability
		//bin which contains it
		double x=rand->Uniform(1.0);
		unsigned int idx=0;
		while(table[idx].first<x)
			idx++;
		return(table[idx].second);
	}
};
typedef workaround_discrete_distribution discrete_distribution;
#endif

#endif //include guard
