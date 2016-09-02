/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id: I3DOMLaunchCleaning.cxx 142504 2016-02-25 20:36:55Z olivas $
 */
#include "DomTools/I3DOMLaunchCleaning.h"

#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/I3Vector.h>
#include <icetray/I3Frame.h>
#include <algorithm>

I3_MODULE(I3DOMLaunchCleaning);

I3DOMLaunchCleaning::I3DOMLaunchCleaning(const I3Context& context) 
  : I3ConditionalModule(context),
    cleanedKeysList_(""),
    inIceInput_("InIceRawData"),
    iceTopInput_("IceTopRawData"),
    inIceOutput_("CleanInIceRawData"),
    iceTopOutput_("CleanIceTopRawData"),
    firstLaunchCleaning_(false)
{
  AddOutBox("OutBox");

  AddParameter("CleanedKeys",
	       "OMKeys to clean out of the launch map",
	       cleanedKeys_);

  AddParameter("CleanedKeysList",
	       "Name of frame vector containing the list of OMKeys to clean out of the launch map",
	       cleanedKeysList_);

  AddParameter("InIceInput",
	       "input inice DOMLaunches",
	       inIceInput_);

  AddParameter("InIceOutput",
	       "output inice DOMLaunches",
	       inIceOutput_);

  AddParameter("IceTopInput",
	       "input icetop DOMLaunches",
	       iceTopInput_);

  AddParameter("IceTopOutput",
	       "output icetop DOMLaunches",
	       iceTopOutput_);

  AddParameter("FirstLaunchCleaning",
	       "True if you want to clean out all but the first DOM launch",
	       firstLaunchCleaning_);
}

void I3DOMLaunchCleaning::Configure()
{
  GetParameter("CleanedKeys",
	       cleanedKeys_);

  GetParameter("CleanedKeysList",
	       cleanedKeysList_);

  GetParameter("InIceInput",
	       inIceInput_);

  GetParameter("InIceOutput",
	       inIceOutput_);

  GetParameter("IceTopInput",
	       iceTopInput_);

  GetParameter("IceTopOutput",
	       iceTopOutput_);

  GetParameter("FirstLaunchCleaning",
	       firstLaunchCleaning_);
           
  if( cleanedKeysList_.length() != 0 )
  {
     log_debug
        ( "Will use frameobject %s to built list of OMKeys to clean if present.",
          cleanedKeysList_.c_str() );
  }
}

void I3DOMLaunchCleaning::DAQ(I3FramePtr frame)
{

  // By default we will use the list of OMs configured by the
  // the CleanedKeys parameter. If the CleanedKeysList parameter
  // is given we will try to get the list from the frame and
  // overwrite the pointer to the list of OMKeys with the list
  // found in the frame. If we do not find the object in the
  // frame we will fall back to the list configured by the
  // CleanedKeys parameters. This will allow as to use the
  // BadDomList from verification, but use a static list of
  // known bad DOMs if this is list not available.
  std::vector<OMKey> badOmKeys( cleanedKeys_ );

  if( cleanedKeysList_.length() != 0 )
  {
    if( frame->Has( cleanedKeysList_ ) )
    {
      I3VectorOMKeyConstPtr cleanedKeysListPtr = 
        frame->Get<I3VectorOMKeyConstPtr>(cleanedKeysList_);
      badOmKeys.clear();
      std::copy( cleanedKeysListPtr->begin(), 
                 cleanedKeysListPtr->end(), 
                 std::back_insert_iterator<std::vector<OMKey> >(badOmKeys) );
    }
    else
    {
     log_warn
        ( "Configured CleanedKeysList with name %s was not found in frame. "
          "We will revert to the static list.",
          cleanedKeysList_.c_str() );      
    }
  }

  I3DOMLaunchSeriesMapConstPtr inIceResponse = 
    frame->Get<I3DOMLaunchSeriesMapConstPtr>(inIceInput_);
  I3DOMLaunchSeriesMapConstPtr iceTopResponse = 
    frame->Get<I3DOMLaunchSeriesMapConstPtr>(iceTopInput_);


  I3DOMLaunchSeriesMapPtr newInIceResponse;
  if(inIceResponse)
    newInIceResponse = 
      I3DOMLaunchSeriesMapPtr(new I3DOMLaunchSeriesMap(*inIceResponse));

  I3DOMLaunchSeriesMapPtr newIceTopResponse;
  if(iceTopResponse)
    newIceTopResponse = 
      I3DOMLaunchSeriesMapPtr(new I3DOMLaunchSeriesMap(*iceTopResponse));


  for(std::vector<OMKey>::iterator iter = badOmKeys.begin() ; 
      iter != badOmKeys.end() ; 
      iter++)
    {
      if(newInIceResponse)
	newInIceResponse->erase(*iter);
      if(newIceTopResponse)
	newIceTopResponse->erase(*iter);
    }

  if(firstLaunchCleaning_)
    {
      if(newInIceResponse)
	{
	  for(I3DOMLaunchSeriesMap::iterator iter = 
		newInIceResponse->begin() ; 
	      iter != newInIceResponse->end() ; 
	      iter++)
	    {
	      if(iter->second.size() > 1)
		iter->second.resize(1);
	    }
	}
      if(newIceTopResponse)
	{
	  for(I3DOMLaunchSeriesMap::iterator iter = 
		newIceTopResponse->begin() ; 
	      iter != newIceTopResponse->end() ; 
	      iter++)
	    {
	      if(iter->second.size() > 1)
		iter->second.resize(1);
	    }
	}

    }

  if(newInIceResponse)
    frame->Put(inIceOutput_,newInIceResponse);
  if(newIceTopResponse)
    frame->Put(iceTopOutput_,newIceTopResponse);

  PushFrame(frame,"OutBox");
}
