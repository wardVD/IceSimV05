// This is the actual waveform decoder for engineering format waveforms

#include <payload-parsing/dump-raw-engineering.h>
#include <payload-parsing/utility.h>
#include <dataclasses/physics/I3DOMLaunch.h>

using namespace std;


namespace payload_parsing
{
  I3DOMLaunch::TriggerType eventTrigTypeToDOMLaunch(int v)
  {
    if (v == 0) {
      return I3DOMLaunch::TEST_PATTERN;
    }
    else if (v == 1) {
      return I3DOMLaunch::CPU_REQUESTED;
    }
    else if (v == 2) {
      return I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER;
    }
    else if (v == 3) {
      return I3DOMLaunch::FLASHER_BOARD_TRIGGER;
    }
    log_fatal("unknown trigger type %d", v);
  }
};

using namespace payload_parsing;


void payload_parsing::dump_raw_engineering(unsigned start,
    const vector<char>& buffer,
    I3DOMLaunch& launch)
{

  log_trace("found engineereing data ... ");

  int flen = 0;
  const unsigned char* buf = (const unsigned char*) &(buffer[0 + start]);

  unsigned char atwd[4];
  atwd[0] = buf[6] & 0xf;
  atwd[1] = buf[6] >> 4;
  atwd[2] = buf[7] & 0xf;
  atwd[3] = buf[7] >> 4; 

  unsigned char tbyte = buf[8];

  char ttype = tbyte & 0x0F;
  //  unsigned char mbyte;

  bool lcup = tbyte & (1 << 6);
  bool lcdown = tbyte & (1 << 5);
  bool fbrun = tbyte & (1 << 4);
  bool lcrequireboth = tbyte &(1 << 3);

  int mode = 0;
  if(lcup) mode += I3DOMLaunch::LC_UPPER;
  if(lcdown) mode += I3DOMLaunch::LC_LOWER;
  if(fbrun) mode += I3DOMLaunch::FR_RUN;
  if(lcrequireboth) mode += I3DOMLaunch::REQUIRE_BOTH;

  I3DOMLaunch::TriggerMode triggermode = 
    static_cast<I3DOMLaunch::TriggerMode>(mode);

  I3DOMLaunch::TriggerType triggertype = eventTrigTypeToDOMLaunch(ttype);

  launch.SetTriggerType(triggertype);

  //this needs to be looked at! I don't know if that is the LCBit...

  launch.SetLCBit(ttype);

  launch.SetTriggerMode(triggermode);

  //which atwd:
  uint8_t miscbyte = 
    decode<uint8_t>(BIG, start + 4, buffer);

  char miscbit;

  miscbit = (miscbyte & (1<<0));

  int setATWD;

  if (miscbit == 1) {
    setATWD = I3DOMLaunch::ATWDb;
  }
  else if (miscbit == 0) {
    setATWD = I3DOMLaunch::ATWDa;
  }
  else {
    log_fatal("don't know which atwd!!");
  }

  launch.SetWhichATWD(static_cast<I3DOMLaunch::ATWDselect>(setATWD));

  // still need to find the right byte(s) for this... question for DAQ.
  uint16_t chargeStampHighestSample =
    decode<uint16_t>(BIG, start + 2, buffer);

  launch.SetChargeStampHighestSample(chargeStampHighestSample);

  flen = buf[5];

  int idx = 16;

  for (int i=0; i<flen; i++) {
    std::vector<int>& waveform = launch.GetRawFADC();
    waveform.resize(flen);

    uint16_t fadcbuffer =
      decode<uint16_t>(BIG, idx + start, buffer);

    waveform[i] = fadcbuffer;
    idx+=2;
  }
  for (int i = 0; i < 4; i++) {
    std::vector<int>& waveform = launch.GetRawATWD(i);
    int np[] = { 32, 64, 16, 128 };
    const int n = np[ (atwd[i] & 0xc) >> 2 ];
    if ( (atwd[i] & 1) == 0) continue;

    waveform.resize(n);     

    if (atwd[i] & 2) {
      /* short ... */
      for (int j = 0; j < n; j++) {

        uint16_t atwdbuffer = 
          decode<uint16_t>(BIG, idx + start, buffer);

        waveform[j] = atwdbuffer;
        idx += 2;
      } 
      reverse(waveform.begin(), waveform.end());
    }
    else {
      // unsigned char...
      log_fatal("atwd format not as expected");
    }
  }
};
