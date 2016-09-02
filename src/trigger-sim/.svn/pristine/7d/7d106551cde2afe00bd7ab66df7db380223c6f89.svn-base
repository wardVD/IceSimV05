#include <I3Test.h>

#include <trigger-sim/utilities/DOMSetFunctions.h>
#include <icetray/OMKey.h>

#include <vector>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;
using std::vector;

const int DOMSET_2_HIGH_STRING(78);
const int DOMSET_2_LOW_STRING(1);
const int DOMSET_2_HIGH_DOM(60);
const int DOMSET_2_LOW_DOM(1);

const int DOMSET_4_DC_HIGH_DOM(60);
const int DOMSET_4_DC_LOW_DOM(11);
const int DOMSET_4_II_HIGH_DOM(60);
const int DOMSET_4_II_LOW_DOM(41);
const std::vector<int> DOMSET_4_II_STRINGS = list_of(26)(27)(35)(36)(37)(45)(46);

const int DOMSET_5_DC_HIGH_DOM(60);
const int DOMSET_5_DC_LOW_DOM(11);
const int DOMSET_5_II_HIGH_DOM(60);
const int DOMSET_5_II_LOW_DOM(39);
const std::vector<int> DOMSET_5_II_STRINGS = list_of(26)(27)(35)(36)(37)(45)(46);

const int DOMSET_6_DC_HIGH_DOM(60);
const int DOMSET_6_DC_LOW_DOM(11);
const int DOMSET_6_II_HIGH_DOM(60);
const int DOMSET_6_II_LOW_DOM(39);
const std::vector<int> DOMSET_6_II_STRINGS = list_of(25)(26)(27)(34)(35)(36)(37)(44)(45)(46)(47)(54);

TEST_GROUP(DOMSetTests);

TEST(DOMSet2Test)
{

  // generate only DOM set 2 DOMs
  // and ensure that InDOMSet returns
  // true for all of them
  for(int str(DOMSET_2_LOW_STRING); 
      str <= DOMSET_2_HIGH_STRING;
      str++)
    for( int om(DOMSET_2_LOW_DOM);
	 om <= DOMSET_2_HIGH_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(DOMSetFunctions::InDOMSet(test_om,2,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(DOMSet4_DC_Test)
{

  // generate only DOM set 4 DeepCore DOMs
  // and ensure that InDOMSet returns
  // true for all of them
  for(int str(81); str <= 90; str++)
    for( int om(DOMSET_4_DC_LOW_DOM);
	 om <= DOMSET_4_DC_HIGH_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(DOMSetFunctions::InDOMSet(test_om,4,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(DOMSet4_II_Test)
{
  // generate only DOM set 4 InIce DOMs
  // and ensure that InDOMSet returns
  // true for all of them
  BOOST_FOREACH(const int& str, DOMSET_4_II_STRINGS)
    for( int om(DOMSET_4_II_LOW_DOM);
	 om <= DOMSET_4_II_HIGH_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(DOMSetFunctions::InDOMSet(test_om,4,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(NotInDOMSet4_DC_Test1)
{

  // generate DOMs *NOT* in DomSet 4 
  // and ensure InDOMSet returns false
  for(int str(81); str <= 90; str++)
    for( int om(1);
	 om < DOMSET_4_DC_LOW_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(!DOMSetFunctions::InDOMSet(test_om,4,I3MapKeyVectorIntConstPtr()));
    }
}

TEST(NotInDOMSet4_II_Test1)
{
  // generate DOMs *NOT* in DomSet 4 
  // and ensure InDOMSet returns false
  BOOST_FOREACH(const int& str, DOMSET_4_II_STRINGS)
    for( int om(1);
	 om < DOMSET_4_II_LOW_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(!DOMSetFunctions::InDOMSet(test_om,4,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(NotDOMSet4_II_String)
{
  // generate DOMs from strings *NOT* in DomSet 4 
  // and ensure InDOMSet returns false
  for(int test_string(1); test_string <= 78; test_string++){
    //skip strings that are not part of DeepCore
    if(count(DOMSET_4_II_STRINGS.begin(), DOMSET_4_II_STRINGS.end(),test_string))
      continue;
    for( int om(1);
	 om < 60 ;
	 om ++){
      OMKey test_om(test_string,om);
      ENSURE(!DOMSetFunctions::InDOMSet(test_om,4,I3MapKeyVectorIntConstPtr()));
    }
  }

}

TEST(DOMSet5_DC_Test)
{

  // generate only DOM set 5 DeepCore DOMs
  // and ensure that InDOMSet returns
  // true for all of them
  for(int str(81); str <= 90; str++)
    for( int om(DOMSET_5_DC_LOW_DOM);
	 om <= DOMSET_5_DC_HIGH_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(DOMSetFunctions::InDOMSet(test_om,5,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(DOMSet5_II_Test)
{
  // generate only DOM set 5 InIce DOMs
  // and ensure that InDOMSet returns
  // true for all of them
  BOOST_FOREACH(const int& str, DOMSET_5_II_STRINGS)
    for( int om(DOMSET_5_II_LOW_DOM);
	 om <= DOMSET_5_II_HIGH_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(DOMSetFunctions::InDOMSet(test_om,5,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(NotInDOMSet5_DC_Test1)
{

  // generate DOMs *NOT* in DomSet 5 
  // and ensure InDOMSet returns false
  for(int str(81); str <= 90; str++)
    for( int om(1);
	 om < DOMSET_5_DC_LOW_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(!DOMSetFunctions::InDOMSet(test_om,5,I3MapKeyVectorIntConstPtr()));
    }
}

TEST(NotInDOMSet5_II_Test1)
{
  // generate DOMs *NOT* in DomSet 5 
  // and ensure InDOMSet returns false
  BOOST_FOREACH(const int& str, DOMSET_5_II_STRINGS)
    for( int om(1);
	 om < DOMSET_5_II_LOW_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(!DOMSetFunctions::InDOMSet(test_om,5,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(NotDOMSet5_II_String)
{
  // generate DOMs from strings *NOT* in DomSet 5 
  // and ensure InDOMSet returns false
  for(int test_string(1); test_string <= 78; test_string++){
    //skip strings that are not part of DeepCore
    if(count(DOMSET_5_II_STRINGS.begin(), DOMSET_5_II_STRINGS.end(),test_string))
      continue;
    for( int om(1);
	 om < 60 ;
	 om ++){
      OMKey test_om(test_string,om);
      ENSURE(!DOMSetFunctions::InDOMSet(test_om,5,I3MapKeyVectorIntConstPtr()));
    }
  }

}

// DOMSET 6 tests

TEST(DOMSet6_DC_Test)
{

  // generate only DOM set 6 DeepCore DOMs
  // and ensure that InDOMSet returns
  // true for all of them
  for(int str(79); str <= 86; str++)
    for( int om(DOMSET_6_DC_LOW_DOM);
	 om <= DOMSET_6_DC_HIGH_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(DOMSetFunctions::InDOMSet(test_om,6,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(DOMSet6_II_Test)
{
  // generate only DOM set 6 InIce DOMs
  // and ensure that InDOMSet returns
  // true for all of them
  BOOST_FOREACH(const int& str, DOMSET_6_II_STRINGS)
    for( int om(DOMSET_6_II_LOW_DOM);
	 om <= DOMSET_6_II_HIGH_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(DOMSetFunctions::InDOMSet(test_om,6,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(NotInDOMSet6_DC_Test1)
{

  // generate DOMs *NOT* in DomSet 6 
  // and ensure InDOMSet returns false
  for(int str(79); str <= 86; str++)
    for( int om(1);
	 om < DOMSET_6_DC_LOW_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(!DOMSetFunctions::InDOMSet(test_om,6,I3MapKeyVectorIntConstPtr()));
    }
}

TEST(NotInDOMSet6_II_Test1)
{
  // generate DOMs *NOT* in DomSet 6 
  // and ensure InDOMSet returns false
  BOOST_FOREACH(const int& str, DOMSET_6_II_STRINGS)
    for( int om(1);
	 om < DOMSET_6_II_LOW_DOM ;
	 om ++){
      OMKey test_om(str,om);
      ENSURE(!DOMSetFunctions::InDOMSet(test_om,6,I3MapKeyVectorIntConstPtr()));
    }

}

TEST(NotDOMSet6_II_String)
{
  // generate DOMs from strings *NOT* in DomSet 6 
  // and ensure InDOMSet returns false
  for(int test_string(1); test_string <= 78; test_string++){
    //skip strings that are not part of DeepCore
    if(count(DOMSET_6_II_STRINGS.begin(), DOMSET_6_II_STRINGS.end(),test_string))
      continue;
    for( int om(1);
	 om < 60 ;
	 om ++){
      OMKey test_om(test_string,om);
      ENSURE(!DOMSetFunctions::InDOMSet(test_om,6,I3MapKeyVectorIntConstPtr()));
    }
  }

}
