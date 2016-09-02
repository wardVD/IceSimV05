#ifndef PAYLOAD_PARSING_UTILITY_H
#define PAYLOAD_PARSING_UTILITY_H

#include <vector>
#include <boost/utility.hpp>
#include <boost/detail/endian.hpp>

#include <icetray/I3Logging.h>


namespace payload_parsing
{

  enum Endian
  {
    BIG,
    LITTLE
  };

  /**
   * @brief a little function to swap the endianness of a particular 
   * type
   */
  template <class T>
    T endian_swap(T t)
    {
      T toReturn = 0;
      for(unsigned i = 0 ; i < sizeof(T) ; i++)
      {
        T shifted = t >> ((sizeof(T) - (i + 1)) * 8) & 0xFF;
        shifted = shifted << (i * 8);
        toReturn = toReturn | shifted;
      }
      return toReturn;
    }

  /**
   * @brief A small function which reads a type T out of a byte buffer
   * with a given starting position
   */
  template <class T>
    inline T decode(Endian e,size_t start, const std::vector<char>& eventData)
    {
      if(eventData.size() - start < sizeof(T) || start >= eventData.size())
        log_fatal("%zd bytes in the buffer and that's not enough to get a "
            "type of size %zd",eventData.size() - start,sizeof(T));

      T value;
      const T* arrayStart = 
        reinterpret_cast<const T*>(boost::addressof(eventData[start]));
      value = *arrayStart;

#if defined(BOOST_BIG_ENDIAN)
      if(e == LITTLE)
      {
        value = endian_swap(value);
        return value;
      }
      else if(e == BIG)
      {
        return value;
      }
      else
      {
        log_fatal("unknown endianess %d has been specified",e);
      }

#elif defined(BOOST_LITTLE_ENDIAN)
      if(e == LITTLE)
      {
        return value;
      }
      else if(e == BIG)
      {
        value = endian_swap(value);
        return value;
      }
      else
      {
        log_fatal("unknown endianess %d has been specified",e);
      }
#endif
    }  
};

#endif
