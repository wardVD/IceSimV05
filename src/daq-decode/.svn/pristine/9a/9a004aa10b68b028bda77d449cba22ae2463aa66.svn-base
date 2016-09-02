/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3DAQEventDecoder.h
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
#ifndef I3_DAQ_EVENT_DECODER_H_INCLUDED
#define I3_DAQ_EVENT_DECODER_H_INCLUDED

// forward declarations

class I3Time;
class I3Frame;

// header files

#include <vector>

#include <icetray/I3Logging.h>
#include <icetray/I3DefaultName.h>
#include <icetray/I3FrameObject.h>

// definitions


/**
 * @brief Abstract base class that defines an interface for a decoder
 * that decodes the I3 DAQ event byte stream and fills dataclasses structures.
 */
class I3DAQEventDecoder
{
public:
  /**
   * @brief Default constructor.
   */
  I3DAQEventDecoder();
  /**
   * @brief Destructor.
   */
  virtual ~I3DAQEventDecoder() = 0;
  /**
   * @brief Decodes a DAQ event byte stream and fills dataclasses structures.
   * 
   * @param frame Container for the created dataclasses structures.
   * @param data DAQ event byte stream.
   * @return The time of the event.
   */
  virtual I3Time FillEvent(I3Frame& frame,
                           const std::vector<char>& data) const = 0;

private:
  // private copy constructors and assignment
  I3DAQEventDecoder(const I3DAQEventDecoder &);
  I3DAQEventDecoder & operator=(const I3DAQEventDecoder &);


  // instance member data


  // logging
  SET_LOGGER("I3DAQEventDecoder");
};

I3_POINTER_TYPEDEFS(I3DAQEventDecoder);
I3_DEFAULT_NAME(I3DAQEventDecoder);

#endif /* I3_DAQ_EVENT_DECODER_H_INCLUDED */
