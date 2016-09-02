#include <I3Test.h>
#include <payload-parsing/utility.h>
#include <stdint.h>

TEST_GROUP(bad_decode);

using namespace std;
using namespace payload_parsing;

TEST(uint64_t_test)
{
  vector<char> buffer(7);
  uint32_t out = decode<uint32_t>(BIG,0,buffer);
  cout<<out<<endl; // just to stop warning
  try 
  {
    uint64_t out2 = decode<uint64_t>(BIG,0,buffer);
    cout<<out2<<endl; // just to stop warning
    ENSURE(0,"That should have thrown");
  }
  catch(const exception& e)
  {
  }
}


TEST(uint64_t_test_from_middle)
{
  vector<char> buffer(50);
  try 
  {
    uint64_t out = decode<uint64_t>(BIG,43,buffer);
    cout<<out<<endl; // just to stop warning
    ENSURE(0,"That should have thrown");
  }
  catch(const exception& e)
  {
  }
}

TEST(uint64_t_read_past_end)
{
  vector<char> buffer(50);
  try 
  {
    uint64_t out = decode<uint64_t>(BIG,60,buffer);
    cout<<out<<endl; // just to stop warning
    ENSURE(0,"That should have thrown");
  }
  catch(const exception& e)
  {
  }
}

TEST(size_zero_test)
{
  vector<char> buffer(0);
  try 
  {
    uint32_t out2 = decode<uint32_t>(BIG,0,buffer);
    cout<<out2<<endl; // just to stop warning
    ENSURE(0,"That should have thrown");
  }
  catch(const exception& e)
  {
  }


}
