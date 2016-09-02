/**
 * @author Anna Pollmann <anna.pollmann@uni-wuppertal.de>
 */

#include <cmath>
#include <vector>
#include <algorithm> // std::sort
#include <iostream>

#include "phys-services/I3Calculator.h"

#include "CommonVariables/time_characteristics/calculator.h"

//##############################################################################
namespace common_variables { namespace time_characteristics {

namespace details {

bool sortPulsesForTime(I3RecoPulse i, I3RecoPulse j){
    return (i.GetTime() < j.GetTime());
}

bool sortPairPulsesForTime(std::pair<I3RecoPulse, I3Position> i, std::pair<I3RecoPulse, I3Position> j){
    return (i.first.GetTime() < j.first.GetTime());
}

}/*details*/
//##############################################################################

//______________________________________________________________________________
I3TimeCharacteristicsValuesPtr
CalculateTimeCharacteristics(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{

    double time_sum = 0;
    double time_sum_squared = 0;
    int nFirstPulses=0;
    std::vector<I3RecoPulse> firstPulses;
    std::vector<std::pair<I3RecoPulse, I3Position> > firstPulsesPos;


    // Iterate over every hit DOM --------------------------------------------------------
    for(I3RecoPulseSeriesMap::const_iterator pulsesMapCIter = pulsesMap.begin();
        pulsesMapCIter != pulsesMap.end();
        ++pulsesMapCIter
       )
    {
        // get key and pulses from the DOM
        const OMKey&             omKey  = pulsesMapCIter->first;
        const I3RecoPulseSeries& pulses = pulsesMapCIter->second;

        // Check if the hit DOM matches with the current geometry given by the GCD file,
        //and get the position of the DOM.
        I3OMGeoMap::const_iterator geoMapCIter = geometry.omgeo.find(omKey);
        if(geoMapCIter == geometry.omgeo.end())
        {
            log_fatal(
                "common_variables::time_characteristics::CalculateTimeCharacteristics: "
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
                "common_variables::time_characteristics::CalculateTimeCharacteristics: "
                "The OMKey %s does not have pulses at all! "
                "Skipping this DOM!",
                omKey.str().c_str()
            );
            continue;
        }


        // get FIRST pulses
        I3RecoPulseSeries::const_iterator it = pulses.begin();
        // get times
        double time_first_pulse = it->GetTime();
        time_sum+=time_first_pulse;
        time_sum_squared+=time_first_pulse*time_first_pulse;
        // get first pulses per dom
        firstPulses.push_back(*it);
        nFirstPulses++;
        // get first pulses per dom and its positions
        firstPulsesPos.push_back(std::make_pair(*it, omPos));

    }

    // sort vectors of pulses
    std::sort(firstPulses.begin(), firstPulses.end(), details::sortPulsesForTime);
    std::sort(firstPulsesPos.begin(), firstPulsesPos.end(), details::sortPairPulsesForTime);

    // calc variables --------------------------------------------------------------------


    double timelength_fwhm=-1;
    int timelength_last_first=-1;
    int timelength_maxgap=-1; // int
    int zpattern=0; // int

    if (nFirstPulses<1){

        log_error(
                "common_variables::time_characteristics::CalculateTimeCharacteristics: "
                "Not enough pulses at the DOMs! Just found %d Pulses.",
                nFirstPulses
            );

    }
    else {

        // calculate gaussian fit
        double std_deviation_times = sqrt((1.0/nFirstPulses) * (time_sum_squared - (1.0/nFirstPulses*(time_sum*time_sum)))); // calculate standard deviation with http://en.wikipedia.org/wiki/Standard_deviation#Identities_and_mathematical_properties
        timelength_fwhm = 2.0*sqrt(2.0*log(2.0)) * std_deviation_times;

        // calculate last pulse time minus first pulse time
        timelength_last_first = firstPulses.back().GetTime() - firstPulses.front().GetTime();

        // loop only over First pulses
        timelength_maxgap=0;
        double last_time=0;

        int counter =0;
        for(std::vector<I3RecoPulse>::iterator it=firstPulses.begin();
            it!=firstPulses.end();
            ++it)
            {
            int t = (*it).GetTime();

            // max-gap
            if (((t - last_time) > timelength_maxgap) && (counter!=0) ){
                timelength_maxgap = t - last_time;
            }
            last_time=t;

            counter++;
        }

        //---
        // loop over first pulses using the positions
        double lastZ=0;
        zpattern=0;

        for (std::vector<std::pair<I3RecoPulse, I3Position> >::iterator it = firstPulsesPos.begin();
             it!=firstPulsesPos.end();
             ++it)
            {

            double z = (*it).second.GetZ();
            int t = (*it).first.GetTime();

            if ((z>lastZ) && (counter!=0)){
                zpattern++;
            }
            else if ((z<lastZ) && (counter!=0)){
                zpattern--;
            }

            lastZ=z;
        }

    }

    // ###################################################################################
    // Finalize

    //--------------------------------------------------------------------------
    // Construct the I3TimeCharacteristicsValues frame object with all
    // calculated values.
    I3TimeCharacteristicsValuesPtr i3TimeCharacteristicsValuesPtr(new I3TimeCharacteristicsValues(
        timelength_fwhm,
        timelength_last_first,
        timelength_maxgap,
        zpattern

    ));

    return i3TimeCharacteristicsValuesPtr;

}

//______________________________________________________________________________
}/*monopoles*/}/*common_variables*/
//##############################################################################
