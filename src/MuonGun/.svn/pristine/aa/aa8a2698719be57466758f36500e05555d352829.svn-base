/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <MuonGun/I3MuonGun.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Direction.h>

void
register_I3MuonGun()
{
	using namespace I3MuonGun;
	namespace bp = boost::python;
	
	bp::def("depth", &GetDepth, "Convert a z coordinate to a depth.");
}
