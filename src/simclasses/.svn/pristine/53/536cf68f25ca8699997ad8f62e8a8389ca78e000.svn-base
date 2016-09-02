#include <simclasses/I3ParticleIDMap.hpp>
#include <ostream>
#include "I3SequenceOpOStream.h"

std::vector<I3ParticleID> findParents(uint32_t pulseIndex, const ParticlePulseIndexMap& idMap){
  std::vector<I3ParticleID> results;
  for(ParticlePulseIndexMap::const_iterator particle=idMap.begin(), particleEnd=idMap.end(); particle!=particleEnd; particle++){
    std::vector<uint32_t>::const_iterator it=std::lower_bound(particle->second.begin(),particle->second.end(),pulseIndex);
    if(it!=particle->second.end() && *it==pulseIndex)
      results.push_back(particle->first);
  }
  return(results);
}

I3_SERIALIZABLE(I3ParticleIDMap);
