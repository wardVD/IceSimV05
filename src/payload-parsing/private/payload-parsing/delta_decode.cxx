// Here we decode the actual delta compressed waveforms

#include <payload-parsing/delta_decode.h>
#include <payload-parsing/utility.h>
#include <vector>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <payload-parsing/utility.h>


using namespace std;


namespace payload_parsing
{
  I3DOMLaunch::TriggerType eventTrigTypeToDOMLaunchDelta(int trigword)
  {
    if (trigword & 0x0) {
      return I3DOMLaunch::TEST_PATTERN;
    }
    else if (trigword & 0x4) {
      return I3DOMLaunch::CPU_REQUESTED;
    }
    else if (trigword & 0x2) {
      return I3DOMLaunch::MPE_DISCRIMINATOR_TRIGGER;
    }
    else if (trigword & 0x1) {
      return I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER;
    }
    else if (trigword & 0x20) {
      return I3DOMLaunch::FLASHER_BOARD_TRIGGER;
    }
    else if (trigword & 0x10) {
      return I3DOMLaunch::ONBOARD_LED;
    }
    log_fatal("unknown trigger type %d", trigword);
  }

  void delta_decode(unsigned start,
      const std::vector<char>& buffer,
      I3DOMLaunch& launch,
      unsigned WhichChargeStamp)
  {

    uint32_t word1 = decode<uint32_t>(BIG, start + 50, buffer);
    uint32_t word3 = decode<uint32_t>(BIG, start + 54, buffer);

    unsigned compflag = ((word1 >> 31) & 0x01);
    if(compflag == 0)
      log_fatal("compflag != 1, indicating that "
          "this is not a delta compressed "
          "waveform, though thats what we "
          "are expecting");

    unsigned trigword = ((word1 >> 18) & 0x3ff);

    I3DOMLaunch::TriggerType triggertype = eventTrigTypeToDOMLaunchDelta(trigword);

    launch.SetTriggerType(triggertype);

    //bool lcup = trigword & (0x100);
    //bool lcdown = trigword & (0x200);
    bool fbrun = trigword & (0x20);
    unsigned LC = ((word1 >> 16) & 0x3); 

    int mode = 0;
    if(LC == 2) 
      mode += I3DOMLaunch::LC_UPPER;
    if(LC == 1)
      mode += I3DOMLaunch::LC_LOWER;
    if(fbrun)
      mode += I3DOMLaunch::FR_RUN;
    if(LC == 3)
    {
      //  mode += I3DOMLaunch::REQUIRE_BOTH;
      mode += I3DOMLaunch::LC_UPPER;
      mode += I3DOMLaunch::LC_LOWER;
    }

    unsigned Min_Bias = ((word1 >> 30) & 0x01);
    if (Min_Bias == 1)
    {
      mode += I3DOMLaunch::MIN_BIAS;
    }

    //	 cout << "LC bit :    " << LC << endl;

    unsigned fadcAvail = ((word1 >> 15) & 0x1);
    //    cout << "fadcAvail :  " << fadcAvail << endl;

    unsigned atwdAvail = ((word1 >> 14) & 0x1);
    //    cout << "atwdAvail :  " << atwdAvail << endl;

    unsigned atwdnch = ((word1 >> 12) & 0x3);
    //    cout << "atwdnch   :  " << atwdnch << endl;

    unsigned isAtwdB = ((word1 >> 11) & 0x1);

    int setATWD;

    if (isAtwdB == 1) {
      setATWD = I3DOMLaunch::ATWDb;
    }
    else if (isAtwdB == 0) {
      setATWD = I3DOMLaunch::ATWDa;
    }
    else {
      log_fatal("don't know which atwd!!");
    }

    launch.SetWhichATWD(static_cast<I3DOMLaunch::ATWDselect>(setATWD));
    //cout << "isAtwdB   :  " << isAtwdB << endl;

    unsigned hitsize = (word1 & 0x7FF);
    // cout << "hitsize   :  " << hitsize << endl;

    // Handling InIceCharge Stamp
    vector<int> decodedRawChargeStamp;
    decodedRawChargeStamp.resize(1, 0);
    if (WhichChargeStamp == 0)
    {
      //Peak Range
      unsigned peakrange = ((word3 >> 31) & 1);
      //cout << "peakrange : " << peakrange << endl;

      //This is the Charge Stamp Highest Sample... in dataclasses
      unsigned peaksamp = ((word3 >> 27) & 0xf);
      //cout << "peaksamp  :  " << peaksamp << endl;
      //assert(peaksamp < 16);

      unsigned prepeak = 0;
      unsigned peakcnt = 0;
      unsigned postpeak = 0;

      if(peakrange == 0)
      {
        prepeak = ((word3 >> 18) & 0x1ff);
        //cout << "prepeak   :  " << prepeak << endl;
        peakcnt = ((word3 >> 9) & 0x1ff);
        //cout << "peakcnt   :  " << peakcnt << endl;
        postpeak = word3 & 0x1ff ;
        //cout << "postpeak  :  " << postpeak << endl;
      }
      else if(peakrange == 1)
      {
        prepeak = ((word3 >> 18) & 0x1ff) << 1;
        //cout << "prepeak   :  " << prepeak << endl;
        peakcnt = ((word3 >> 9) & 0x1ff) << 1;
        //cout << "peakcnt   :  " << peakcnt << endl;
        postpeak = ((word3 >> 0)  & 0x1ff) << 1;
        //cout << "postpeak  :  " << postpeak << endl;
      }
      else
      {
        log_fatal("the peak range has to be either 1 or 2");
      }

      // peaksamp is an unisgned int, so this would never fail anyway:
      //if (peaksamp < 0)
      //  {
      //    log_fatal("Peak sample should be greater than 0");
      //  }
      decodedRawChargeStamp.resize(3, 0);
      decodedRawChargeStamp[0] = prepeak;
      decodedRawChargeStamp[1] = peakcnt;
      decodedRawChargeStamp[2] = postpeak;
      launch.SetChargeStampHighestSample(peaksamp);
      launch.SetRawATWDChargeStamp(0);
      launch.SetWhichATWDChargeStamp(0);
    }
    else if(WhichChargeStamp == 1)
    {
      //cout << "Doing some IceTop Charge Stamp decoding" << endl;
      unsigned IceTopChargeStamp = 0;
      unsigned Channel;
      IceTopChargeStamp = ((word3 >> 0) & 0xffff);
      Channel = ((word3 >> 17) & 0x3);
      launch.SetRawATWDChargeStamp(IceTopChargeStamp);
      launch.SetWhichATWDChargeStamp(Channel);
      launch.SetChargeStampHighestSample(0);
      //cout << IceTopChargeStamp << "  " << Channel << "  " << endl;;
    }
    else if((WhichChargeStamp != 0) | (WhichChargeStamp != 1))
    {
      log_fatal("The Charge Stamp has to be either InIce or IceTop...");
    }

    bitbuffer bits(buffer, start + 8 + 50, hitsize - 8);

    unsigned bitsPerWord = 3;

    int accummulatedWaveform = 0;
    size_t waveformBin = 0;

    vector<int> decodedWaveform;
    if(fadcAvail && atwdAvail)
    {
      launch.SetLCBit(true);
      switch(atwdnch)
      {
        case 0:
          decodedWaveform.resize(256 + 128, 0);
          break;
        case 1:
          decodedWaveform.resize(256 + 128 + 128, 0);
          break;
        case 2:
          decodedWaveform.resize(256 + 128 + 128 + 128, 0);
          break;
        case 3:
          decodedWaveform.resize(256 + 128 + 128 + 128 + 128, 0);
          break;
      }
    }
    else if(fadcAvail && !atwdAvail)
    {
      launch.SetLCBit(true);
      decodedWaveform.resize(256, 0);
    }
    else if(!fadcAvail && !atwdAvail)
    {
      launch.SetLCBit(false);
      mode += I3DOMLaunch::SLC_READOUT;
      //log_error("TriggerType is SLC... I3DOMLaunch.h needs to be updated");
    }

    I3DOMLaunch::TriggerMode triggermode = 
      static_cast<I3DOMLaunch::TriggerMode>(mode);

    launch.SetTriggerMode(triggermode);

    unsigned bitsRetrieved = 0;

    //cout<<"bitsPopped for call "<<calls<<endl;
    if(fadcAvail || atwdAvail){
      while(bits.remainingBits() - bitsPerWord > 0 && 
          waveformBin < decodedWaveform.size())
      {         
        delta d = bits.pop(bitsPerWord);

        //cout<<"bitsRetrieved:"<<bitsRetrieved<<endl;
        //cout<<"bitsPopped:"<<bitsPerWord<<endl;

        bitsRetrieved += bitsPerWord;
        //dumpbits(d);
        if(d.isFlag())
        {
          //cout<<"found a flag and so switching bitsPerWord "
          //<<"was originally "<<bitsPerWord<<" and is now ";
          switch(bitsPerWord)
          {
            case 1:
              bitsPerWord = 2;
              break;
            case 2:
              bitsPerWord = 3;
              break;
            case 3:
              bitsPerWord = 6;
              break;
            case 6:
              bitsPerWord = 11;
              break;
            default:
              log_fatal("wrong 'bitsPerWord', code error %d", bitsPerWord);
          }
          //cout<<bitsPerWord<<endl;
        }
        else
        {
          accummulatedWaveform += d.getValue();
          decodedWaveform[waveformBin] = accummulatedWaveform;
          //cout<<"waveform["<<waveformBin<<"]="<<accummulatedWaveform<<endl;
          waveformBin += 1;
          if(d.wastedBits())
          {
            switch(bitsPerWord)
            {
              case 2:
                bitsPerWord = 1;
                break;
              case 3:
                bitsPerWord = 2;
                break;
              case 6:
                bitsPerWord = 3;
                break;
              case 11:
                bitsPerWord = 6;
                break;
              default:
                log_fatal("wrong 'bitsPerWord', code error %d",bitsPerWord);
            }
          }

          if(waveformBin == 256 || 
              waveformBin == 256 + 128 ||
              waveformBin == 256 + 128 + 128 ||
              waveformBin == 256 + 128 + 128 + 128)
          {
            accummulatedWaveform =0;
            bitsPerWord = 3;
          }

        }
      }
    }

    // putting the RawChargeStamp into the DOMLaunch
    if(decodedRawChargeStamp.size() == 3)
    {
      int size = decodedRawChargeStamp.size();
      launch.GetRawChargeStamp().resize(size);
      copy(&(decodedRawChargeStamp[0]), &(decodedRawChargeStamp[size]),
          launch.GetRawChargeStamp().begin());
    }


    // now extract the waveform into the DOMLaunch 
    if(decodedWaveform.size() >= 256)
    {
      launch.GetRawFADC().resize(256);
      copy(&(decodedWaveform[0]),
          &(decodedWaveform[256]),
          launch.GetRawFADC().begin());
    }

    if(decodedWaveform.size() >= 128 + 256)
    {
      launch.GetRawATWD(0).resize(128);

      copy(&(decodedWaveform[256]),
          &(decodedWaveform[128 + 256]),
          launch.GetRawATWD(0).begin());
      // reversing waveforms so that the timing is right...
      reverse(launch.GetRawATWD(0).begin(), launch.GetRawATWD(0).end());

    }

    if(decodedWaveform.size() >= 128 + 128 + 256)
    {
      launch.GetRawATWD(1).resize(128);
      copy(&(decodedWaveform[128 + 256]),
          &(decodedWaveform[128 + 128 + 256]),
          launch.GetRawATWD(1).begin());
      // reversing waveforms so that the timing is right...
      reverse(launch.GetRawATWD(1).begin(), launch.GetRawATWD(1).end());
    }

    if(decodedWaveform.size() >= 128 + 128 + 128 + 256)
    {
      launch.GetRawATWD(2).resize(128);
      copy(&(decodedWaveform[128 + 128 + 256]),
          &(decodedWaveform[128 + 128 + 128 +256]),
          launch.GetRawATWD(2).begin());
      // reversing waveforms so that the timing is right...
      reverse(launch.GetRawATWD(2).begin(), launch.GetRawATWD(2).end());
    } 
  }
};
