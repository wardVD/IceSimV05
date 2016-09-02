/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3FrameBufferDump.h
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
#ifndef I3FRAMEBUFFERDUMP_H_INCLUDED
#define I3FRAMEBUFFERDUMP_H_INCLUDED

// forward declarations

class I3Context;

// header files

#include <fstream>
#include <string>

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>

// definitions


/** Dump event data from I3 DAQ into a file. 
 *
 * I3FrameBufferDump supports two parameter:
 * <ul>
 * <li><VAR>BufferID</VAR> (the ID of the buffer to dump
 * into the outputfile (default value is "I3DAQData")) and
 * <li><VAR></VAR> (the name of the outputfile to create
 * (default value is "")).
 * </ul>
 */
class I3FrameBufferDump : public I3ConditionalModule
{
 public:
  I3FrameBufferDump(const I3Context& context);
  virtual ~I3FrameBufferDump();
  void Configure();
  void DAQ(I3FramePtr frame);
  void Finish();

 private:
  std::string outfile_;
  std::string frameBufferName_;
  boost::shared_ptr<std::ofstream> fout_;


  // private copy constructors and assignment
  I3FrameBufferDump(const I3FrameBufferDump&);
  I3FrameBufferDump& operator=(const I3FrameBufferDump&);


  // logging
  SET_LOGGER ("I3FrameBufferDump");
};

#endif
