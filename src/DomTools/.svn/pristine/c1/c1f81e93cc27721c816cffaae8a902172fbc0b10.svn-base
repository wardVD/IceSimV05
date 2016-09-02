/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id$
 *
 * @file Utility.h
 * @version $Revision:$
 * @date $Date$
 */

#ifndef DOMTOOLS_UTILITY_H_INCLUDED
#define DOMTOOLS_UTILITY_H_INCLUDED

#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3MCHit.h>
#include <icetray/I3Units.h>
#include <cmath>

namespace domtools
{
  inline double GetTime(const I3RecoHit& hit)
    {
      return hit.GetTime();
    }

  inline double GetTime(const I3RecoPulse& pulse)
    {
      return pulse.GetTime();
    }

  inline double GetTime(const I3DOMLaunch& launch)
    {
      return launch.GetStartTime();
    }
    
  inline double GetTime(const I3MCHit& hit)
    {
      return hit.GetTime();
    }

  inline double GetWidth(const I3RecoHit& hit)
    {
      return 0;
    }

  inline double GetWidth(const I3MCHit& hit)
    {
      return 0;
    }

  inline double GetWidth(const I3RecoPulse& pulse)
    {
      return pulse.GetWidth();
    }

  inline double GetWidth(const I3DOMLaunch& launch)
    {
      double atwd0 = launch.GetRawATWD(0).size()*3.*I3Units::ns;
      double atwd1 = launch.GetRawATWD(1).size()*3.*I3Units::ns;
      double atwd2 = launch.GetRawATWD(2).size()*3.*I3Units::ns;
      double fadc  = launch.GetRawFADC().size()*25.*I3Units::ns;

      double max_width=NAN;
      if(std::isnan(max_width) || atwd0>max_width) max_width=atwd0; 
      if(std::isnan(max_width) || atwd1>max_width) max_width=atwd1; 
      if(std::isnan(max_width) || atwd2>max_width) max_width=atwd2; 
      if(std::isnan(max_width) || fadc>max_width)  max_width=fadc; 

      return max_width;
    }
    
  inline double GetAmplitude(const I3RecoHit& hit)
    {
      return NAN;
    }
  
  inline double GetAmplitude(const I3MCHit& hit)
    {
      return NAN;
    }

  inline double GetAmplitude(const I3RecoPulse& pulse)
    {
      return pulse.GetCharge();
    }

  inline double GetAmplitude(const I3DOMLaunch& launch)
    {
      return NAN;
    }

};
#endif
