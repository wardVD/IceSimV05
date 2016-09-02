/*
* author: naoko
* jan 16 2012 @ southpole
* copy frame object to keep from p-frame to q-frame and name them with identifier
*/

#include <icetray/I3Frame.h>
#include <icetray/I3FrameObject.h>
#include <icetray/I3PacketModule.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "dataclasses/physics/I3EventHeader.h"

class KeepFromSubstream : public I3PacketModule {
  public:
    KeepFromSubstream(const I3Context& context);
    void Configure();
    void FramePacket(std::vector<I3FramePtr> &frames);
  private:
    std::string stream_name_;
    std::vector<std::string> keep_keys_;
    bool keep_stream_;
};

KeepFromSubstream::KeepFromSubstream(const I3Context& context) : 
  I3PacketModule(context, I3Frame::DAQ)
{
  keep_stream_ = false;

  AddOutBox("OutBox");
  AddParameter("StreamName","Name of the subevent stream",stream_name_);
  AddParameter("KeepKeys","List of key names to keep",keep_keys_);
  AddParameter("KeepStream","Boolean, keep the substream P-frames?",keep_stream_);
}

void KeepFromSubstream::Configure()
{
  GetParameter("StreamName",stream_name_);
  GetParameter("KeepKeys",keep_keys_);
  GetParameter("KeepStream",keep_stream_);

  if (keep_keys_.empty()||stream_name_.empty())
    log_fatal("Must specify stream and at least 1 key to keep");

}

void KeepFromSubstream::FramePacket(std::vector<I3FramePtr> &frames)
{
  int framenum = 0;
  //loop over each frame
  for (std::vector<I3FramePtr>::const_iterator frame = frames.begin(); frame != frames.end(); frame++) {

    //if the frame is from the selected stream
    I3EventHeaderConstPtr ehnow = (*frame)->Get<I3EventHeaderConstPtr>("I3EventHeader");
    if (!ehnow) {
      log_fatal("No I3EventHeader in frame!");
      continue;
    }

    //I3EventHeader ehnow = (*frame)->Get("I3EventHeader");
    std::string   ssnow = ehnow->GetSubEventStream();
    if(ssnow == stream_name_){  

      //loop over each keep keys
      for (std::vector<std::string>::iterator key = keep_keys_.begin(); key != keep_keys_.end(); key++ ) {
        I3FrameObjectConstPtr object = (*frame)->Get<I3FrameObjectConstPtr>(*key);
	//see if it matches
	if (object){
  	    //put the result in the frame
	    //char buf[3];
	    //sprintf(buf,"%d",framenum);
	    std::string outkey;
	    //outkey = *key + '_' + stream_name_ + buf;
	    outkey = *key + '_' + stream_name_ + boost::lexical_cast<std::string>(framenum);
  	    frames[0]->Put(outkey, object);
	}
      }
      framenum++;
    }
  }
  
  //push frame
  for (std::vector<I3FramePtr>::const_iterator frame = frames.begin(); frame != frames.end(); frame++) {
    I3EventHeaderConstPtr ehnow = (*frame)->Get<I3EventHeaderConstPtr>("I3EventHeader");
    if (!ehnow) log_fatal("No I3EventHeader in frame!");

    std::string   ssnow = ehnow->GetSubEventStream();
    if(ssnow == stream_name_){  
	if(keep_stream_) PushFrame(*frame);
    } else {
	PushFrame(*frame);
    }
  }
}

I3_MODULE(KeepFromSubstream);
