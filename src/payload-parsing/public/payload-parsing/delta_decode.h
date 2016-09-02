#ifndef DELTA_DECODE_H
#define DELTA_DECODE_H

#include <vector>
#include <payload-parsing/utility.h>
#include <payload-parsing/decode.h>

class I3DOMLaunch;


namespace payload_parsing
{
  template <class T>
    bool getbit(unsigned bit,T value)
    {
      T mask = 0x01 << bit;
      bool toReturn = (value & mask )>> bit;
      return toReturn;
    }

  void delta_decode(unsigned start,
      const std::vector<char>& buffer,
      I3DOMLaunch& launch,
      unsigned WhichChargeStamp);


  struct delta
  {
    delta(uint32_t value,unsigned bits) : value_(value),bits_(bits){}

    bool isFlag()
    {
      switch (bits_)
      {
        case 1:
          if(value_ == 1)
            return true;
        case 2:
          if(value_ == 2)
            return true;
        case 3:
          if(value_ == 4)
            return true;
        case 6:
          if(value_ == 32)
            return true;
      }
      return false;
    };

    int getValue()
    {
      bool mostSignificantBit = getbit(bits_-1,value_);
      if(mostSignificantBit)
      {
        int toReturn = 0;
        for(unsigned i = 0 ; i < bits_ -1; i++)
        {
          bool bit = getbit(i,value_);
          toReturn = toReturn | ((bit ? 0 : 1) << i);
        }
        return -(toReturn +1);
      }
      else
      {
        return value_;
      }
    }

    unsigned rawValue()
    {
      return value_;
    }

    unsigned getBits()
    {
      return bits_;
    }

    bool wastedBits()
    {
      if(isFlag())
      {
        return false;
      }

      switch (bits_)
      {
        case 1:
          return false;
          break;
        case 2:
          if(abs(getValue()) < 1)
            return true;
          break;
        case 3:
          if(abs(getValue()) < 2)
            return true;
          break;
        case 6:
          if(abs(getValue()) < 4)
            return true;
          break;
        case 11:
          if(abs(getValue()) < 32)
            return true;
          break;
      }
      return false;
    }

    private:
    int value_;
    unsigned bits_;
  };

  class bitbuffer
  {
    public:
      bitbuffer(const std::vector<char>& v,
          unsigned start,
          unsigned length) : buffer(v)
      {
        bitPosition = 0;
        bytePosition = start;
        remainingBits_ = length * 8;

        //          if(start + length > v.size())
        // log_fatal("error in bit buffer.  The end of the bit buffer is " */
        //"beyond the data available.  Corrupt data?  Bad code?"); */
      }

      int remainingBits()
      {
        return remainingBits_ >= 0 ? remainingBits_ : 0;
      }

      delta pop(unsigned size)
      {
        remainingBits_ -= size;
        uint32_t mask = 0;
        //cout << "Bit Position: " << bitPosition << endl;
        unsigned posn = bitPosition;
        for(unsigned i = 0 ; i < size ; i++)
        {
          unsigned thisMask = 0x00000001;
          thisMask = thisMask << posn;
          mask = mask | thisMask;
          posn += 1;
        }
        //    bittify(mask);
        if(bytePosition >= buffer.size())
          log_fatal("reading past the end of the buffer");
        const char* readLocation = &buffer[bytePosition];
        uint32_t out = *((uint32_t*)readLocation);

        //The delta compressed data is little endian so on a big endian
        //machine we need to endian swap the data:

#if defined(BOOST_LITTLE_ENDIAN)
        {
          // out = out; // no-op
        }
#elif defined(BOOST_BIG_ENDIAN)
        {
          out = endian_swap(out);
        }
#endif

        //    bittify(out);
        out = out & mask;
        out = out >> bitPosition;

        unsigned nextBitPosn = bitPosition + size;
        unsigned byteOffset = nextBitPosn / 8;
        bytePosition += byteOffset;
        bitPosition = nextBitPosn % 8;

        return delta(out,size);
      }
      
    private:
      const std::vector<char>& buffer;
      unsigned bytePosition;
      unsigned bitPosition;
      int remainingBits_;
  };


  inline int dumpbits(delta todump)
  {

    for(unsigned i = 0 ; i < todump.getBits(); i++)
    {
      std::cout<<getbit(todump.getBits() - i -1,todump.rawValue());
    }
    std::cout<<"="<<todump.rawValue();
    std::cout<<"   "<<todump.getValue();

    std::cout<<std::endl;

    return 0;
  }

}

#endif
