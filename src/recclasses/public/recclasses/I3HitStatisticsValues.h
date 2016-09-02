/**
 * $Id: I3HitStatisticsValues.h 143970 2016-03-30 16:26:58Z hdembinski $
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file I3HitStatisticsValues.h
 * @version $Revision: 143970 $
 * @date $Date: 2016-03-30 11:26:58 -0500 (Wed, 30 Mar 2016) $
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#ifndef I3HITSTATISTICSVALUES_H_INCLUDED
#define I3HITSTATISTICSVALUES_H_INCLUDED

#include <iostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include "icetray/I3FrameObject.h"
#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"

#include "dataclasses/I3Position.h"

static const unsigned i3hitstatisticsvalues_version_ = 0;

/**
 * @brief Stores result of a hit statistics calculation (see project CommonVariables).
 */
class I3HitStatisticsValues : public I3FrameObject
{
  public:
    //__________________________________________________________________________
    I3HitStatisticsValues()
      : cog_(I3Position(NAN, NAN, NAN, I3Position::car)),
        cogZSigma_(NAN),
        minPulseTime_(NAN),
        maxPulseTime_(NAN),
        qMaxDoms_(NAN),
        qTotPulses_(NAN),
        zMin_(NAN),
        zMax_(NAN),
        zMean_(NAN),
        zSigma_(NAN),
        zTravel_(NAN)
    {}

    //__________________________________________________________________________
    I3HitStatisticsValues(const I3HitStatisticsValues& rhs)
      : cog_(rhs.GetCOG()),
        cogZSigma_(rhs.GetCOGZSigma()),
        minPulseTime_(rhs.GetMinPulseTime()),
        maxPulseTime_(rhs.GetMaxPulseTime()),
        qMaxDoms_(rhs.GetQMaxDoms()),
        qTotPulses_(rhs.GetQTotPulses()),
        zMin_(rhs.GetZMin()),
        zMax_(rhs.GetZMax()),
        zMean_(rhs.GetZMean()),
        zSigma_(rhs.GetZSigma()),
        zTravel_(rhs.GetZTravel())
    {}

    //__________________________________________________________________________
    I3HitStatisticsValues(
        const I3Position& cog,
        double            cogZSigma,
        double            minPulseTime,
        double            maxPulseTime,
        double            qMaxDoms,
        double            qTotPulses,
        double            zMin,
        double            zMax,
        double            zMean,
        double            zSigma,
        double            zTravel
    )
      : cog_(cog),
        cogZSigma_(cogZSigma),
        minPulseTime_(minPulseTime),
        maxPulseTime_(maxPulseTime),
        qMaxDoms_(qMaxDoms),
        qTotPulses_(qTotPulses),
        zMin_(zMin),
        zMax_(zMax),
        zMean_(zMean),
        zSigma_(zSigma),
        zTravel_(zTravel)
    {}

    //__________________________________________________________________________
    inline
    const I3Position&
    GetCOG() const
    { return this->cog_; }

    //__________________________________________________________________________
    inline
    I3Position&
    GetCOG()
    { return this->cog_; }

    //__________________________________________________________________________
    inline
    double
    GetCOGZSigma() const
    { return this->cogZSigma_; }

    //__________________________________________________________________________
    inline
    double
    GetMaxPulseTime() const
    { return this->maxPulseTime_; }

    //__________________________________________________________________________
    inline
    double
    GetMinPulseTime() const
    { return this->minPulseTime_; }

    //__________________________________________________________________________
    inline
    double
    GetQMaxDoms() const
    { return this->qMaxDoms_; }

    //__________________________________________________________________________
    inline
    double
    GetQTotPulses() const
    { return this->qTotPulses_; }

    //__________________________________________________________________________
    inline
    double
    GetZMax() const
    { return this->zMax_; }

    //__________________________________________________________________________
    inline
    double
    GetZMean() const
    { return this->zMean_; }

    //__________________________________________________________________________
    inline
    double
    GetZMin() const
    { return this->zMin_; }

    //__________________________________________________________________________
    inline
    double
    GetZSigma() const
    { return this->zSigma_; }

    //__________________________________________________________________________
    inline
    double
    GetZTravel() const
    { return this->zTravel_; }

    //__________________________________________________________________________
    inline
    void
    SetCOG(const I3Position& cog)
    { this->cog_ = cog; }

    //__________________________________________________________________________
    inline
    void
    SetCOGZSigma(double s)
    { this->cogZSigma_ = s; }

    //__________________________________________________________________________
    inline
    void
    SetMaxPulseTime(double t)
    { this->maxPulseTime_ = t; }

    //__________________________________________________________________________
    inline
    void
    SetMinPulseTime(double t)
    { this->minPulseTime_ = t; }

    //__________________________________________________________________________
    inline
    void
    SetQMaxDoms(double q)
    { this->qMaxDoms_ = q; }

    //__________________________________________________________________________
    inline
    void
    SetQTotPulses(double q)
    { this->qTotPulses_ = q; }

    //__________________________________________________________________________
    inline
    void
    SetZMax(double z)
    { this->zMax_ = z; }

    //__________________________________________________________________________
    inline
    void
    SetZMean(double z)
    { this->zMean_ = z; }

    //__________________________________________________________________________
    inline
    void
    SetZMin(double z)
    { this->zMin_ = z; }

    //__________________________________________________________________________
    inline
    void
    SetZSigma(double s)
    { this->zSigma_ = s; }

    //__________________________________________________________________________
    inline
    void
    SetZTravel(double z)
    { this->zTravel_ = z; }

  protected:
    /// The position of the Center-Of-Gravity (COG) of the event.
    I3Position cog_;
    /// The sigma value of the z-component of the COG position.
    double cogZSigma_;
    /// The minimal time of all pulses, e.g. the time of the first pulse.
    double minPulseTime_;
    /// The maximal time of all pulses, e.g. the time of the last pulse.
    double maxPulseTime_;
    /// The maximal value of all DOM pulse charge sums.
    double qMaxDoms_;
    /// The total charge of all pulses.
    double qTotPulses_;
    /// The minimal OM_Z value of all hits.
    double zMin_;
    /// The maximal OM_Z value of all hits.
    double zMax_;
    /// The mean of the OM_Z values.
    double zMean_;
    /// The sigma (RMSD) of the OM_Z values.
    double zSigma_;
    /// The average over (OM_Z minus the average over the OM_Z values of the
    /// first quartile of all pulses), thus an OM_Z value.
    double zTravel_;

  private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, unsigned version);

    SET_LOGGER("I3HitStatisticsValues")
};

//______________________________________________________________________________
std::ostream& operator<<(std::ostream& oss, const I3HitStatisticsValues& rhs);

//______________________________________________________________________________
I3_POINTER_TYPEDEFS(I3HitStatisticsValues);
BOOST_CLASS_VERSION(I3HitStatisticsValues, i3hitstatisticsvalues_version_);

//==============================================================================

#endif // I3HITSTATISTICSVALUES_H_INCLUDED
