#ifndef DUMPRAWENGINEERING_H
#define DUMPRAWENGINEERING_H

#include <vector>


class I3DOMLaunch;


namespace payload_parsing
{
  void dump_raw_engineering(unsigned start,
      const std::vector<char>& buffer,
      I3DOMLaunch& launch);
};

#endif
