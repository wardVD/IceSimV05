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
 * $Id: I3Converters.cxx 108199 2013-07-12 21:33:08Z nwhitehorn $
 *
 * @file I3Converters.cxx
 * @version $Revision: 108199 $
 * @date $Date: 2013-07-12 16:33:08 -0500 (Fri, 12 Jul 2013) $
 * @author Claudio Kopper
 */

#include <sstream>

#include "clsim/converter/I3PhotonConverter.h"
#include "clsim/converter/I3MCHitConverterWithIDs.h"
#include "clsim/converter/I3MCPEConverterWithIDs.h"
#include "tableio/converter/pybindings.h"

void register_I3Converters()
{
    I3CONVERTER_NAMESPACE(clsim);

    I3CONVERTER_EXPORT_DEFAULT(I3PhotonConverter, "Dumps a single I3Photon to a table column");
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3PhotonSeriesMapConverter,"Dumps all I3Photons in a I3PhotonSeriesMap");

    // this is not the default converter, the default one is in dataclasses
    I3_MAP_CONVERTER_EXPORT(I3MCHitSeriesMapConverterWithIDs, "Dumps all I3MCHits from a I3MCHitSeriesMap");

    I3_MAP_CONVERTER_EXPORT(I3MCPESeriesMapConverterWithIDs, "Dumps all I3MCPEs from a I3MCPESeriesMap");

}
