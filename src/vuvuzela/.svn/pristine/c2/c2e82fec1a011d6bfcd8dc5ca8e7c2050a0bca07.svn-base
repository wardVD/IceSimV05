// $id$

#include "vuvuzela/VuvuzelaFunctions.h"

/* ******************************************************************** */ 
/* MakeNonThermalHits                                                   */
/** Produces the decay and scintillation hits. The former are produced 
 * by throwing decays uniformly in time from start to stop. The latter 
 * are produced by simulating a standard normal distribution (mu=0, sigma=1)
 * and exponentiating according to
 * f(x) = 10^(mean + sigma*g(x))
 * Hits produced are added to the buffer if the dt satisfy dt>2 us
 *
 *  \param random A random service capable of producing a uniform distribution
 *  \param bufferSeries The buffer holding the hit times. New hits are added here
 *  \param bufferTime The current time of the buffer. This is used to keep ordering of the buffer hits.
 *  \param decayRate The decay rate
 *  \param nHits The mean number of scintillation hits from each decay
 *  \param mean The mean for the lognormal describing scintillation in Log10(dt/ns)
 *  \param sigma The sigma for the lognormal describing scintillation in Log10(dt/ns)
 *  \param start The time for the event to begin
 *  \param stop The time for the event to end
 *  \returns void
 *//******************************************************************* */ 
void MakeNonThermalHits(I3RandomServicePtr random,
                        std::set<double>& bufferSeries,
                        const double bufferTime,
                        const double decayRate,
                        const double nHits,
                        const double mean,
                        const double sigma,
                        const double start,
                        const double stop,
			bool disableCutoff)
{
  // First hit of the clusters
  int nClusters = random->Poisson(fabs(stop-start) * decayRate);

  for (int i=0; i < nClusters; i++){
    // The first hit of the clusters is uniformly distributed in time
    // We want to keep the current time in the event
    double currentTime = bufferTime + random->Uniform(0, stop-start);
    bufferSeries.insert( currentTime );

    // The number of hits is drawn from a Poisson distribution with
    // a constant mean.
    int nhits = random->Poisson(nHits);
    
    // Throw a standard normal distribution
    std::set<double> deltaTs;
    for (int j=0; j<nhits; j++){
      double dt = random->Gaus(0, 1);

      // Add the hit from the cluster onto the event after converting to lognormal
      deltaTs.insert( pow(10, mean + sigma*dt) );
    } // end loop over nHits

    // Loop over the deltaTs and get the hit times.
    std::set<double>::iterator dtIter;
    double hitTime = *(deltaTs.begin()) * 2 + currentTime;
    for(dtIter = deltaTs.begin(); dtIter != deltaTs.end(); ++dtIter){


      // Truncate the distribution?
      if (!disableCutoff)
	if (*dtIter < 2*I3Units::microsecond) continue;

      //double hitTime = std::accumulate(deltaTs.begin(), dtIter,
      //                                 *(deltaTs.begin()) + currentTime);

      // Put the hit time into the buffer
      bufferSeries.insert(hitTime);
      hitTime += (*dtIter);

    }      
        
  } // end loop over nClusters
}

/* ******************************************************************** */ 
/* MakeThermalHits                                                      */
/** Produces the thermal hits by throwing hits uniform in time.
 *
 *  \param random A random service capable of producing a uniform distribution
 *  \param bufferSeries The buffer holding the hit times. Hits are added to this directly
 *  \param bufferTime The current time of the buffer. This is used to keep ordering of the buffer hits.
 *  \param rate The thermal rate
 *  \param start The time for the event to begin
 *  \param stop The time for the event to end
 *  \returns void
 *//******************************************************************* */ 
void MakeThermalHits(I3RandomServicePtr random,
		     std::set<double>& bufferSeries,
		     const double bufferTime,
		     const double rate, 
		     const double start, 
		     const double stop){
  
  double mean = fabs(stop-start)*rate;
  int nhits = random->Poisson(mean);

  // Put the hit time directly into the buffer
  for (int i=0; i<nhits; i++)
    bufferSeries.insert( bufferTime + random->Uniform(0, stop-start) );
  
  return;
}

/* ******************************************************************** */ 
/* GetTimeRange                                                         */
/** Reads the hit map to find the time of the first and last hits.
 *  There may be a faster way to do this, but this was taken from
 *  noise-generator.
 *
 *  \param inputHitMap The I3MCPESeriesMapConstPtr to check.
 *  \returns A std::pair constaining the [first, last] hit times from the map.
 *//******************************************************************* */ 
std::pair<double, double> GetTimeRange(I3MCPESeriesMapConstPtr inputHitMap){

  double firstHit = std::numeric_limits<double>::max();
  double lastHit = -std::numeric_limits<double>::max();

  // Loop over the map
  I3MCPESeriesMap::const_iterator map_iter;
  for(map_iter = inputHitMap->begin(); map_iter != inputHitMap->end(); ++map_iter){

    // Loop over the hits
    const I3MCPESeries& hitSeries = map_iter->second;
    I3MCPESeries::const_iterator series_iter;
    for (series_iter = hitSeries.begin(); series_iter != hitSeries.end(); ++series_iter){
      
      // If this hit is earlier or later than firstHit or lastHit,
      // save it to the right place.
      double thisTime = series_iter->time;
      if (thisTime < firstHit) firstHit = thisTime;
      if (thisTime > lastHit) lastHit = thisTime;
     }
  }

  if (firstHit == std::numeric_limits<double>::max() || lastHit == -std::numeric_limits<double>::max())
    return std::pair<double, double>(0, 0);

  return std::pair<double, double>(firstHit, lastHit);
}

/* ******************************************************************** */ 
/* CompareMCPEs                                                        */
/** A comparison operator for I3MCPEs. Used as a functor when
 *  merging the input and noise hit maps. 
 *
 *  \param lhs The first MCPE
 *  \param rhs The second MCPE
 *  \returns True if lhs comes before rhs, false otherwise.
 *//******************************************************************* */ 
bool CompareMCPEs(const I3MCPE& lhs, const I3MCPE& rhs){
  return lhs.time < rhs.time;
}

/* ******************************************************************** */ 
/* AddHitSeries                                                        */
/** Merges two I3MCPEMaps. Shamelessly stolen wholesale from noise-generator.
 *
 *  \param firstterm The input hit map from the frame
 *  \param secondterm The noise hit map produced by Vuvuzela
 *  \returns An I3MCPESeriesMapConstPtr for the merged and sorted hitmap
 *//******************************************************************* */ 
I3MCPESeriesMapConstPtr AddHitMaps(I3MCPESeriesMapConstPtr firstterm, I3MCPESeriesMapConstPtr secondterm){
  I3MCPESeriesMapPtr lhs(new I3MCPESeriesMap(*firstterm));
  I3MCPESeriesMap::const_iterator iter;

  // Loop over the noise hits
  for(iter = secondterm->begin(); iter != secondterm->end(); ++iter){
    // If there are prexisting hits on that DOM, then save those first
    const OMKey& dom = iter->first;
    I3MCPESeriesMap::const_iterator keyVectPair = lhs->find(dom);
    if(keyVectPair != lhs->end()){
      I3MCPESeries newHS = keyVectPair->second;
      const I3MCPESeries& hs = iter->second;
      I3MCPESeries::const_iterator hsIter;
      assert(!iter->second.empty());
      // Then add each of the noise hits. Could be done with STL copy too.
      for(hsIter = hs.begin(); hsIter != hs.end(); ++hsIter) 
        newHS.push_back(*hsIter); 

      // Sort the hit series
      sort(newHS.begin(), newHS.end(), CompareMCPEs);
      (*lhs)[dom] = newHS;
    }
    // If there weren't any prexisting hits, just copy the noise hits
    else if (!iter->second.empty()){
      (*lhs)[dom] = iter->second;
    }
  }
  return I3MCPESeriesMapConstPtr(lhs);
}
