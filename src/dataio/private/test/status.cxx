/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: status.cxx 1433 2012-11-04 16:34:42Z nwhitehorn $

    @version $Revision: 1433 $
    @date $Date: 2012-11-04 10:34:42 -0600 (Sun, 04 Nov 2012) $

*/

#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>

#include <icetray/I3Logging.h>

#include "dataclasses/status/I3DOMStatus.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/status/I3TriggerStatus.h"


#include "serialization-test.h"

#include <boost/preprocessor.hpp>

using namespace boost::archive;
using namespace std;

TEST_GROUP(status);

#define TEST_THESE (I3DetectorStatus)

#define NON_I3FO_ITEMS (I3DOMStatus)(I3TriggerStatus)

#define SERIALIZATION_TEST(r,data,t) SERIALIZE(t)

BOOST_PP_SEQ_FOR_EACH(SERIALIZATION_TEST, ~, TEST_THESE);

