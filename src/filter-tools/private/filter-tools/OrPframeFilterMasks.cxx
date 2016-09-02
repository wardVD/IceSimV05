/*
* author: naoko
* dec 5 2011
* make new FilterMask in q-frame as an 'or' of all filtermasks in p-frames
*/

#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3FilterResult.h>
#include <filter-tools/CreateFilterMask.h>
#include <icetray/I3PacketModule.h>
#include <boost/foreach.hpp>


class OrPframeFilterMasks : public I3PacketModule {
  public:
    OrPframeFilterMasks(const I3Context& context);
    void Configure();
    void FramePacket(std::vector<I3FramePtr> &frames);
  private:
    std::string output_name_;
    std::string input_name_;
};

OrPframeFilterMasks::OrPframeFilterMasks(const I3Context& context) : 
  I3PacketModule(context, I3Frame::DAQ),
  output_name_("QFilterMask"),
  input_name_("FilterMask")
{
  AddOutBox("OutBox");
  AddParameter("OutputName","Name to give the I3FilterResultMap object in the frame",output_name_);
  AddParameter("InputName","Name of I3FilterResultMap object to collect in the p-frames",input_name_);
}

void OrPframeFilterMasks::Configure()
{
  GetParameter("OutputName",output_name_);
  GetParameter("InputName",input_name_);

  if(output_name_ == input_name_) log_warn("INPUT and OUTPUT the same name. You will NOT be able to see Q-frame filtermask from P-frames");
}

void OrPframeFilterMasks::FramePacket(std::vector<I3FramePtr> &frames)
{
  //create new filter result
  I3FilterResultMapPtr resultmap= I3FilterResultMapPtr ( new I3FilterResultMap());
 
  //loop over each frame
  for (std::vector<I3FramePtr>::const_iterator frame = frames.begin(); frame != frames.end(); frame++) {
      I3FilterResultMapConstPtr filter_result_map=(*frame)->Get<I3FilterResultMapConstPtr>(input_name_);

      if ( filter_result_map ) {
        for (I3FilterResultMap::const_iterator filter_result=filter_result_map->begin(); filter_result != filter_result_map->end(); filter_result++) {
	  std::string name = filter_result->first;

          //add value to map with the name of the I3Bool as the name of the filter
	  // || is not a problem since initialized to false
          (*resultmap)[name].conditionPassed=filter_result->second.conditionPassed || (*resultmap)[name].conditionPassed;
          (*resultmap)[name].prescalePassed=filter_result->second.prescalePassed || (*resultmap)[name].prescalePassed;
	}
      }
  }
  
  //put the result in the frame and push
  if (frames[0]->Has(output_name_)) frames[0]->Delete(output_name_);
  frames[0]->Put(output_name_, resultmap);
  for (std::vector<I3FramePtr>::const_iterator frame = frames.begin(); frame != frames.end(); frame++) PushFrame(*frame);
  
}

I3_MODULE(OrPframeFilterMasks);
