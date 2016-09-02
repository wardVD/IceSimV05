/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file direct_hits/calculator.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the implementation of the calculator functions that
 *        are defined inside the common_variables::direct_hits
 *        namespace.
 *
 *        This file is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation; either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        This program is distributed in the hope that it will be useful,
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */
#include <cmath>
#include <vector>

#include "phys-services/I3Calculator.h"

#include "CommonVariables/hit_multiplicity/NHitStringsIterativeCalculator.h"
#include "CommonVariables/track_characteristics/calculator.h"

#include "CommonVariables/direct_hits/calculator.h"

namespace hm = common_variables::hit_multiplicity;
namespace tc = common_variables::track_characteristics;

//##############################################################################
namespace common_variables { namespace direct_hits {

//##############################################################################
namespace details {

//______________________________________________________________________________
/** A structure to store temporary data about the calculation of a particular
 *  class of direct hits.
 */
struct DirectHitsClassData_t
{
    /** The definition of the class of the direct hits.
     */
    I3DirectHitsDefinition dhDefinition;

    /** The generated values for a particular direct hits time window.
     */
    hm::details::NHitStringsIterativeCalculator nDirStringsIterCalc;
    uint32_t                                    nDirDoms;
    uint64_t                                    nDirPulses;
    double                                      qDirPulses;
    hm::details::NHitStringsIterativeCalculator nEarlyStringsIterCalc;
    uint32_t                                    nEarlyDoms;
    uint64_t                                    nEarlyPulses;
    double                                      qEarlyPulses;
    hm::details::NHitStringsIterativeCalculator nLateStringsIterCalc;
    uint32_t                                    nLateDoms;
    uint64_t                                    nLatePulses;
    double                                      qLatePulses;

    /** The flags if the current processed DOM has early, direct, or late pulses
     *  for this particular direct hits time window.
     */
    bool currentDomGotEarlyPulses;
    bool currentDomGotDirectPulses;
    bool currentDomGotLatePulses;

    /** The vector with all direct hit distances along the track of the direct
     *  hit DOMs.
     */
    std::vector<double> dirHitDistancesAlongTrack;

    //__________________________________________________________________________
    DirectHitsClassData_t(const I3DirectHitsDefinition& dhDefinition_)
      : dhDefinition(dhDefinition_),
        nDirDoms(0),
        nDirPulses(0),
        qDirPulses(0.),
        nEarlyDoms(0),
        nEarlyPulses(0),
        qEarlyPulses(0.),
        nLateDoms(0),
        nLatePulses(0),
        qLatePulses(0.),
        currentDomGotEarlyPulses(false),
        currentDomGotDirectPulses(false),
        currentDomGotLatePulses(false)
    {}
};

typedef std::vector<DirectHitsClassData_t> DirectHitsClassDataVector_t;

//______________________________________________________________________________
}/*details*/
//##############################################################################

//______________________________________________________________________________
I3DirectHitsDefinitionSeries
GetDefaultDefinitions()
{
    I3DirectHitsDefinitionSeries defaultDefinitions;

    defaultDefinitions.push_back(I3DirectHitsDefinition("A", -15.*I3Units::ns,  15.*I3Units::ns));
    defaultDefinitions.push_back(I3DirectHitsDefinition("B", -15.*I3Units::ns,  25.*I3Units::ns));
    defaultDefinitions.push_back(I3DirectHitsDefinition("C", -15.*I3Units::ns,  75.*I3Units::ns));
    defaultDefinitions.push_back(I3DirectHitsDefinition("D", -15.*I3Units::ns, 125.*I3Units::ns));

    return defaultDefinitions;
}

//______________________________________________________________________________
I3DirectHitsValuesMapPtr
CalculateDirectHits(
    const I3DirectHitsDefinitionSeries& dhDefinitions,
    const I3Geometry&                   geometry,
    const I3RecoPulseSeriesMap&         pulsesMap,
    const I3Particle&                   particle
)
{
    //--------------------------------------------------------------------------
    // Create data variables for each class of direct hits.
    details::DirectHitsClassDataVector_t dhClassDataVector;
    for(I3DirectHitsDefinitionSeries::const_iterator dhDefinitionsCIter = dhDefinitions.begin();
        dhDefinitionsCIter != dhDefinitions.end();
        ++dhDefinitionsCIter
       )
    {
        const I3DirectHitsDefinition& dhDefinition = *dhDefinitionsCIter;

        details::DirectHitsClassData_t dhClassData(dhDefinition);
        dhClassDataVector.push_back(dhClassData);
    }

    //--------------------------------------------------------------------------
    // Iterate over every hit DOM.
    for(I3RecoPulseSeriesMap::const_iterator pulsesMapCIter = pulsesMap.begin();
        pulsesMapCIter != pulsesMap.end();
        ++pulsesMapCIter
       )
    {
        const OMKey&             omKey  = pulsesMapCIter->first;
        const I3RecoPulseSeries& pulses = pulsesMapCIter->second;

        // Check if the hit DOM matches with the current geometry.
        I3OMGeoMap::const_iterator geoMapCIter = geometry.omgeo.find(omKey);
        if(geoMapCIter == geometry.omgeo.end())
        {
            log_fatal(
                "common_variables::direct_hits::CalculateDirectHits: "
                "Didn't find the current OMKey %s in the Geometry! "
                "Did you use the wrong Geometry?",
                omKey.str().c_str()
            );
        }

        // Get the position of the DOM.
        const I3Position& omPos = geoMapCIter->second.position;

        // Define a flag if the current DOM got direct pulses from any class of
        // direct hits, so the hit distance along the track, which is needed for
        // the direct track hit distribution smoothness value, has to be
        // calculated only once and only if the DOM got a direct pulse at all.
        bool currentDomGotDirectPulses = false;

        // Iterate over the pulses of the DOM's pulse series.
        for(I3RecoPulseSeries::const_iterator pulsesCIter = pulses.begin();
            pulsesCIter != pulses.end();
            ++pulsesCIter
           )
        {
            const I3RecoPulse& pulse = *pulsesCIter;

            double timeRes = I3Calculator::TimeResidual(particle, omPos, pulse.GetTime());

            // Count the direct hits variables for all classes of direct hits.
            for(details::DirectHitsClassDataVector_t::iterator dhClassDataVectorIter = dhClassDataVector.begin();
                dhClassDataVectorIter != dhClassDataVector.end();
                ++dhClassDataVectorIter
               )
            {
                details::DirectHitsClassData_t& dhClassData = *dhClassDataVectorIter;

                const I3DirectHitsTimeWindow& tw = dhClassData.dhDefinition.GetTimeWindow();
                if(tw.IsTimeBeforeTimeWindow(timeRes))
                {
                    dhClassData.currentDomGotEarlyPulses = true;

                    dhClassData.nEarlyPulses += 1;
                    dhClassData.qEarlyPulses += pulse.GetCharge();
                }
                else if(tw.IsTimeInsideTimeWindow(timeRes))
                {
                    currentDomGotDirectPulses = true;
                    dhClassData.currentDomGotDirectPulses = true;

                    dhClassData.nDirPulses += 1;
                    dhClassData.qDirPulses += pulse.GetCharge();
                }
                else if(tw.IsTimeAfterTimeWindow(timeRes))
                {
                    dhClassData.currentDomGotLatePulses = true;

                    dhClassData.nLatePulses += 1;
                    dhClassData.qLatePulses += pulse.GetCharge();
                }
            }//END FOR dhClassDataVector
        }//END FOR I3RecoPulseSeries

        // Calculate the hit distance along the track, if this DOM got direct
        // pulses.
        double hitDistAlongTrack = NAN;
        if(currentDomGotDirectPulses)
            hitDistAlongTrack = I3Calculator::DistanceAlongTrack(particle, omPos);

        // Count NDirDoms, NEarlyDoms, and NLateDoms.
        // Also record the hit distance along the track for each class of direct
        // hits, which got direct hits from this current DOM.
        for(details::DirectHitsClassDataVector_t::iterator dhClassDataVectorIter = dhClassDataVector.begin();
            dhClassDataVectorIter != dhClassDataVector.end();
            ++dhClassDataVectorIter
           )
        {
            details::DirectHitsClassData_t& dhClassData = *dhClassDataVectorIter;

            if(dhClassData.currentDomGotEarlyPulses)
            {
                dhClassData.currentDomGotEarlyPulses = false;
                dhClassData.nEarlyStringsIterCalc.DoNextIteration(omKey.GetString());
                dhClassData.nEarlyDoms += 1;
            }
            if(dhClassData.currentDomGotDirectPulses)
            {
                dhClassData.currentDomGotDirectPulses = false;
                dhClassData.nDirStringsIterCalc.DoNextIteration(omKey.GetString());
                dhClassData.nDirDoms += 1;
                dhClassData.dirHitDistancesAlongTrack.push_back(hitDistAlongTrack);
            }
            if(dhClassData.currentDomGotLatePulses)
            {
                dhClassData.currentDomGotLatePulses = false;
                dhClassData.nLateStringsIterCalc.DoNextIteration(omKey.GetString());
                dhClassData.nLateDoms += 1;
            }
        }//END FOR dhClassDataVector
    }//END FOR I3RecoPulseSeriesMap

    // Iterate through all the direct hit classes and put the output into an
    // I3DirectHitsValuesMap object.
    I3DirectHitsValuesMapPtr dhValuesMapPtr(new I3DirectHitsValuesMap);
    for(details::DirectHitsClassDataVector_t::iterator dhClassDataVectorIter = dhClassDataVector.begin();
        dhClassDataVectorIter != dhClassDataVector.end();
        ++dhClassDataVectorIter
       )
    {
        details::DirectHitsClassData_t& dhClassData = *dhClassDataVectorIter;

        // Finalize the calculation of the number of hit strings.
        dhClassData.nDirStringsIterCalc.FinalizeCalculation();
        dhClassData.nEarlyStringsIterCalc.FinalizeCalculation();
        dhClassData.nLateStringsIterCalc.FinalizeCalculation();

        // Calculate the direct track length and the direct track hit
        // distribution smoothness value.
        double dirTrackLength = NAN;
        double dirSmoothness  = NAN;
        // The direct track length is only defined if there are more than 2
        // direct DOM hits, and the direct smoothness value is anyway only
        // defined for at least 3 direct hit DOMs.
        if(dhClassData.dirHitDistancesAlongTrack.size() >= 2)
        {
            sort(dhClassData.dirHitDistancesAlongTrack.begin(),
                 dhClassData.dirHitDistancesAlongTrack.end()
            );
            dirTrackLength = dhClassData.dirHitDistancesAlongTrack.back() -
                             dhClassData.dirHitDistancesAlongTrack.front();
            dirSmoothness  = tc::details::CalculateTrackHitsDistributionSmoothness(
                dhClassData.dirHitDistancesAlongTrack
            );
        }

        I3DirectHitsValues dhValues(
            dhClassData.nDirStringsIterCalc.AsUInt32(),
            dhClassData.nDirDoms,
            dhClassData.nDirPulses,
            dhClassData.qDirPulses,
            dhClassData.nEarlyStringsIterCalc.AsUInt32(),
            dhClassData.nEarlyDoms,
            dhClassData.nEarlyPulses,
            dhClassData.qEarlyPulses,
            dhClassData.nLateStringsIterCalc.AsUInt32(),
            dhClassData.nLateDoms,
            dhClassData.nLatePulses,
            dhClassData.qLatePulses,
            dirTrackLength,
            dirSmoothness
        );
        std::pair<I3DirectHitsValuesMap::iterator, bool> ret =
        (*dhValuesMapPtr).insert(std::pair<std::string, I3DirectHitsValues>(
            dhClassData.dhDefinition.GetName(), dhValues
        ));
        if(ret.second == false)
        {
            log_fatal(
                "common_variables::direct_hits::CalculateDirectHits: "
                "Could not insert direct hits values into "
                "I3DirectHitsValuesMap using key \"%s\"!",
                dhClassData.dhDefinition.GetName().c_str()
            );
        }
    }

    return dhValuesMapPtr;
}

//______________________________________________________________________________
I3RecoPulseSeriesMapPtr GetDirectHitsPulseMap(
    const I3DirectHitsDefinition dhDefinition,
    const I3Geometry&            geometry,
    const I3RecoPulseSeriesMap&  pulsesMap,
    const I3Particle&            particle,
    bool                         allPulsesOfDirectDoms
)
{
    I3RecoPulseSeriesMapPtr outputPulseMapPtr(new I3RecoPulseSeriesMap);
    I3RecoPulseSeriesMap &outputPulseMap = *outputPulseMapPtr;

    for(I3RecoPulseSeriesMap::const_iterator pulsesMapCIter = pulsesMap.begin();
        pulsesMapCIter != pulsesMap.end();
        ++pulsesMapCIter
       )
    {
        const OMKey&             omKey  = pulsesMapCIter->first;
        const I3RecoPulseSeries& pulses = pulsesMapCIter->second;

        // Check if the hit DOM matches with the current geometry.
        I3OMGeoMap::const_iterator geoMapCIter = geometry.omgeo.find(omKey);
        if(geoMapCIter == geometry.omgeo.end())
        {
            log_fatal(
                "common_variables::direct_hits::GetDirectHitsPulseMap: "
                "Didn't find the current OMKey %s in the Geometry! "
                "Did you use the wrong Geometry?",
                omKey.str().c_str()
            );
        }

        // Get the position of the DOM.
        const I3Position& omPos = geoMapCIter->second.position;

        I3RecoPulseSeries newPulseSeries;

        // Iterate over the pulses of the DOM's pulse series.
        for(I3RecoPulseSeries::const_iterator pulsesCIter = pulses.begin();
            pulsesCIter != pulses.end();
            ++pulsesCIter
           )
        {
            const I3RecoPulse& pulse = *pulsesCIter;

            double timeRes = I3Calculator::TimeResidual(particle, omPos, pulse.GetTime());

            const I3DirectHitsTimeWindow& tw = dhDefinition.GetTimeWindow();
            // If the pulse is inside the direct hits time window, it's placed
            // into the new I3RecoPulseSeriesMap object.
            if(tw.IsTimeInsideTimeWindow(timeRes))
            {
                if(allPulsesOfDirectDoms)
                {
                    newPulseSeries = pulses;
                    break;
                }
                else
                {
                    newPulseSeries.push_back(pulse);
                }
            }
        }//END FOR I3RecoPulseSeries

        if(newPulseSeries.size() > 0)
            outputPulseMap[omKey] = newPulseSeries;
    }//END FOR I3RecoPulseSeriesMap

    return outputPulseMapPtr;
}

//______________________________________________________________________________
I3VectorDoublePtr
GetDirectPulsesTimeResiduals(
    const I3DirectHitsDefinition dhDefinition,
    const I3Geometry&            geometry,
    const I3RecoPulseSeriesMap&  pulsesMap,
    const I3Particle&            particle
)
{
    I3VectorDoublePtr outputTimeResidualsVectorPtr(new I3VectorDouble);
    I3VectorDouble &outputTimeResidualsVector = *outputTimeResidualsVectorPtr;

    for(I3RecoPulseSeriesMap::const_iterator pulsesMapCIter = pulsesMap.begin();
        pulsesMapCIter != pulsesMap.end();
        ++pulsesMapCIter
       )
    {
        const OMKey&             omKey  = pulsesMapCIter->first;
        const I3RecoPulseSeries& pulses = pulsesMapCIter->second;

        // Check if the hit DOM matches with the current geometry.
        I3OMGeoMap::const_iterator geoMapCIter = geometry.omgeo.find(omKey);
        if(geoMapCIter == geometry.omgeo.end())
        {
            log_fatal(
                "common_variables::direct_hits::GetDirectPulsesTimeResiduals: "
                "Didn't find the current OMKey %s in the Geometry! "
                "Did you use the wrong Geometry?",
                omKey.str().c_str()
            );
        }

        // Get the position of the DOM.
        const I3Position& omPos = geoMapCIter->second.position;

        I3RecoPulseSeries newPulseSeries;

        // Iterate over the pulses of the DOM's pulse series.
        for(I3RecoPulseSeries::const_iterator pulsesCIter = pulses.begin();
            pulsesCIter != pulses.end();
            ++pulsesCIter
           )
        {
            const I3RecoPulse& pulse = *pulsesCIter;

            double timeRes = I3Calculator::TimeResidual(particle, omPos, pulse.GetTime());

            const I3DirectHitsTimeWindow& tw = dhDefinition.GetTimeWindow();
            // If the pulse is inside the direct hits time window, its time
            // residual is placed into the output vector.
            if(tw.IsTimeInsideTimeWindow(timeRes))
            {
                outputTimeResidualsVector.push_back(timeRes);
            }
        }//END FOR I3RecoPulseSeries
    }//END FOR I3RecoPulseSeriesMap

    return outputTimeResidualsVectorPtr;
}

//______________________________________________________________________________
}/*direct_hits*/}/*common_variables*/
//##############################################################################
