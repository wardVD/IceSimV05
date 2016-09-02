/**
 * $Id: I3HitMultiplicityValues.h 143970 2016-03-30 16:26:58Z hdembinski $
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
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
#ifndef I3HITMULTIPLICITYVALUES_H_INCLUDED
#define I3HITMULTIPLICITYVALUES_H_INCLUDED

#include <iostream>
#include <stdint.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include "icetray/I3FrameObject.h"
#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"

static const unsigned i3hitmultiplicityvalues_version_ = 0;

/**
 * @brief Stores result values of a hit multiplicity calculation (see project CommonVariables).
 */
class I3HitMultiplicityValues : public I3FrameObject
{
  public:
    //__________________________________________________________________________
    I3HitMultiplicityValues()
      : nHitStrings_(0),
        nHitDoms_(0),
        nHitDomsOnePulse_(0),
        nPulses_(0)
    {}

    //__________________________________________________________________________
    I3HitMultiplicityValues(const I3HitMultiplicityValues& rhs)
      : nHitStrings_(rhs.GetNHitStrings()),
        nHitDoms_(rhs.GetNHitDoms()),
        nHitDomsOnePulse_(rhs.GetNHitDomsOnePulse()),
        nPulses_(rhs.GetNPulses())
    {}

    //__________________________________________________________________________
    I3HitMultiplicityValues(
        uint32_t nHitStrings,
        uint32_t nHitDoms,
        uint32_t nHitDomsOnePulse,
        uint64_t nPulses
    )
      : nHitStrings_(nHitStrings),
        nHitDoms_(nHitDoms),
        nHitDomsOnePulse_(nHitDomsOnePulse),
        nPulses_(nPulses)
    {}

    //__________________________________________________________________________
    virtual
    ~I3HitMultiplicityValues()
    {}

    //__________________________________________________________________________
    inline
    uint32_t
    GetNHitDoms() const
    { return this->nHitDoms_; }

    //__________________________________________________________________________
    inline
    uint32_t
    GetNHitDomsOnePulse() const
    { return this->nHitDomsOnePulse_; }

    //__________________________________________________________________________
    inline
    uint32_t
    GetNHitStrings() const
    { return this->nHitStrings_; }

    //__________________________________________________________________________
    inline
    uint64_t
    GetNPulses() const
    { return this->nPulses_; }

    //__________________________________________________________________________
    inline
    void
    SetNHitDoms(uint32_t n)
    { this->nHitDoms_ = n; }

    //__________________________________________________________________________
    inline
    void
    SetNHitDomsOnePulse(uint32_t n)
    { this->nHitDomsOnePulse_ = n; }

    //__________________________________________________________________________
    inline
    void
    SetNHitStrings(uint32_t n)
    { this->nHitStrings_ = n; }

    //__________________________________________________________________________
    inline
    void
    SetNPulses(uint64_t n)
    { this->nPulses_ = n; }

  protected:
    /// The number of hit strings.
    uint32_t nHitStrings_;
    /// The number of hit DOMs, historically called NChannel.
    uint32_t nHitDoms_;
    /// The number of DOMs with exactly one pulse.
    uint32_t nHitDomsOnePulse_;
    /// The number of pulses, historically called NHit.
    uint64_t nPulses_;

  private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, unsigned version);

    SET_LOGGER("I3HitMultiplicityValues")
};

//______________________________________________________________________________
std::ostream& operator<<(std::ostream& oss, const I3HitMultiplicityValues& rhs);

//______________________________________________________________________________
I3_POINTER_TYPEDEFS(I3HitMultiplicityValues);
BOOST_CLASS_VERSION(I3HitMultiplicityValues, i3hitmultiplicityvalues_version_);

//==============================================================================

#endif // I3HITMULTIPLICITYVALUES_H_INCLUDED
