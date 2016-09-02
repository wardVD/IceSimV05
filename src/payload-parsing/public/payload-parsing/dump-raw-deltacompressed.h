#ifndef DUMPRAWDELTACOMPRESSED_H
#define DUMPRAWDELTACOMPRESSED_H

#include <vector>
#include <payload-parsing/utility.h>
#include <payload-parsing/decode.h>

class I3DOMLaunch;

namespace payload_parsing
{
  void dump_delta_compressed(unsigned start,
      const std::vector<char>& buffer,
      I3DOMLaunch& launch);

  int DeltaCodec (const std::vector<char>& buffer,
      std::vector<int>& out,
      int size_of_vector);
}
#endif
