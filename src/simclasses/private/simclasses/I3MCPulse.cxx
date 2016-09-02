#include <simclasses/I3MCPulse.h>
#include <ostream>
#include "I3SequenceOpOStream.h"

I3_SERIALIZABLE(I3MCPulseSeriesMap);

std::ostream& operator<<(std::ostream& os, const I3MCPulse& pulse) {
    os << "[ I3MCPulse::"
       << "\n  Time   :" << pulse.time 
       << "\n  Charge :" << pulse.charge
       << "\n  Source :" << pulse.source << " ]";
    return os;
}

I3_SEQUENCE_OP_OSTREAM(I3MCPulseSeries, "\n ");
