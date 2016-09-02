#ifndef RECCLASSES_I3PORTIAEVENTCONVERTER_H
#define RECCLASSES_I3PORTIAEVENTCONVERTER_H

#include "recclasses/I3PortiaEvent.h"
#include <tableio/I3Converter.h>

/// Converter for I3PortiaEvent
class I3PortiaEventConverter : public I3ConverterImplementation<I3PortiaEvent> 
{
 private:
  I3TableRowDescriptionPtr CreateDescription(const I3PortiaEvent &evt); 
  size_t FillRows(const I3PortiaEvent &evt, I3TableRowPtr rows);
};

#endif
