/**
    copyright  (C) 2004
    the icecube collaboration

    @version $Revision: 94949 $
    @date $Date: 2012-11-04 10:40:30 -0600 (Sun, 04 Nov 2012) $
    @author blaufuss

    @todo
*/

#include <I3Test.h>
#include <iostream>
#include <fstream>
#include <string>
#include <icetray/serialization.h>
#include "boost/archive/xml_oarchive.hpp"
#include "dataclasses/status/I3DOMStatus.h"
#include "dataclasses/physics/I3DOMLaunch.h"

using namespace std;

TEST_GROUP(I3DOMStatus);

TEST(class_versioning)
{

  ofstream ofs("/tmp/I3DOMStatus.xml");
  boost::archive::xml_oarchive oa(ofs);

  I3DOMLaunch l;
  oa << make_nvp("launch",l);

  I3DOMStatus d;
  oa << make_nvp("status",d);

  unlink("/tmp/I3DOMStatus.xml");
}
