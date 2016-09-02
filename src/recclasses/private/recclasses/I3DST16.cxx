/**
    copyright  (C) 2008
    the icecube collaboration

    @version $Revision: 1.2 $
    @date: mié dic 10 09:25:03 CST 2008
    @author juancarlos@icecube.wisc.edu
*/

#include <icetray/serialization.h>
#include <recclasses/I3DST.h>
#include <recclasses/I3DST16.h>
#include <boost/functional/hash/hash.hpp>

#include <cmath>

using std::cout;

I3DST16::I3DST16() : 
	t0_(0),
	t1_(0),
	reco1_(),
	reco2_(),
	nchannel_(0),
	nstring_(0),
	cog_(0,0,0),
	ndir_(0),
	ldir_(0),
	rlogl_(0),
	logE_(0),
	event_id_(0),
	sub_event_id_(0),
	triggertag_(0)
{ }

void I3DST16::SetRlogL(double rlogl)
{
    if (std::isnan(rlogl)) {
      rlogl_= 255;
      return;
    } 
    int irlogl = int(round(rlogl*10));

    irlogl = max(irlogl,0);
    irlogl = min(irlogl,254);
    rlogl_= uint8_t(irlogl);
}

void I3DST16::SetLogE(double logE)
{
    if (std::isnan(logE)) {
	  logE_ = 255;
	  return;
    }

	// store logE as a bin index 
	int ilogE = int(round((logE - DST_LOGE_OFFSET)/DST_LOGE_BINSIZE));

	// Make sure index is within 8-bit range of uint8_t
	ilogE = min(ilogE,254);
	ilogE = max(ilogE,0);

	logE_ = uint8_t(ilogE);
}


template <class Archive>
void
I3DST16::serialize(Archive& ar,unsigned version) 
{
  if (version != i3dst16_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DST class.", 
                    version,i3dst16_version_);
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("triggertag",triggertag_);
  ar & make_nvp("event_id",event_id_);
  ar & make_nvp("sub_event_id",sub_event_id_);
  ar & make_nvp("t0",t0_);
  ar & make_nvp("t1",t1_);
  ar & make_nvp("reco1",reco1_);
  ar & make_nvp("reco2",reco2_);
  ar & make_nvp("nchannel",nchannel_);
  ar & make_nvp("nstring",nstring_);
  ar & make_nvp("cog",cog_);
  ar & make_nvp("ndir",ndir_);
  ar & make_nvp("ldir",ldir_);
  ar & make_nvp("rlogl",rlogl_);
  ar & make_nvp("logE",logE_);
  ar & make_nvp("reco_label",reco_label_);
}

uint16_t DST16Utils::TriggerOffset(int triggerIndex) 
{
    if ( triggerIndex < 0 ) 
       return 0;
	
	return 1 << unsigned(triggerIndex);
}

int DST16Utils::TriggerIndex(TriggerKey key, vector<uint16_t> triggerIDs) 
{
    if (!key.CheckConfigID()) 
       return -1;

    vector<uint16_t>::iterator it;
    it = std::find( triggerIDs.begin(), triggerIDs.end(), key.GetConfigID() );

    if ( it == triggerIDs.end() )
       return -1;

    return  it - triggerIDs.begin();
}





I3_SERIALIZABLE(I3DST16);


