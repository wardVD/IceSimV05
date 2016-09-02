/*
* author: naoko
* april 19 2012
*
* module that distributes objects made on PnF clients that have format xxx_substreamnameN to the appropriate p-frame
* run after frames have been qconverted and re-trigger-split offline
* 
*/

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <icetray/I3Frame.h>
#include <icetray/I3FrameObject.h>
#include <icetray/I3Module.h>
#include <icetray/I3PacketModule.h>
#include <dataclasses/physics/I3EventHeader.h>

class DistributePnFObjects : public I3PacketModule {
  public:
    DistributePnFObjects(const I3Context& context);
    void Configure();
    void FramePacket(std::vector<I3FramePtr> &frames);
  private:
    std::vector<std::string> substream_names;
};

DistributePnFObjects::DistributePnFObjects(const I3Context& context) : 
  I3PacketModule(context)
{
  substream_names.push_back("NullSplit");
  substream_names.push_back("InIceSplit");
  substream_names.push_back("IceTopSplit");

  AddParameter("SubstreamNames", "list of the name of substreams", substream_names);

  AddOutBox("OutBox");
}

void DistributePnFObjects::Configure()
{
  GetParameter("SubstreamNames", substream_names);
}

void DistributePnFObjects::FramePacket(std::vector<I3FramePtr> &frames)
{

  std::vector<std::string> delete_keys;

  // loop stream names from argument
  for (std::vector<std::string>::const_iterator substream_name=substream_names.begin(); substream_name!=substream_names.end(); ++substream_name){

	// loop objects in q-frame
	for (I3Frame::typename_iterator iter = frames[0]->typename_begin(); iter!=frames[0]->typename_end(); ++iter) {        

	  const std::string& substream = *substream_name;
	  const std::string& key = iter->first;

	  size_t pos = key.find(substream);
	
          // did you find a frame object to move that's not DST?
	  if(pos != std::string::npos && key.find("I3DST") == std::string::npos) { 

	    unsigned int subeventid = boost::lexical_cast<unsigned int>(key.substr(pos + substream.length(),
	                                                                           key.size() - (pos + substream.length())));

	    std::string new_key = key.substr(0, pos - 1);

	    // loop substreams (p-frames) to get the right one
            bool foundPFrame = false;
	    for (std::vector<I3FramePtr>::const_iterator frame = frames.begin(); frame != frames.end(); ++frame){
    		const I3EventHeader& ehnow = (*frame)->Get<I3EventHeader>();
    		const std::string& ssnow = ehnow.GetSubEventStream();
    		unsigned int idnow = ehnow.GetSubEventID();

		if (ssnow == substream && idnow == subeventid) {
                    if (foundPFrame)
                        log_error("P-frame split sub_event_ids are not unique for run %u, event %u, split %s/%u.",
                                  ehnow.GetRunID(), ehnow.GetEventID(), substream.c_str(), idnow);
                    (*frame)->take(*(frames[0]), key, new_key);
                    (*frame)->ChangeStream(new_key, (*frame)->GetStop());
                    foundPFrame = true;
                }
	    }

            if (foundPFrame) {
	         delete_keys.push_back(key);
            } else {
                log_warn("Event does not have a SubEventStream %s with ID %i. Object was not distributed from Q-frame.",
                         substream.c_str(), subeventid);
            }
	  }
	}
  }

  // delete objects that were copied from the q-frame
  for (std::vector<std::string>::const_iterator key=delete_keys.begin(); key!=delete_keys.end(); ++key) {
        frames[0]->Delete(*key);
  }

  // push all frames
  for (std::vector<I3FramePtr>::const_iterator frame = frames.begin(); frame != frames.end(); ++frame) {
        PushFrame(*frame);
  }

}

I3_MODULE(DistributePnFObjects);
