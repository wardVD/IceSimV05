#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3FilterResult.h>
#include <icetray/I3Bool.h>
#include <filter-tools/CreateFilterMask.h>

CreateFilterMask::CreateFilterMask(const I3Context& context) : 
  I3Module(context),
  output_name_("FilterMask")
{
  AddOutBox("OutBox");
  AddParameter("OutputName","Name to give the I3FilterResultMap object in the frame",output_name_);
}

void CreateFilterMask::Configure()
{
  GetParameter("OutputName",output_name_);
}

void CreateFilterMask::Physics(I3FramePtr frame)
{
  //create new filter result
  I3FilterResultMapPtr resultmap= I3FilterResultMapPtr ( new I3FilterResultMap());
 
  //loop over each object in the frame
  for (I3Frame::const_iterator itr=frame->begin();itr!=frame->end();itr++)
    {
      //see if it is an I3Bool
      try{

	I3BoolConstPtr filter=boost::dynamic_pointer_cast< const I3Bool >(itr->second);
	if ( filter )
	  {	  
	    //add value of bool to map with the name of the I3Bool as the name of the filter
	    (*resultmap)[itr->first]=I3FilterResult();
	    (*resultmap)[itr->first].conditionPassed=filter->value;
	    (*resultmap)[itr->first].prescalePassed=filter->value;   
	  }
	}
      catch (const std::exception& e) {
	//do nothing
      }
    }
  
  //put the result in the frame and push
  frame->Put(output_name_, resultmap);
  PushFrame(frame,"OutBox");
  
}

void CreateFilterMask::Finish()
{
}

I3_MODULE(CreateFilterMask);
