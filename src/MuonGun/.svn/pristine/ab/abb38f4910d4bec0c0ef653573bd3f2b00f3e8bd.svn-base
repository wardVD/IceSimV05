/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <MuonGun/SamplingSurface.h>
#include <icetray/I3Logging.h>

namespace I3MuonGun {

template <typename Archive>
void
SamplingSurface::serialize(Archive &ar, unsigned version)
{
	if (version > 0)
		log_fatal_stream("Version "<<version<<" is from the future");

	ar & make_nvp("Base", base_object<I3Surfaces::SamplingSurface>(*this));
}

SamplingSurface::~SamplingSurface() {}

}

I3_SERIALIZABLE(I3MuonGun::SamplingSurface);
