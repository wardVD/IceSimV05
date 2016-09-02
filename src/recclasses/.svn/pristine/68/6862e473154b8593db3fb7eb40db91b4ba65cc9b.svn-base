/**
    copyright  (C) 2008
    the icecube collaboration

    @version $Revision: 1.2 $
    @date: mié dic 10 09:25:03 CST 2008
    @author juancarlos@icecube.wisc.edu
*/

#include <icetray/serialization.h>
#include <recclasses/I3DST.h>
#include <recclasses/I3DST13.h>
#include <boost/functional/hash/hash.hpp>

#include <cmath>

using std::cout;

I3DST13::I3DST13() : 
	t0_(0),
	t1_(0),
	event_id_(0)
{ }


uint16_t DST13Utils::TriggerOffset(int triggerIndex) 
{
    if ( triggerIndex < 0 ) 
       return 0;
	
	return 1 << unsigned(triggerIndex);
}

int DST13Utils::TriggerIndex(TriggerKey key, vector<uint16_t> triggerIDs) 
{
    if (!key.CheckConfigID()) 
       return -1;

    vector<uint16_t>::iterator it;
    it = std::find( triggerIDs.begin(), triggerIDs.end(), key.GetConfigID() );

    if ( it == triggerIDs.end() )
       return -1;

    return  it - triggerIDs.begin();
}


template <class Archive>
void
I3DST13::serialize(Archive& ar,unsigned version) 
{
  if (version != i3dst13_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DST class.", 
                    version,i3dst13_version_);
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("event_id",event_id_);
  ar & make_nvp("t0",t0_);
  ar & make_nvp("t1",t1_);
}

I3_SERIALIZABLE(I3DST13);
