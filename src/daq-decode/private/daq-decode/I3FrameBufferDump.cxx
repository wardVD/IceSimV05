/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3FrameBufferDump.cxx 142134 2016-02-19 19:00:15Z tschmidt $
 *
 * @file I3FrameBufferDump.cxx
 * @version $Revision: 142134 $
 * @date $Date: 2016-02-19 13:00:15 -0600 (Fri, 19 Feb 2016) $
 * @author tschmidt
 */

// class header files

#include <daq-decode/I3FrameBufferDump.h>
I3_MODULE(I3FrameBufferDump);

#include <dataclasses/I3Vector.h>
#include <icetray/I3Frame.h>

// namespace declarations


// implementation

I3FrameBufferDump::I3FrameBufferDump(const I3Context& context) 
  : I3ConditionalModule(context),
    frameBufferName_("I3DAQData")
{
  AddParameter("Outfile",
               "The name of the outputfile to create",
               outfile_);
  AddParameter("BufferID",
               "The ID of the buffer to dump into the outputfile",
               frameBufferName_);

  AddOutBox("OutBox");
}

I3FrameBufferDump::~I3FrameBufferDump()
{
}


void I3FrameBufferDump::Configure()
{
  GetParameter("Outfile", outfile_);
  GetParameter("BufferID", frameBufferName_);

  fout_ = boost::shared_ptr<std::ofstream>(new std::ofstream(outfile_.c_str()));
  
  if(!(*fout_))
    log_fatal("cannot open file %s", outfile_.c_str());
}


void I3FrameBufferDump::DAQ(I3FramePtr frame)
{
  I3VectorCharConstPtr data = frame->Get<I3VectorCharConstPtr>(frameBufferName_);

  if(data && (data->size() > 0))
  {
    const char* rawdata = boost::addressof((*data)[0]);
      
    fout_->write(rawdata, data->size());
  }

  PushFrame(frame, "OutBox");
}


void I3FrameBufferDump::Finish()
{
  if(fout_)
    fout_->close();
}
