/* ******************************************************************** */ 
/** class VuvuzelaFunctions
 *  Version $Id: $
 *  date: $Date: $
 *  @author Michael Larson
 *  (c) 2012 IceCube Collaboration
 *
 *  \brief A set of functions used by Vuvuzela. Separated to simplify
 *  the testing and readability slightly. 
 *//******************************************************************* */ 
#ifndef VUVUZELAFUNCTIONS_H_INCLUDED
#define VUVUZELAFUNCTIONS_H_INCLUDED

#include <algorithm>
#include <functional>
#include <numeric>
#include <set>
#include <math.h>

#include "simclasses/I3MCPE.h"

#include "icetray/I3Units.h"
#include "icetray/OMKey.h"

#include "phys-services/I3RandomService.h"

/* ******************************************************************** */ 
/* MakeNonThermalHits                                                   */
/** \brief Produces the decay and scintillation hits. The former are produced 
 * by throwing decays uniformly in time from start to stop. The latter 
 * follow a lognormal distribution.
 *//******************************************************************* */ 
void MakeNonThermalHits(I3RandomServicePtr random,
			std::set<double>& bufferSeries,
			const double bufferTime,
			const double rate, 
			const double index, 
			const double mean, 
			const double sigma,
			const double start, 
			const double stop,
			bool disableCutoff);

/* ******************************************************************** */ 
/* MakeThermalHits                                                      */
/** \brief Produces the thermal hits by throwing hits uniform in time.
 *//******************************************************************* */ 
void MakeThermalHits(I3RandomServicePtr random,
		     std::set<double>& bufferSeries,
		     const double bufferTime,
		     const double rate, 
		     const double start, 
		     const double stop);

/* ******************************************************************** */ 
/* GetTimeRange                                                         */
/** \brief Reads the hit map to find the time of the first and last hits.
 *  There may be a faster way to do this, but this was taken from
 *  noise-generator.
 *//******************************************************************* */ 
std::pair<double, double> GetTimeRange(I3MCPESeriesMapConstPtr inputHitMap);

/* ******************************************************************** */ 
/* CompareMCPEs                                                        */
/** \brief A comparison operator for I3MCPEs. Used as a functor when
 *  merging the input and noise hit maps. 
 *//******************************************************************* */ 
bool CompareMCPEs(const I3MCPE& lhs, const I3MCPE& rhs);

/* ******************************************************************** */ 
/* AddHitMaps                                                           */
/** \brief Merges two I3MCPEMaps. Shamelessly stolen wholesale from noise-generator.
*//******************************************************************* */ 
I3MCPESeriesMapConstPtr AddHitMaps(I3MCPESeriesMapConstPtr firstterm,
                                    I3MCPESeriesMapConstPtr secondterm);

#endif
