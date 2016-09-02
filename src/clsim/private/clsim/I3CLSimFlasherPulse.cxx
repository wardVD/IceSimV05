/**
 * Copyright (c) 2011, 2012
 * Claudio Kopper <claudio.kopper@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * $Id: I3CLSimFlasherPulse.cxx 130806 2015-03-31 03:12:34Z nega $
 *
 * @file I3CLSimFlasherPulse.cxx
 * @version $Revision: 130806 $
 * @date $Date: 2015-03-30 22:12:34 -0500 (Mon, 30 Mar 2015) $
 * @author Claudio Kopper
 */

#include <clsim/I3CLSimFlasherPulse.h>

#include <stdexcept>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <cmath>



// construction & destruction
I3CLSimFlasherPulse::I3CLSimFlasherPulse()
:
flasherPulseType_(I3CLSimFlasherPulse::Unknown),
time_(NAN),
numberOfPhotonsNoBias_(NAN),
pulseWidth_(NAN),
angularEmissionSigmaPolar_(NAN),
angularEmissionSigmaAzimuthal_(NAN)
{
    
}



I3CLSimFlasherPulse::~I3CLSimFlasherPulse() 
{ 
    
}


template <class Archive>
void I3CLSimFlasherPulse::serialize(Archive &ar, unsigned version)
{
    if (version > i3clsimflasherpulse_version_)
        log_fatal("Attempting to read version %u from file but running version %u of I3CLSimFlasherPulse class.",version,i3clsimflasherpulse_version_);
    
    ar & make_nvp("flasherPulseType", flasherPulseType_);
    ar & make_nvp("dir", dir_);
    ar & make_nvp("pos", pos_);
    ar & make_nvp("time", time_);
    ar & make_nvp("numberOfPhotonsNoBias", numberOfPhotonsNoBias_);
    ar & make_nvp("pulseWidth", pulseWidth_);
    ar & make_nvp("angularEmissionSigmaPolar", angularEmissionSigmaPolar_);
    ar & make_nvp("angularEmissionSigmaAzimuthal", angularEmissionSigmaAzimuthal_);
}     

I3_SERIALIZABLE(I3CLSimFlasherPulse);
I3_SERIALIZABLE(I3CLSimFlasherPulseSeries);


// comparison
bool operator==(const I3CLSimFlasherPulse &a, const I3CLSimFlasherPulse &b)
{
    if (a.flasherPulseType_ != b.flasherPulseType_) return false;

    if (a.dir_.GetZenith() != b.dir_.GetZenith()) return false;
    if (a.dir_.GetAzimuth() != b.dir_.GetAzimuth()) return false;
    if (a.pos_.GetX() != b.pos_.GetX()) return false;
    if (a.pos_.GetY() != b.pos_.GetY()) return false;
    if (a.pos_.GetZ() != b.pos_.GetZ()) return false;
    
    if (std::isnan(a.time_) || std::isnan(b.time_)) {
        if ((!std::isnan(a.time_)) || (!std::isnan(b.time_))) return false;
    } else {
        if (a.time_ != b.time_) return false;
    }
    
    if (std::isnan(a.numberOfPhotonsNoBias_) || std::isnan(b.numberOfPhotonsNoBias_)) {
        if ((!std::isnan(a.numberOfPhotonsNoBias_)) || (!std::isnan(b.numberOfPhotonsNoBias_))) return false;
    } else {
        if (a.numberOfPhotonsNoBias_ != b.numberOfPhotonsNoBias_) return false;
    }
    
    if (std::isnan(a.pulseWidth_) || std::isnan(b.pulseWidth_)) {
        if ((!std::isnan(a.pulseWidth_)) || (!std::isnan(b.pulseWidth_))) return false;
    } else {
        if (a.pulseWidth_ != b.pulseWidth_) return false;
    }

    if (std::isnan(a.angularEmissionSigmaPolar_) || std::isnan(b.angularEmissionSigmaPolar_)) {
        if ((!std::isnan(a.angularEmissionSigmaPolar_)) || (!std::isnan(b.angularEmissionSigmaPolar_))) return false;
    } else {
        if (a.angularEmissionSigmaPolar_ != b.angularEmissionSigmaPolar_) return false;
    }

    if (std::isnan(a.angularEmissionSigmaAzimuthal_) || std::isnan(b.angularEmissionSigmaAzimuthal_)) {
        if ((!std::isnan(a.angularEmissionSigmaAzimuthal_)) || (!std::isnan(b.angularEmissionSigmaAzimuthal_))) return false;
    } else {
        if (a.angularEmissionSigmaAzimuthal_ != b.angularEmissionSigmaAzimuthal_) return false;
    }

    
    return true;
}

