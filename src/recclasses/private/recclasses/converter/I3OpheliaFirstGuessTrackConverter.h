#ifndef I3OPHELIAFIRSTGUESSTRACKCONVERTER_H_INCLUDED
#define I3OPHELIAFIRSTGUESSTRACKCONVERTER_H_INCLUDED

#include <tableio/I3Converter.h>
#include "recclasses/I3OpheliaFirstGuessTrack.h"

/// Converter for I3OpheliaFirstGuessTrack
class I3OpheliaFirstGuessTrackConverter : public I3ConverterImplementation<I3OpheliaFirstGuessTrack> 
{
 private:
  I3TableRowDescriptionPtr CreateDescription(const I3OpheliaFirstGuessTrack &fgtrack); 
  size_t FillRows(const I3OpheliaFirstGuessTrack &fgtrack, I3TableRowPtr rows);
};

#endif  // I3OPHELIAFIRSTGUESSTRACKCONVERTER_H_INCLUDED
