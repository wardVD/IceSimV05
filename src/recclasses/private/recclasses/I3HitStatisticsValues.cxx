/**
 * $Id: I3HitStatisticsValues.cxx 143026 2016-03-11 15:28:50Z nega $
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file I3HitStatisticsValues.cxx
 * @version $Revision: 143026 $
 * @date $Date: 2016-03-11 09:28:50 -0600 (Fri, 11 Mar 2016) $
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the implementation of the I3HitStatisticsValues
 *        class, which is an I3FrameObject holding the result values of hit
 *        statistics calculation.
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
 */
#include <iostream>

#include "icetray/serialization.h"
#include "icetray/I3Units.h"

#include "recclasses/I3HitStatisticsValues.h"

//______________________________________________________________________________
template <class Archive>
void
I3HitStatisticsValues::
serialize(Archive& ar, unsigned version)
{
    if(version > i3hitstatisticsvalues_version_)
        log_fatal("Attempting to read version %u from file but running version "
                  "%u of I3HitStatisticsValues class.",
                  version, i3hitstatisticsvalues_version_
        );

    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("COG",           this->cog_);
    ar & make_nvp("COGZSigma",     this->cogZSigma_);
    ar & make_nvp("MinPulseTime",  this->minPulseTime_);
    ar & make_nvp("MaxPulseTime",  this->maxPulseTime_);
    ar & make_nvp("QMaxDoms",      this->qMaxDoms_);
    ar & make_nvp("QTotPulses",    this->qTotPulses_);
    ar & make_nvp("ZMin",          this->zMin_);
    ar & make_nvp("ZMax",          this->zMax_);
    ar & make_nvp("ZMean",         this->zMean_);
    ar & make_nvp("ZSigma",        this->zSigma_);
    ar & make_nvp("ZTravel",       this->zTravel_);
}

I3_SERIALIZABLE(I3HitStatisticsValues);

//______________________________________________________________________________
std::ostream&
operator<<(std::ostream& oss, const I3HitStatisticsValues& rhs)
{
    oss << "I3HitStatisticsValues("                                          <<
        "COG: "                << rhs.GetCOG()                       << ", " <<
        "COGZSigma [m]: "      << rhs.GetCOGZSigma()/I3Units::m      << ", " <<
        "MinPulseTime [ns]: "  << rhs.GetMinPulseTime()/I3Units::ns  << ", " <<
        "MaxPulseTime [ns]: "  << rhs.GetMaxPulseTime()/I3Units::ns  << ", " <<
        "QMaxDoms [PE]: "      << rhs.GetQMaxDoms()                  << ", " <<
        "QTotPulses [PE]: "    << rhs.GetQTotPulses()                << ", " <<
        "ZMin [m]: "           << rhs.GetZMin()/I3Units::m           << ", " <<
        "ZMax [m]: "           << rhs.GetZMax()/I3Units::m           << ", " <<
        "ZMean [m]: "          << rhs.GetZMean()/I3Units::m          << ", " <<
        "ZSigma [m]: "         << rhs.GetZSigma()/I3Units::m         << ", " <<
        "ZTravel [m]: "        << rhs.GetZTravel()/I3Units::m        <<
        ")";
    return oss;
}