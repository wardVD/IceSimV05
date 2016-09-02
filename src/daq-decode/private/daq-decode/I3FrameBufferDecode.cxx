/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3FrameBufferDecode.cxx 94949 2012-11-04 16:40:30Z nwhitehorn $
 *
 * @file I3FrameBufferDecode.cxx
 * @version $Revision: 94949 $
 * @date $Date: 2012-11-04 17:40:30 +0100 (So, 04 Nov 2012) $
 * @author tschmidt
 */

// class header files

#include <daq-decode/I3FrameBufferDecode.h>
I3_MODULE(I3FrameBufferDecode);

#include <exception>

#include <icetray/I3Context.h>
#include <icetray/I3DefaultName.h>
#include <icetray/I3Bool.h>
#include <dataclasses/I3Time.h>
#include <dataclasses/I3Vector.h>

// namespace declarations

using namespace std;

// implementation

I3FrameBufferDecode::I3FrameBufferDecode(const I3Context& context) 
  : I3ConditionalModule(context),
    bufferID_("I3DAQData"),
    exceptionID_("I3DAQDecodeException")
{
  AddParameter("BufferID",
               "The ID of the buffer to decode in the frame",
               bufferID_);
  AddParameter("ExceptionID",
               "This boolean indicates any exception that is thrown during decoding",
               exceptionID_);

  AddOutBox("OutBox");
}


I3FrameBufferDecode::~I3FrameBufferDecode()
{
}


void I3FrameBufferDecode::Configure()
{
  GetParameter("BufferID", bufferID_);
  GetParameter("ExceptionID", exceptionID_);
  if(!(decoder_ = context_.Get<I3DAQEventDecoderPtr>()))
    log_fatal("context contains nothing at slot \"%s\"",
              I3DefaultName<I3DAQEventDecoderPtr>::value());
}


void I3FrameBufferDecode::DAQ(I3FramePtr frame)
{
  I3VectorCharConstPtr buffer = frame->Get<I3VectorCharConstPtr>(bufferID_);
  //  JEBEventInfo was from the TWR+I# days.  that ship has sailed....
  //JEBEventInfoConstPtr eventInfo = frame->Get<JEBEventInfoConstPtr>();
  
  if(!buffer) // this doesn't look like daq data ... just proceed
    PushFrame(frame, "OutBox");
  else
  {
    I3BoolPtr caught(new I3Bool(true));
    I3FramePtr newFrame(new I3Frame(frame->GetStop()));
    newFrame->drop_blobs(frame->drop_blobs());

    try
    {
      decoder_->FillEvent(*newFrame, *buffer);
      caught->value = false;
    }
    catch(exception& ex)
    {
      if(!exceptionID_.empty())
        log_warn("catching standard exception: \"%s\"", ex.what());
      else throw;
    }
    catch(...)
    {
      if(!exceptionID_.empty())
        log_warn("catching unexpected exception");
      else throw;
    }
  
    if(caught->value) newFrame->Put(exceptionID_, caught);
    
    string name;
    for(I3Frame::typename_iterator iter = frame->typename_begin();
        iter != frame->typename_end();
        ++iter)
    {
      name = iter->first;
      if(newFrame->count(name))
        newFrame->take(*frame, name, name + "_orig");
      else
        newFrame->take(*frame, name);
    }
  
    PushFrame(newFrame, "OutBox");
  }
}
