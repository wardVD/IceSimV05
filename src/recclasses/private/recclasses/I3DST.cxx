/**
    copyright  (C) 2008
    the icecube collaboration

    @version $Revision: 1.2 $
    @date: mié dic 10 09:25:03 CST 2008
    @author juancarlos@icecube.wisc.edu
*/

#include <icetray/serialization.h>
#include <recclasses/I3DST.h>
#include <boost/functional/hash/hash.hpp>

#include <sstream>
#include <bitset>
#include <limits>
#include <cmath>

using std::cout;

DSTReco::DSTReco():
        direction(0),
        distance(0),
        position(0,0,0) {}

I3DST::I3DST() : 
	reco1_(),
	reco2_(),
	nhit0_(0),   
	nhit1_(0),   
	nchannel_(0),
	nstring_(0),
	time_(0),
	cog_(0,0,0),
	ndir_(0),
	ldir_(0),
	rlogl_(0),
	triggertag_(0),
	legacy_reco1_(0),
	legacy_reco2_(0),
	legacy_twr_nhit0_(0),
	legacy_twr_nhit1_(0),
	legacy_twr_nchannel_(0)
{ }

void I3DST::SetRlogL(double rlogl)
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

void I3DST::SetLogE(double logE)
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
DSTReco::serialize(Archive& ar,unsigned version) 
{
  if (version > dstreco09_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DST class.", 
                    version,i3dst09_version_);
  if (version < dstreco09_version_) {
     ar & make_nvp("direction",direction);
     ar & make_nvp("distance",distance);
     ar & make_nvp("position",position);
  } else if (version == dstreco09_version_) {
     ar & make_nvp("direction",direction);
  }
}

template <class Archive>
void
DSTPosition::serialize(Archive& ar,unsigned version) 
{
  ar & make_nvp("x",x_);
  ar & make_nvp("y",y_);
  ar & make_nvp("z",z_);
}


template <class Archive>
  void I3DST::save(Archive& ar, unsigned version) const
 {
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("triggertag",triggertag_);
  ar & make_nvp("reco1",reco1_);
  ar & make_nvp("reco2",reco2_);
  ar & make_nvp("nchannel",nchannel_);
  ar & make_nvp("nstring",nstring_);
  ar & make_nvp("nhit0",nhit0_);
  ar & make_nvp("nhit1",nhit1_);
  ar & make_nvp("cog",cog_);
  ar & make_nvp("time",time_);
  ar & make_nvp("ndir",ndir_);
  ar & make_nvp("ldir",ldir_);
  ar & make_nvp("rlogl",rlogl_);
  ar & make_nvp("logE",logE_);
  ar & make_nvp("reco_label",reco_label_);
}

template <class Archive>
void
I3DST::load(Archive& ar,unsigned version) 
{
  if (version > i3dst09_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DST class.", 
                    version,i3dst09_version_);
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

  if (version <= i3dst07b_version_) {
     ar & make_nvp("triggertag",triggertag_);
     ar & make_nvp("coord_reco1",legacy_reco1_);
     reco1_.SetDirection(legacy_reco1_);

     ar & make_nvp("coord_reco2",legacy_reco2_);
     reco2_.SetDirection(legacy_reco2_);

     ar & make_nvp("distance",legacy_distance_);
     reco1_.SetDistance(legacy_distance_);
     reco2_.SetDistance(legacy_distance_);

     ar & make_nvp("twr_ndom",legacy_twr_nchannel_);
     ar & make_nvp("ic_ndom",nchannel_);
     ar & make_nvp("twr_nhit0",legacy_twr_nhit0_);
     ar & make_nvp("twr_nhit1",legacy_twr_nhit1_);
     ar & make_nvp("ic_nhit0",nhit0_);
     ar & make_nvp("ic_nhit1",nhit1_);
     ar & make_nvp("time",time_);
     ar & make_nvp("reco_label",reco_label_);

  } else if (version == i3dst08_version_) {

     ar & make_nvp("triggertag",triggertag_);
     ar & make_nvp("reco1",reco1_);
     ar & make_nvp("reco2",reco2_);
     ar & make_nvp("nchannel",nchannel_);
     ar & make_nvp("nstring",nstring_);
     ar & make_nvp("nhit0",nhit0_);
     ar & make_nvp("nhit1",nhit1_);
     ar & make_nvp("cog",cog_);
     ar & make_nvp("time",time_);
     ar & make_nvp("ndir",ndir_);
     ar & make_nvp("reco_label",reco_label_);

  } else if (version == i3dst09_version_) {
     ar & make_nvp("triggertag",triggertag_);
     ar & make_nvp("reco1",reco1_);
     ar & make_nvp("reco2",reco2_);
     ar & make_nvp("nchannel",nchannel_);
     ar & make_nvp("nstring",nstring_);
     ar & make_nvp("nhit0",nhit0_);
     ar & make_nvp("nhit1",nhit1_);
     ar & make_nvp("cog",cog_);
     ar & make_nvp("time",time_);
     ar & make_nvp("ndir",ndir_);
     ar & make_nvp("ldir",ldir_);
     ar & make_nvp("rlogl",rlogl_);
     ar & make_nvp("logE",logE_);
     ar & make_nvp("reco_label",reco_label_);
   }
}

I3_SPLIT_SERIALIZABLE(I3DST);
I3_SERIALIZABLE(DSTPosition);
I3_SERIALIZABLE(DSTReco);


uint16_t DSTUtils::TriggerOffset(TriggerKey key) 
{
	uint16_t triggertype = 0;
	uint16_t source = 0;

	switch(key.GetSource()) {
			case TriggerKey::IN_ICE: 
					log_debug(" IC ");
					source = DST_IN_ICE;
					log_debug("SourceID IC (%d)\n", DST_IN_ICE);
					break;
			case TriggerKey::ICE_TOP: 
					log_debug(" IT ");
					source = DST_ICE_TOP;
					log_debug("SourceID IT (%d)\n", DST_ICE_TOP);
					break;
			case TriggerKey::AMANDA_TWR_DAQ: 
					log_debug(" TWR ");
					source = DST_AMANDA_TWR_DAQ;
					log_debug("SourceID TWR (%d)\n", DST_AMANDA_TWR_DAQ);
					break;
			case TriggerKey::SPASE: 
					log_debug(" SPASE ");
					source = DST_SPASE;
					log_debug("SourceID SPASE (%d)\n", DST_SPASE);
					break;
			default: 
					//log_error(" could not determine detector %d ", key.GetSource());
					source = 0;
	}
	switch(key.GetType()) {
			case TriggerKey::SIMPLE_MULTIPLICITY : 
					log_debug(" SM ");
					triggertype = DST_SIMPLE_MULTIPLICITY; 
					break;
			case TriggerKey::MIN_BIAS : 
					log_debug(" MB ");
					triggertype = DST_MIN_BIAS;
					break;
			case TriggerKey::STRING: 
					log_debug(" STRING ");
					triggertype = DST_STRING;
					break;
			case TriggerKey::VOLUME: 
					log_debug(" VOLUME ");
					triggertype = DST_VOLUME;
					break;
			case TriggerKey::SPHERE: 
					log_debug(" SPHERE ");
					triggertype = DST_SPHERE;
					break;
			case TriggerKey::FRAGMENT_MULTIPLICITY : 
					log_debug(" FM ");
					triggertype = DST_FRAGMENT_MULTIPLICITY;
					break;
			case TriggerKey::UNKNOWN_TYPE : 
					log_debug(" UNKNOWN ");
					triggertype = DST_UNKNOWN_TYPE;
					break;
			case TriggerKey::SPASE_2 : 
					log_debug(" SP ");
					triggertype = DST_SPASE_2;
					break;
			default: 
					log_debug(" could not determine trigger type %d ", key.GetType());
					triggertype  = 0;
			log_debug("TypeID SP (%d)\n", triggertype);
	}
	log_debug("triggertype << source = (%d) %d (%d)\n", triggertype, triggertype << source, triggertype);
	return triggertype << source;
}

bool DSTUtils::IsDSTHeader(char* stream)
{
	uint8_t flag;

	DSTUtils::copy(&flag,stream,0);
	log_debug("flag = %d, header_flag = %d" , int(flag), int(DSTUtils::header_flag));
	return (flag & DSTUtils::header_flag);
}

// iota is a terrible name for this function. this could easily be
// template-ized. then casting your input will give you the output
// format you want. ie:
// DSTUtils::itoa((uint8_t) 8)  == 00001000
// DSTUtils::itoa((uint16_t) 8) == 0000000000001000

string DSTUtils::itoa(const uint16_t& bits) {
	std::stringstream ss;
	ss << std::bitset<std::numeric_limits<uint16_t>::digits>(bits);
	return ss.str();
}

void DSTUtils::PrintTrigger(I3TriggerHierarchyConstPtr triggers) 
{
	string ret = "";

    I3TriggerHierarchy::iterator trigger_it;
    for (trigger_it = triggers->begin(); trigger_it != triggers->end(); trigger_it++) 
    { 
            //bitwise OR
            ret = ret + trigger_it->GetTriggerKey().GetSourceString() + ":";
            ret = ret + trigger_it->GetTriggerKey().GetTypeString()   + ", ";
    }
    log_info("triggers: %s",ret.c_str()); 
}
