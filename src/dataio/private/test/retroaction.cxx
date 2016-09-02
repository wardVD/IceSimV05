/**
   copyright  (C) 2004
   the icecube collaboration
   $Id: retroaction.cxx 1688 2013-07-10 18:20:38Z nwhitehorn $

   @version $Revision: 1688 $
   @date $Date: 2013-07-10 13:20:38 -0500 (Wed, 10 Jul 2013) $

*/

#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>

#include <icetray/I3Logging.h>
#include <icetray/Utility.h>

#include "serialization-test.h"
#include "dataio-test.h"

#include <boost/preprocessor.hpp>
#include <boost/foreach.hpp>

using namespace boost::archive;
using namespace std;

TEST_GROUP(retroaction);

TEST(read)
{
  vector<string> i3files;
  string ports = GetDataDir();

  glob((ports + "/serialization/*/*.i3").c_str(), i3files);
  ENSURE(i3files.size() != 0);
  BOOST_FOREACH(const string& s, i3files)
    {
      log_info("%s", s.c_str());
      I3FramePtr fp = load_i3_file(s);
      ENSURE((bool)fp);
	
      cout << "From " << s << ":\n" << *fp << "\n";
      for (I3Frame::const_iterator iter = fp->begin();
	   iter != fp->end();
	   iter++)
	cout << iter->first << " deserialized\n";
    }
}
