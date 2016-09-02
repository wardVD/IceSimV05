#include <vector>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <dataclasses/physics/I3DOMLaunch.h>
#include <payload-parsing/dump-raw-deltacompressed.h>
#include <payload-parsing/utility.h>

#define MAXHITBLOCK 4024 // Can't be any bigger because of domapp /coms

using namespace std;


namespace payload_parsing
{

  int BitsPerWord_;
  int BitBoundary_;

  int ShiftUp(int BitsPerWord_)
  {
    //    cout << "shift up" << endl;
    switch (BitsPerWord_)
    {
      case 1:
        BitsPerWord_ = 2;
        BitBoundary_ = 1;
        break;
      case 2:
        BitsPerWord_ = 3;
        BitBoundary_ = 2;
        break;
      case 3:
        BitsPerWord_ = 6;
        BitBoundary_ = 4;
        break;
      case 6:
        BitsPerWord_ = 11;
        BitBoundary_ = 32;
      case 11:
        break;
    }
    // cout << "fIBitsPerWord_ after shifting inside shift function: " << BitsPerWord_ << endl;
    return BitsPerWord_;
    log_trace("Shifting bits Up");
  }

  int ShiftDown(int BitsPerWord_)
  {
    //cout << "shift down" << endl;
    switch (BitsPerWord_)
    {
      case 1:
        break;
      case 2:
        BitsPerWord_ = 1;
        BitBoundary_ = 0;
        break;
      case 3:
        BitsPerWord_ = 2;
        BitBoundary_ = 1;
        break;
      case 4:
        BitsPerWord_ = 3;
        BitBoundary_ = 2;
        break;
      case 6:
        BitsPerWord_ = 3;
        BitBoundary_ = 2;
        break;
      case 11:
        BitsPerWord_ = 6;
        BitBoundary_ = 4;
        break;
    }
    return BitsPerWord_;
    log_trace("shifting bits down");
  }

  int GetBits(const vector<char>& buffer)
  {
    int n_byte_read = 0;
    int BitsValid_ = 0;
    //int BitsRegister_ = 0;
    BitsPerWord_ = 3;
    BitBoundary_ = 2;
    while (BitsValid_ < BitsPerWord_) {
      //uint8_t placeHolder = 
      decode<uint8_t>(LITTLE, n_byte_read + 44, buffer);
      //int nextByte = placeHolder & 0xff;
      //cout << "BitsRegister_ " << BitsRegister_ << endl;
      //BitsRegister_ |= (nextByte << BitsValid_);

      n_byte_read++;
      BitsValid_ += 8;
    }

    //BitsRegister_ >>= BitsPerWord_;
    //BitsValid_ -= BitsPerWord_;
    //cout << "NBYTES IN GETBITS: " << n_byte_read << endl;
    //cout << "lets see if this is running" << endl;

    return n_byte_read;
  }

  int DeltaCodec (const vector<char>& buffer, 
      vector<int>& out, 
      int size_of_vector,
      unsigned start)
  {
    out.resize(size_of_vector);
    //cout << "size of vector" << size_of_vector << endl;
    int value = 0;

    int BitsPerWord_ = 3;
    int BitBoundary_ = 2;

    int n_byte_read = start;

    for (int i = 0; i < size_of_vector; i++) {

      //getting delta from buffer
      char c = buffer[n_byte_read+i];
      //int delta = 0;

      //int delta = n_byte_read % 8;     
      int delta = c % 8;

      while (true) {

        n_byte_read += GetBits(buffer);
        //	cout << "NBYTES: " << n_byte_read << endl;
        if (delta == (1 << (BitsPerWord_-1))) {
          //  cout << "Delta: " << delta << endl;
          //cout << "BitsPerWord_ before shifting up: " << BitsPerWord_ << endl;
          BitsPerWord_ =  ShiftUp(BitsPerWord_);
          //cout << "BitsPerWord_ after shifting up: " <<  BitsPerWord_ << endl;
        }
        else {
          // valid delta
          cout << "VALID DELTA: " << delta << endl;
          break;
        }
      }

      // checking size
      if (abs(delta) < BitBoundary_) {
        BitsPerWord_ = ShiftDown(BitsPerWord_);
      }

      value += delta;

      // putting delta's into vector
      out[i] = value;
      cout << "OUT: " << value << endl;
    }

    return n_byte_read;
  }







  void dump_delta_compressed(unsigned start,
      const std::vector<char>& buffer,
      I3DOMLaunch& launch)
  {
    uint32_t word1 = decode<uint32_t>(LITTLE, start, buffer);
    uint32_t word2 = decode<uint32_t>(LITTLE, start + 4, buffer);
    uint32_t word3 = decode<uint32_t>(LITTLE, start + 8, buffer);

    unsigned compflag = ((word1 >> 31) & 0x01);
    cout << "Compflag:   " << compflag << endl;

    unsigned trigword = ((word1 >> 18) & 0x1F);
    cout << "Trigword:   " << trigword << endl;

    unsigned LC = ((word1 >> 16) & 0x3);
    cout << "LC bit :    " << LC << endl;

    unsigned fadcAvail = ((word1 >> 15) & 0x1);
    cout << "fadcAvail :  " << fadcAvail << endl;

    unsigned atwdAvail = ((word1 >> 14) & 0x1);
    cout << "atwdAvail :  " << atwdAvail << endl;

    unsigned atwdnch = ((word1 >> 12) & 0x3);
    cout << "atwdnch   :  " << atwdnch << endl;

    unsigned isAtwdB = ((word1 >> 11) & 0x1);
    cout << "isAtwdB   :  " << isAtwdB << endl;

    unsigned hitsize = (word1 & 0x3FF);
    cout << "hitsize   :  " << hitsize << endl;

    unsigned tlsb = word2;
    cout << "word2     :  " << tlsb << endl;

    unsigned peakrange = ((word3 >> 31) & 0x1);
    cout << "peakrange :  " << peakrange << endl;

    unsigned peaksamp = ((word3 >> 27) & 0x7);
    cout << "peaksamp  :  " << peaksamp << endl;

    unsigned prepeak = ((word3 >> 18) & 0x1F);
    cout << "prepeak   :  " << prepeak << endl;

    unsigned peakcnt = ((word3 >> 9) & 0x1FF);
    cout << "peakcnt   :  " << peakcnt << endl;

    unsigned postpeak = word3 & 0x1FF;
    cout << "postpeak  :  " << postpeak << endl;

    vector<int> out;

    DeltaCodec (buffer, out, hitsize, start + 12);
  }
};
