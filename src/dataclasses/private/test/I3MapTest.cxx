/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: I3MapTest.cxx 94949 2012-11-04 16:40:30Z nwhitehorn $

    @version $Revision: 94949 $
    @date $Date: 2012-11-04 10:40:30 -0600 (Sun, 04 Nov 2012) $
    @author pretz

    @todo
*/

#include <I3Test.h>

#include "icetray/OMKey.h"
#include "dataclasses/I3Map.h"

#include <iostream>
#include <string>

using namespace std;

TEST_GROUP(I3Map);

TEST(at)
{
  I3Map<std::string,std::string> mymap;

  mymap["one"] = "one";
  mymap["two"] = "two";
  mymap.at("one") = "One";
  mymap.at("two") = "Two";

  std::cout << mymap.at("one");
  std::cout << mymap.at("two");
  try {
    std::cout << mymap.at("three");
    FAIL("that should have thrown.  there's nothing at three.");
  } catch(const std::exception&e) { }
}

TEST(with_omkeys)
{
  I3Map<OMKey,std::string> mymap;

  mymap[OMKey(1,1)] = "one-one";
  mymap[OMKey(2,2)] = "two-two";
  mymap.at(OMKey(1,1)) = "One-One";
  mymap.at(OMKey(2,2)) = "Two-Two";

  std::cout << mymap.at(OMKey(1,1));
  std::cout << mymap.at(OMKey(2,2));
  try {
    std::cout << mymap.at(OMKey(3,3));
    FAIL("that should have thrown, there's nothing at OMKey(3,3)");
  } catch (const std::exception&e) { }
}
