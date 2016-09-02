#include <I3Test.h>
#include <vector>

#include <payload-parsing/delta_decode.h>

using namespace std;

TEST_GROUP(bitbuffertest);

using namespace payload_parsing;

TEST(remainingBits)
{
  vector<char> buf(100);
  bitbuffer b(buf,0,50);
  ENSURE(b.remainingBits() == 50 *8);
  b.pop(3);
  ENSURE(b.remainingBits() == 50 *8 - 3);
  b.pop(11);
  ENSURE(b.remainingBits() == 50 *8 - 3 - 11);
}

TEST(twos_complement)
{
  {
    delta d(0,4);
    dumpbits(d);
    ENSURE(d.getValue() == 0);
  }
  {
    delta d(1,4);
    dumpbits(d);
    ENSURE(d.getValue() == 1);
  }
  {
    delta d(2,4);
    dumpbits(d);
    ENSURE(d.getValue() == 2);
  }
  {
    delta d(3,4);
    dumpbits(d);
    ENSURE(d.getValue() == 3);
  }
  {
    delta d(4,4);
    dumpbits(d);
    ENSURE(d.getValue() == 4);
  }
  {
    delta d(5,4);
    dumpbits(d);
    ENSURE(d.getValue() == 5);
  }
  {
    delta d(6,4);
    dumpbits(d);
    ENSURE(d.getValue() == 6);
  }
  {
    delta d(7,4);
    dumpbits(d);
    ENSURE(d.getValue() == 7);
  }
  {
    delta d(8,4);
    dumpbits(d);
    ENSURE(d.getValue() == -8);
  }
  {
    delta d(9,4);
    dumpbits(d);
    ENSURE(d.getValue() == -7);
  }
  {
    delta d(10,4);
    dumpbits(d);
    ENSURE(d.getValue() == -6);
  }
  {
    delta d(11,4);
    dumpbits(d);
    ENSURE(d.getValue() == -5);
  }
  {
    delta d(12,4);
    dumpbits(d);
    ENSURE(d.getValue() == -4);
  }
  {
    delta d(13,4);
    dumpbits(d);
    ENSURE(d.getValue() == -3);
  }
  {
    delta d(14,4);
    dumpbits(d);
    ENSURE(d.getValue() == -2);
  }
  {
    delta d(15,4);
    dumpbits(d);
    ENSURE(d.getValue() == -1);
  }
}


TEST(test_read_in_middle_oneread)
{
  cout<<endl;
  cout<<endl;
  cout<<endl;
  vector<char> buffer(100);

#if defined(BOOST_LITTLE_ENDIAN)
  {
    *((unsigned*)&buffer[16]) = 0x00112304;
    *((unsigned*)&buffer[20]) = 0xc70671e0;
  }
#elif defined(BOOST_BIG_ENDIAN)
  {
    *((unsigned*)&buffer[16]) = 0x04231100;
    *((unsigned*)&buffer[20]) = 0xe07106c7;
  }
#endif

  bitbuffer bits(buffer,0,8);

  cout<<bits.remainingBits()<<endl;

  bits.pop(128);

  {
    delta d = bits.pop(3);
    dumpbits(d);
    ENSURE(d.isFlag());
  }
}

TEST(test_read_in_middle)
{
  cout<<endl;
  cout<<endl;
  cout<<endl;
  vector<char> buffer(400);
#if defined(BOOST_LITTLE_ENDIAN)
  {
    *((unsigned*)&buffer[16]) = 0x00112304;
    *((unsigned*)&buffer[20]) = 0xc70671e0;
  }
#elif defined(BOOST_BIG_ENDIAN)
  {
    *((unsigned*)&buffer[16]) = 0x04231100;
    *((unsigned*)&buffer[20]) = 0xe07106c7;
  }
#endif

  bitbuffer bits(buffer,0,100);

  // cout<<bits.remainingBits()<<endl;
  // the way this is setup there is not data before byte 16 (bit 128)
  // so we need to go to the data first in order to be able to decode
  // it correctly. Hence we have the extra bits.pop(1) here in order
  // to correctly go to the first real data:
  bits.pop(1);
  bits.pop(3);
  bits.pop(6);
  bits.pop(11);
  bits.pop(11);
  bits.pop(6);
  bits.pop(3);
  bits.pop(2);
  bits.pop(2);
  bits.pop(2);
  bits.pop(3);
  bits.pop(6);
  bits.pop(6);
  bits.pop(11);
  bits.pop(11);
  bits.pop(11);
  bits.pop(11);
  bits.pop(11);
  bits.pop(11);

  {
    delta d = bits.pop(3);
    dumpbits(d);
    ENSURE(d.isFlag());
  }

  {
    delta d = bits.pop(6);
    dumpbits(d);
    ENSURE(d.isFlag());
  }

  {
    delta d = bits.pop(11);
    dumpbits(d);
    ENSURE(d.getValue() == 145);
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(11);
    dumpbits(d);
    ENSURE(d.getValue() == 1);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(6);
    dumpbits(d);
    ENSURE(d.getValue() == 0);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(3);
    dumpbits(d);
    ENSURE(d.getValue() == -1);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == 1);
    ENSURE(d.getValue() == 1);
    ENSURE(d.getValue() == 1);
    ENSURE(!d.isFlag());
    ENSURE(d.getBits() == 2);
    ENSURE(d.getValue() == 1);
    //    ENSURE(abs(d.getValue()) < 1);
    ENSURE(d.getBits() == 2);
    ENSURE(!d.wastedBits());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == 0);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(1);
    dumpbits(d);
    ENSURE(d.isFlag());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == -1);
    ENSURE(!d.isFlag());
    ENSURE(!d.wastedBits());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == 0);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(1);
    dumpbits(d);
    ENSURE(d.isFlag());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == 1);
  }


  cout<<bits.remainingBits()<<endl;
}

TEST(the_test)
{
  cout<<endl;
  cout<<endl;
  cout<<endl;
  vector<char> buffer(100);

#if defined(BOOST_LITTLE_ENDIAN)
  {
    *((unsigned*)&buffer[0]) = 0x00112304;
    *((unsigned*)&buffer[4]) = 0xc70671e0;
  }
#elif defined(BOOST_BIG_ENDIAN)
  {
    *((unsigned*)&buffer[0]) = 0x04231100;
    *((unsigned*)&buffer[4]) = 0xe07106c7;
  }
#endif

  bitbuffer bits(buffer,0,100);

  {
    delta d = bits.pop(3);
    dumpbits(d);
    ENSURE(d.isFlag());
  }

  {
    delta d = bits.pop(6);
    dumpbits(d);
    ENSURE(d.isFlag());
  }

  {
    delta d = bits.pop(11);
    dumpbits(d);
    ENSURE(d.getValue() == 145);
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(11);
    dumpbits(d);
    ENSURE(d.getValue() == 1);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(6);
    dumpbits(d);
    ENSURE(d.getValue() == 0);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(3);
    dumpbits(d);
    ENSURE(d.getValue() == -1);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == 1);
    ENSURE(d.getValue() == 1);
    ENSURE(d.getValue() == 1);
    ENSURE(!d.isFlag());
    ENSURE(d.getBits() == 2);
    ENSURE(d.getValue() == 1);
    //    ENSURE(abs(d.getValue()) < 1);
    ENSURE(d.getBits() == 2);
    ENSURE(!d.wastedBits());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == 0);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(1);
    dumpbits(d);
    ENSURE(d.isFlag());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == -1);
    ENSURE(!d.isFlag());
    ENSURE(!d.wastedBits());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == 0);
    ENSURE(d.wastedBits());
    ENSURE(!d.isFlag());
  }

  {
    delta d = bits.pop(1);
    dumpbits(d);
    ENSURE(d.isFlag());
  }

  {
    delta d = bits.pop(2);
    dumpbits(d);
    ENSURE(d.getValue() == 1);
  }

  //while(not done)
  //{
  //  dumped = bits.pop(bpw);
  //  if dumped is flag:
  //    bpw += howeverMany;
  //  else
  //  {
  //    accumulatedWaveform[i+1] = accumulatedWaveform[i] + dumped;
  //    if dumped is too small:
  //      bpw -= howeverMany;
  //  }
  //}

  /*
  uint i = 0;
  int bpw = 3;
  while (i < 5)
  {
    dumpbits(bits.pop(bpw)); 

    if (delta::isFlag() true)
    {
      if (bpw == 1)
      {
        bpw = 2;
      }
      else if (bpw == 2)
      {
        bpw = 3;
      }
      else if (bpw == 3)
      {
        bpw = 6;
      }
      else if (bpw == 6)
      {
        bpw = 11;
      }
      else {
        assert(0);
      }
    }
    else {
      cout << endl;
      cout << "bpw: " << bpw << endl;
      cout << "Delta's: " << dumpbits(bits.pop(bpw)) << endl;
    }
    i++;
  }
  */
}
