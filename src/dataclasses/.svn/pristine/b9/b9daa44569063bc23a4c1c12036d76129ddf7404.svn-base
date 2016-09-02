#include <I3Test.h>

#include <dataclasses/physics/I3FilterResult.h>

#include <fstream>
#include <boost/archive/xml_oarchive.hpp>

TEST_GROUP(I3FilterResultTest);

TEST(out_and_in)
{
  {
    std::ofstream fout("test-filtermask-out.xml");
    boost::archive::xml_oarchive ar(fout);
    I3FilterResult result;
    ENSURE(!result.conditionPassed,"Initialized correctly");
    ENSURE(!result.prescalePassed,"Initialized correctly");
    result.conditionPassed = true;
    result.prescalePassed = true;
    ar << make_nvp("result",result);
  }

  {
    std::ifstream fin("test-filtermask-out.xml");
    boost::archive::xml_iarchive ar(fin);
    I3FilterResult result;
    ar >> make_nvp("result",result);
    ENSURE(result.conditionPassed,"Read In correctly");
    ENSURE(result.prescalePassed,"Read In correctly");
  }
}
