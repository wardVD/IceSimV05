#include <string>
#include <stdint.h>
#include <I3Test.h>
#include <payload-parsing/utility.h>
#include "bele.h"

TEST_GROUP(endianness);

using namespace std;
using namespace payload_parsing;

// an independent implementation of decoding from different endiannesses, 
// to check our decoding
// for consistency
template <unsigned length>
struct decoding_properties;

template<>
struct decoding_properties<8>
{
  typedef uint8_t type;
};


template<>
struct decoding_properties<16>
{
  typedef uint16_t type;
};

template<>
struct decoding_properties<32>
{
  typedef uint32_t type;
};

template<>
struct decoding_properties<48>
{
  typedef uint64_t type;
};

template<>
struct decoding_properties<64>
{
  typedef uint64_t type;
};


template<unsigned bits,class buffer>
  typename decoding_properties<bits>::type 
decode_buffer(payload_parsing::Endian e,const buffer& rec,unsigned offset=0)
{
  typedef typename decoding_properties<bits>::type output;
  output toReturn = 0;
  for(unsigned i = 0 ; i < bits/8 ; i++)
  {
    unsigned getbits = 0;
    if(e==BIG)
      getbits = bits - ((1 + i) *8);
    else
      getbits = i*8;

    output mask = 0xff;
    mask = mask <<getbits;
    output temp = (output)rec[i+offset];
    temp = temp<<getbits;
    toReturn = toReturn | (temp & mask);
  }
  return toReturn;
}




TEST(decode64)
{
  vector<char> data(125);
  data[102] = 0x12;
  data[103] = 0x34;
  data[104] = 0x56;
  data[105] = 0x78;
  data[106] = 0x9a;
  data[107] = 0xbc;
  data[108] = 0xbc;
  data[109] = 0xbc;

  ENSURE(decode<uint64_t>(BIG, 102, data) == 
      decode_buffer<64>(BIG, data, 102));
}

TEST(decode32)
{
  vector<char> data(125);
  data[102] = 0x12;
  data[103] = 0x34;
  data[104] = 0x56;
  data[105] = 0x78;
  data[106] = 0x9a;
  data[107] = 0xbc;

  ENSURE(decode<uint32_t>(BIG, 102, data) == 
      decode_buffer<32>(BIG, data, 102));
}

TEST(decode16)
{
  vector<char> data(125);
  data[102] = 0x12;
  data[103] = 0x34;
  data[104] = 0x56;
  data[105] = 0x78;
  data[106] = 0x9a;
  data[107] = 0xbc;

  ENSURE(decode<uint16_t>(BIG, 102, data) == 
      decode_buffer<16>(BIG, data, 102));
}

TEST(endian_swap_16)
{
  uint16_t in = 0x1234;

  uint16_t out = endian_swap(in);

  ENSURE(out == 0x3412);
}

TEST(endian_swap_32)
{
  uint32_t in = 0x12345678;

  uint32_t out = endian_swap(in);

  ENSURE(out == 0x78563412);
}

TEST(endian_swap_64)
{
  uint64_t in = 0x123456789abcdef0LL;

  uint64_t out = endian_swap(in);

  ENSURE(out == 0xf0debc9a78563412LL);
}

TEST(decode_uint16_t)
{
  vector<char> data(10);
  data[0] = 0x12;
  data[1] = 0x34;

  uint16_t testBIG =
    decode<uint16_t>(BIG,0,data);

  uint16_t testLITTLE =
    decode<uint16_t>(LITTLE,0,data);

  ENSURE(testLITTLE == 0x3412);
  ENSURE(testBIG == 0x1234);
}

TEST(decode_uint32_t)
{
  vector<char> data(10);
  data[0] = 0x12;
  data[1] = 0x34;
  data[2] = 0x56;
  data[3] = 0x78;

  uint32_t testBIG = 
    decode<uint32_t>(BIG, 0, data);

  uint32_t testLITTLE =
    decode<uint32_t>(LITTLE, 0, data);

  ENSURE(testLITTLE == 0x78563412);
  ENSURE(testBIG == 0x12345678);
}


TEST(decode_uint64_t)
{
  vector<char> data(30);
  data[0] = 0x12;
  data[1] = 0x34;
  data[2] = 0x56;
  data[3] = 0x78;
  data[4] = 0x9a;
  data[5] = 0xbc;
  data[6] = 0xde;
  data[7] = 0xf1;

  uint64_t testBIG = 
    decode<uint64_t>(BIG, 0, data);

  uint64_t testLITTLE =
    decode<uint64_t>(LITTLE, 0, data);

  ENSURE(testLITTLE == 0xf1debc9a78563412LL);
  ENSURE(testBIG == 0x123456789abcdef1LL);
}

TEST(decode_uint16_t_middle)
{
  vector<char> data(10);
  data[4] = 0x12;
  data[5] = 0x34;

  uint16_t testBIG =
    decode<uint16_t>(BIG,4,data);

  uint16_t testLITTLE =
    decode<uint16_t>(LITTLE,4,data);

  ENSURE(testLITTLE == 0x3412);
  ENSURE(testBIG == 0x1234);
}

TEST(decode_uint32_t_middle)
{
  vector<char> data(10);
  data[3] = 0x12;
  data[4] = 0x34;
  data[5] = 0x56;
  data[6] = 0x78;

  uint32_t testBIG = 
    decode<uint32_t>(BIG, 3, data);

  uint32_t testLITTLE =
    decode<uint32_t>(LITTLE, 3, data);

  ENSURE(testLITTLE == 0x78563412);
  ENSURE(testBIG == 0x12345678);
}


TEST(decode_uint64_t_middle)
{
  vector<char> data(30);
  data[12] = 0x12;
  data[13] = 0x34;
  data[14] = 0x56;
  data[15] = 0x78;
  data[16] = 0x9a;
  data[17] = 0xbc;
  data[18] = 0xde;
  data[19] = 0xf1;

  uint64_t testBIG = 
    decode<uint64_t>(BIG, 12, data);

  uint64_t testLITTLE =
    decode<uint64_t>(LITTLE, 12, data);

  ENSURE(testLITTLE == 0xf1debc9a78563412LL);
  ENSURE(testBIG == 0x123456789abcdef1LL);
}

TEST(decode_uint16_t_end)
{
  vector<char> data(10);
  data[8] = 0x12;
  data[9] = 0x34;

  uint16_t testBIG =
    decode<uint16_t>(BIG,8,data);

  uint16_t testLITTLE =
    decode<uint16_t>(LITTLE,8,data);

  ENSURE(testLITTLE == 0x3412);
  ENSURE(testBIG == 0x1234);
}

TEST(decode_uint32_t_end)
{
  vector<char> data(10);
  data[6] = 0x12;
  data[7] = 0x34;
  data[8] = 0x56;
  data[9] = 0x78;

  uint32_t testBIG = 
    decode<uint32_t>(BIG, 6, data);

  uint32_t testLITTLE =
    decode<uint32_t>(LITTLE,6 , data);

  ENSURE(testLITTLE == 0x78563412);
  ENSURE(testBIG == 0x12345678);
}


TEST(decode_uint64_t_end)
{
  vector<char> data(30);
  data[22] = 0x12;
  data[23] = 0x34;
  data[24] = 0x56;
  data[25] = 0x78;
  data[26] = 0x9a;
  data[27] = 0xbc;
  data[28] = 0xde;
  data[29] = 0xf1;

  uint64_t testBIG = 
    decode<uint64_t>(BIG, 22, data);

  uint64_t testLITTLE =
    decode<uint64_t>(LITTLE, 22, data);

  ENSURE(testLITTLE == 0xf1debc9a78563412LL);
  ENSURE(testBIG == 0x123456789abcdef1LL);
}


TEST(decode_uint16_t_enoughdtata)
{
  vector<char> data(10);
  data[8] = 0x12;
  data[9] = 0x34;
  try
  {
    decode<uint16_t>(LITTLE,9,data);
    ENSURE(0,"That should have thrown");
  }
  catch(const runtime_error& e)
  {
    cout<<"got a runtime_error"<<endl;
  }
  catch(const exception&)
  {
    ENSURE(0,"That should have been a runtime_error");
  }

}


TEST(decode_uint32_t_enoughdtata)
{ 
  vector<char> data(10);
  data[6] = 0x12;
  data[7] = 0x34;
  data[8] = 0x56;
  data[9] = 0x78;

  try
  {
    decode<uint32_t>(LITTLE,7,data);
    ENSURE(0,"That should have thrown");
  }
  catch(const runtime_error& e)
  {
    cout<<"got a runtime_error"<<endl;
  }
  catch(const exception&)
  {
    ENSURE(0,"That should have been a runtime_error");
  }

}

TEST(decode_uint64_t_enoughdtata)
{ 
  vector<char> data(30);
  data[22] = 0x12;
  data[23] = 0x34;
  data[24] = 0x56;
  data[25] = 0x78;
  data[26] = 0x9a;
  data[27] = 0xbc;
  data[28] = 0xde;
  data[29] = 0xf1;

  try
  {
    decode<uint64_t>(LITTLE,23,data);
    ENSURE(0,"That should have thrown");
  }
  catch(const runtime_error& e)
  {
    cout<<"got a runtime_error"<<endl;
  }
  catch(const exception&)
  {
    ENSURE(0,"That should have been a runtime_error");
  }
}

TEST(decode_uint16_t_enoughdtata_BIG)
{
  vector<char> data(10);
  data[8] = 0x12;
  data[9] = 0x34;
  try
  {
    decode<uint16_t>(BIG,9,data);
    ENSURE(0,"That should have thrown");
  }
  catch(const runtime_error& e)
  {
    cout<<"got a runtime_error"<<endl;
  }
  catch(const exception&)
  {
    ENSURE(0,"That should have been a runtime_error");
  }

}


TEST(decode_uint32_t_enoughdtata_BIG)
{ 
  vector<char> data(10);
  data[6] = 0x12;
  data[7] = 0x34;
  data[8] = 0x56;
  data[9] = 0x78;

  try
  {
    decode<uint32_t>(BIG,7,data);
    ENSURE(0,"That should have thrown");
  }
  catch(const runtime_error& e)
  {
    cout<<"got a runtime_error"<<endl;
  }
  catch(const exception&)
  {
    ENSURE(0,"That should have been a runtime_error");
  }

}

TEST(decode_uint64_t_enoughdtata_BIG)
{ 
  vector<char> data(30);
  data[22] = 0x12;
  data[23] = 0x34;
  data[24] = 0x56;
  data[25] = 0x78;
  data[26] = 0x9a;
  data[27] = 0xbc;
  data[28] = 0xde;
  data[29] = 0xf1;

  try
  {
    decode<uint64_t>(BIG,23,data);
    ENSURE(0,"That should have thrown");
  }
  catch(const runtime_error& e)
  {
    cout<<"got a runtime_error"<<endl;
  }
  catch(const exception&)
  {
    ENSURE(0,"That should have been a runtime_error");
  }
}


TEST(signed_vs_unsigned)
{
  vector<unsigned char> data_unsigned(8);
  vector<char> data_signed(8);

  for(unsigned char offset = 0 ; offset < 255 ; offset+= 1)
  {
    for(unsigned char i = 0 ; i < 8 ; i++)
    {
      data_unsigned[i] = i+ offset;
      data_signed[i] = i + offset;
    }

    uint64_t out_1 = decode_buffer<64>(BIG,data_signed);
    uint64_t out_2 = decode_buffer<64>(BIG,data_unsigned);

    ostringstream message;
    message<<"failed on offset "<<(unsigned)offset<<endl;
    ENSURE(out_1 == out_2,message.str().c_str());
  }
}
