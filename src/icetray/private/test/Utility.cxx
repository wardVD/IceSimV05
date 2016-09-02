/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: Utility.cxx 1433 2012-11-04 16:34:42Z nwhitehorn $
 *
 * @version $Revision: 1433 $
 * @date $Date: 2012-11-04 10:34:42 -0600 (Sun, 04 Nov 2012) $
 * @author troy d. straszheim
 *
 * This tests that the global GetService<> works; that the underlying
 * context-switching is done correctly.
 */

#include <I3Test.h>
#include <boost/preprocessor/stringize.hpp>
#include "icetray/I3Tray.h"
#include "icetray/I3ServiceFactory.h"
#include <icetray/I3Frame.h>
#include <icetray/OMKey.h>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

TEST_GROUP(Utility);

TEST(stlfilt)
{
  ENSURE_EQUAL(stlfilt("foo"), "foo");
  ENSURE_EQUAL(stlfilt("std::foo"), "foo");
  ENSURE_EQUAL(stlfilt("std::vector<donk, std::allocator<donk> >"), "vector<donk>");
  ENSURE_EQUAL(stlfilt("std::map<Key, Value, less<Key>, allocator<pair<const Key, Value> > >"),
	       "map<Key, Value>");
}


