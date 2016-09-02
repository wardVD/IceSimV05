/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file hit_statistics/calculator.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the implementation of the calculator functions that
 *        are defined inside the common_variables::hit_statistics
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

#include "CommonVariables/hit_statistics/ArithMeanIterativeCalculator.h"
#include "CommonVariables/hit_statistics/COGIterativeCalculator.h"
#include "CommonVariables/hit_statistics/SigmaIterativeCalculator.h"
#include "CommonVariables/hit_statistics/ZTravelIterativeCalculator.h"

#include "CommonVariables/hit_statistics/calculator.h"

//##############################################################################
namespace common_variables { namespace hit_statistics {

//##############################################################################
// Define namespace for details implementations. This is only for calculation
// efficiency reasons to avoid the looping over the I3RecoPulseSeriesMap several
// times.
namespace details {

//______________________________________________________________________________
struct S1Variables_t
{
    I3Position cog;
    double     cogZSigma;
    double     minPulseTime;
    double     maxPulseTime;
    double     qMaxDoms;
    double     qTotPulses;
    double     zMin;
    double     zMax;
    double     zMean;
    double     zSigma;
    double     zTravel;

    //__________________________________________________________________________
    S1Variables_t(
        const I3Position& cog_,
        double            cogZSigma_,
        double            minPulseTime_,
        double            maxPulseTime_,
        double            qMaxDoms_,
        double            qTotPulses_,
        double            zMin_,
        double            zMax_,
        double            zMean_,
        double            zSigma_,
        double            zTravel_
    )
      : cog(cog_),
        cogZSigma(cogZSigma_),
        minPulseTime(minPulseTime_),
        maxPulseTime(maxPulseTime_),
        qMaxDoms(qMaxDoms_),
        qTotPulses(qTotPulses_),
        zMin(zMin_),
        zMax(zMax_),
        zMean(zMean_),
        zSigma(zSigma_),
        zTravel(zTravel_)
    {}

    //__________________________________________________________________________
    S1Variables_t(const S1Variables_t& rhs)
      : cog(rhs.cog),
        cogZSigma(rhs.cogZSigma),
        minPulseTime(rhs.minPulseTime),
        maxPulseTime(rhs.maxPulseTime),
        qMaxDoms(rhs.qMaxDoms),
        qTotPulses(rhs.qTotPulses),
        zMin(rhs.zMin),
        zMax(rhs.zMax),
        zMean(rhs.zMean),
        zSigma(rhs.zSigma),
        zTravel(rhs.zTravel)
    {}
};

//______________________________________________________________________________
S1Variables_t
CalculateS1Variables(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    double qMaxDoms     = NAN;
    double qTotPulses   = NAN;
    double zMin         = NAN;
    double zMax         = NAN;
    double minPulseTime = NAN;
    double maxPulseTime = NAN;

    COGIterativeCalculator       cogIterCalc;
    SigmaIterativeCalculator     cogZSigmaIterCalc;
    ArithMeanIterativeCalculator zMeanIterCalc;
    SigmaIterativeCalculator     zSigmaIterCalc;
    ZTravelIterativeCalculator   zTravelIterCalc;

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
                "common_variables::hit_statistics::details::CalculateS1Variables: "
                "Didn't find the current OMKey %s in the Geometry! "
                "Did you use the wrong Geometry?",
                omKey.str().c_str()
            );
        }
        const I3Position& omPos = geoMapCIter->second.position;

        // Sanity check: Check if there are pulses at all.
        if(pulses.size() == 0)
        {
            log_error(
                "common_variables::hit_statistics::details::CalculateS1Variables: "
                "The OMKey %s does not have pulses at all! "
                "Skipping this DOM!",
                omKey.str().c_str()
            );
            continue;
        }

        // Calculate ZMin.
        if(std::isnan(zMin) || omPos.GetZ() < zMin)
            zMin = omPos.GetZ();

        // Calculate ZMax.
        if(std::isnan(zMax) || omPos.GetZ() > zMax)
            zMax = omPos.GetZ();

        // Calculate ZMean.
        zMeanIterCalc.DoNextIteration(omPos.GetZ());

        // Calculate ZSigma.
        zSigmaIterCalc.DoNextIteration(omPos.GetZ(), 1.);

        // Define helper variable for calculating qMaxDoms.
        double qDomPulses = 0.;

        // Get the time of the first pulse and count it for the ZTravel
        // calculation.
        zTravelIterCalc.DoNextIteration(pulses[0].GetTime(), omPos.GetZ());

        // Iterate over the pulses of the DOM's pulse series.
        for(I3RecoPulseSeries::const_iterator pulsesCIter = pulses.begin();
            pulsesCIter != pulses.end();
            ++pulsesCIter
           )
        {
            const I3RecoPulse& pulse = *pulsesCIter;

            // Get the charge of the pulse and count it for the COG calculation
            // if the charge has a physical value.
            double pulseCharge = pulse.GetCharge();
            if(! (std::isnan(pulseCharge) ||
                  std::isinf(pulseCharge)
                 )
              )
            {
                // Calculate minPulseTime.
                if(std::isnan(minPulseTime) || pulse.GetTime() < minPulseTime)
                    minPulseTime = pulse.GetTime();

                // Calculate maxPulseTime.
                if(std::isnan(maxPulseTime) || pulse.GetTime() > maxPulseTime)
                    maxPulseTime = pulse.GetTime();

                qDomPulses += pulseCharge;

                // Calculate qTotPulses.
                if(std::isnan(qTotPulses))
                    qTotPulses = pulseCharge;
                else
                    qTotPulses += pulseCharge;

                // Calculate COG.
                cogIterCalc.DoNextIteration(
                    omPos.GetX(), omPos.GetY(), omPos.GetZ(),
                    pulseCharge
                );

                // Calculate COGZSigma.
                cogZSigmaIterCalc.DoNextIteration(omPos.GetZ(), pulseCharge);
            }
        }//END FOR I3RecoPulseSeries

        // Calculate qMaxDoms.
        if(std::isnan(qMaxDoms) || qDomPulses > qMaxDoms)
            qMaxDoms = qDomPulses;
    }//END FOR I3RecoPulseSeriesMap

    //--------------------------------------------------------------------------
    // Finalize the calculation of COG.
    cogIterCalc.FinalizeCalculation();

    //--------------------------------------------------------------------------
    // Finalize the calculation of COGZSigma.
    cogZSigmaIterCalc.FinalizeCalculation();

    //--------------------------------------------------------------------------
    // Finalize the calculation of ZMean.
    zMeanIterCalc.FinalizeCalculation();

    //--------------------------------------------------------------------------
    // Finalize the calculation of ZSigma.
    zSigmaIterCalc.FinalizeCalculation();

    //--------------------------------------------------------------------------
    // Finalize the calculation of ZTravel.
    zTravelIterCalc.FinalizeCalculation();

    S1Variables_t vars(
        cogIterCalc.AsI3Position(),
        cogZSigmaIterCalc.AsDouble(),
        minPulseTime,
        maxPulseTime,
        qMaxDoms,
        qTotPulses,
        zMin,
        zMax,
        zMeanIterCalc.AsDouble(),
        zSigmaIterCalc.AsDouble(),
        zTravelIterCalc.AsDouble()
    );
    return vars;
}

}/*details*/
//##############################################################################

//______________________________________________________________________________
I3HitStatisticsValuesPtr
CalculateHitStatistics(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    //--------------------------------------------------------------------------
    // Calculate all stage 1 variables.
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    //--------------------------------------------------------------------------
    // Construct the I3HitStatisticsValues output I3FrameObject.
    I3HitStatisticsValuesPtr hsValuesPtr(new I3HitStatisticsValues(
        s1Vars.cog,
        s1Vars.cogZSigma,
        s1Vars.minPulseTime,
        s1Vars.maxPulseTime,
        s1Vars.qMaxDoms,
        s1Vars.qTotPulses,
        s1Vars.zMin,
        s1Vars.zMax,
        s1Vars.zMean,
        s1Vars.zSigma,
        s1Vars.zTravel
    ));

    return hsValuesPtr;
}

//______________________________________________________________________________
I3PositionPtr
CalculateCOG(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3PositionPtr cogI3PositionPtr(new I3Position(s1Vars.cog));
    return cogI3PositionPtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateCOGZSigma(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr cogZSigmaI3DoublePtr(new I3Double(s1Vars.cogZSigma));
    return cogZSigmaI3DoublePtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateMinPulseTime(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr minPulseTimeI3DoublePtr(new I3Double(s1Vars.minPulseTime));
    return minPulseTimeI3DoublePtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateMaxPulseTime(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr maxPulseTimeI3DoublePtr(new I3Double(s1Vars.maxPulseTime));
    return maxPulseTimeI3DoublePtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateQMaxDoms(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr qMaxDomsI3DoublePtr(new I3Double(s1Vars.qMaxDoms));
    return qMaxDomsI3DoublePtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateQTotPulses(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr qTotPulsesI3DoublePtr(new I3Double(s1Vars.qTotPulses));
    return qTotPulsesI3DoublePtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateZMin(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr zMinI3DoublePtr(new I3Double(s1Vars.zMin));
    return zMinI3DoublePtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateZMax(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr zMaxI3DoublePtr(new I3Double(s1Vars.zMax));
    return zMaxI3DoublePtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateZMean(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr zMeanI3DoublePtr(new I3Double(s1Vars.zMean));
    return zMeanI3DoublePtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateZSigma(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr zSigmaI3DoublePtr(new I3Double(s1Vars.zSigma));
    return zSigmaI3DoublePtr;
}

//______________________________________________________________________________
I3DoublePtr
CalculateZTravel(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::S1Variables_t s1Vars = details::CalculateS1Variables(
        geometry, pulsesMap
    );

    I3DoublePtr zTravelI3DoublePtr(new I3Double(s1Vars.zTravel));
    return zTravelI3DoublePtr;
}

//______________________________________________________________________________
}/*hit_statistics*/}/*common_variables*/
//##############################################################################
