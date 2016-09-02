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
 * $Id: I3CLSimPhotonHistory.cxx 108199 2013-07-12 21:33:08Z nwhitehorn $
 *
 * @file I3CLSimPhotonHistory.cxx
 * @version $Revision: 108199 $
 * @date $Date: 2013-07-12 16:33:08 -0500 (Fri, 12 Jul 2013) $
 * @author Claudio Kopper
 */

#include <icetray/serialization.h>
#include <clsim/I3CLSimPhotonHistory.h>

I3CLSimPhotonHistory::~I3CLSimPhotonHistory() { }


template <class Archive>
void I3CLSimPhotonHistory::serialize(Archive &ar, unsigned version)
{
    if (version > i3clsimphotonhistory_version_)
        log_fatal("Attempting to read version %u from file but running version %u of I3CLSimPhotonHistory class.",version,i3clsimphotonhistory_version_);
    
    ar & make_nvp("posX", posX_);
    ar & make_nvp("posY", posY_);
    ar & make_nvp("posZ", posZ_);
    
    if (version >= 1) {
        ar & make_nvp("distanceInAbsorptionLengths", distanceInAbsorptionLengths_);
    }
}     


I3_SERIALIZABLE(I3CLSimPhotonHistory);
I3_SERIALIZABLE(I3CLSimPhotonHistorySeries);
