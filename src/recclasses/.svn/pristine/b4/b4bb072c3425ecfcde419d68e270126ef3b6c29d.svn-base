/**
	$Id$
    copyright  (C) 2007
    the icecube collaboration

    @version $Revision: 1.2 $
    @date $Date: 2006-02-20 06:38:45 -0600 (lun, 20 feb 2006) $
    @author juancarlos@icecube.wisc.edu
*/

#ifndef RECCLASSES_I3DSTRECO_16_H_INCLUDED
#define RECCLASSES_I3DSTRECO_16_H_INCLUDED

#include <icetray/I3Units.h>
#include <recclasses/I3DST.h>
#include <icetray/I3FrameObject.h>
#include "dataclasses/TriggerKey.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include <map>

#define SPEED_BIN_SIZE double(0.5/255)

using namespace std;
static const unsigned i3dst16_version_ = 8;

namespace DST16Utils {
  
  /// enumeration of trigger configuration IDs
  enum TriggerConfigID
  {
       IN_ICE_SMT8 = 1006,      ///< InIce simple majority trigger SMT8
       IN_ICE_SMT3 = 1011,      ///< InIce simple majority trigger SMT3
       IN_ICE_STRING_CLUSTER = 1007, ///< cluster trigger
       IN_ICE_SLOW_MP = 22001,  ///< SLOW Monopole trigger
       ICE_TOP_SMT  = 102,      ///< IceTop SMT6
       IN_ICE_CYLINDER = 21001, ///< CylinderTrigger
       IN_ICE_MINBIAS = 101     ///< Minum Bias trigger
  };


  uint16_t TriggerOffset(int triggerIndex);

  int TriggerIndex(TriggerKey key, vector<uint16_t> triggerIDs);

}

/**
 *  @brief Stores DST data extracted from reconstructed events (see project dst-extractor).
 */
class I3DST16: public I3FrameObject
{
	protected:

		/// Event time 1e-5 sec (5 B)
		uint8_t  t0_;   
		uint32_t t1_;   

		// These elements are not serialized as part if I3DST
		uint16_t hpix_nside_;       ///< healpix NSIDE parameter (1 B)
		vector<uint16_t> triggers_; ///< list of encoded triggers
		vector<std::string> recos_; ///< list of reconstructions

		/// 1st reconstruction - LF (4 B)                
		uint32_t reco1_;

		/// 2nd reconstruction - DP (4 B)
		uint32_t reco2_;

		/// Number of DOMs (after cleaning) (2 B)
		uint16_t nchannel_;

		/// Number of hit Strings (after cleaning) (1 B)
		uint8_t nstring_;   
		DSTPosition cog_;      ///< COG  (3 B)
		uint8_t ndir_;         ///< Number of direct hits for second reco (1 B)
		uint8_t ldir_;         ///< (1 B)
		uint8_t rlogl_;        ///< Reduced LLH (1 B)
		uint8_t logE_;         ///< logE Energy estimate   (1 B)
		uint8_t reco_label_;   ///< (1 B)
		uint32_t event_id_;    ///< (4 B) 
		uint8_t sub_event_id_; ///< (1 B)
		uint16_t triggertag_;  ///< (2 B)

		vector<uint8_t> triggertimes_;   


	public:

		template <class Archive> void serialize(Archive&,unsigned);
		I3DST16();

	/**
     * mutator to set number of hit DOMs
     * @param eventid
     */ 
	inline void SetEventId(uint32_t eventid) 
	{
		event_id_ = eventid;
	}

	inline uint32_t GetEventId() const
	{
		return event_id_;
	}

	/**
     * mutator to set subevent id
     * @param subeventid
     */ 
	inline void SetSubEventId(uint8_t subeventid) 
	{
		sub_event_id_ = subeventid;
	}

	inline uint8_t GetSubEventId() const
	{
		return sub_event_id_;
	}

	/**
     * mutator to set number of hit DOMs
     * @param nchannel 16bit unsigned 
     */ 
	inline void SetNDOM(uint16_t nchannel) 
	{
		nchannel_ = min(nchannel,uint16_t((1<<16)-1));
	}

	inline uint16_t GetNDOM() const
	{
		return nchannel_;
	}

	/**
     * accessor to get number of hit strings
     * @return nhit 16bit unsigned corresponding to number of hit strings
     */ 
	inline uint16_t GetNString() const
	{
		return nstring_;
	}

	/**
     * mutator to set number of hit strings
     * @param nstring number of hit strings
     */ 
	inline void SetNString(uint16_t nstring) 
	{
		nstring_= nstring;
	}


	/**
     * mutator to set number of direct hits for second reco
     * @param ndir 8bit unsigned 
     */ 
	inline void SetNDir(uint8_t ndir) 
	{
		ndir_ = ndir;
	}

	/**
     * mutator to set length of direct hits for second reco
     * @param ldir
     */ 
	inline void SetLDir(uint8_t ldir) 
	{
		ldir_ = ldir;
	}

	/**
     * accessor to get number of direct hits for second reco 
     * @return 8bit unsigned int 
     */ 
	inline uint8_t GetNDir() 
	{
		return ndir_;
	}

	/**
     * accessor to get length of direct hits for second reco 
     * @return 16bit unsigned int 
     */ 
	inline uint8_t GetLDir() 
	{
		return ldir_;
	}

	/**
     * mutator to set reduced log likelihood
     * @param rlogl
     */ 
	inline void SetRlogL(uint8_t rlogl) 
	{
		rlogl_=rlogl;
        }

	/**
     * mutator to set reduced log likelihood
     * @param rlogl
     */ 
	void SetRlogL(double rlogl);

	/**
     * accessor to get reduced log likelihood
     * @return double
     */ 
	inline double GetRlogL() 
	{
		return rlogl_/10.0;
	}

	/**
     * mutator to set Center of Gravity of IceCube hits
     * @param x 8bit unsigned
     * @param y 8bit unsigned
     * @param z 8bit unsigned
     */ 
	inline void SetCOG(int8_t x,int8_t y, int8_t z) 
	{
		cog_.SetPosition(x,y,z);
	}

	/**
     * mutator to set Center of Gravity of IceCube hits
     * @param x double
     * @param y double
     * @param z double
     */ 
	inline void SetCOG(double x,double y, double z) 
	{
            if ( std::isnan(x) || std::isnan(y) || std::isnan(z) ) {
                 cog_.SetPosition(128,128,128);
                 return;
            }
            int ix = min(max(int(round(x)),-127),127);
            int iy = min(max(int(round(y)),-127),127);
            int iz = min(max(int(round(z)),-127),127);
			cog_.SetPosition(uint8_t(ix),uint8_t(iy),uint8_t(iz));
	}


	/**
     * accessor to get Center of Gravity of IceCube hits
     * @return DSTPosition (3B)
     */ 
	inline DSTPosition GetCOG() { return cog_; }



	/**
     * accessor to get DSTReco object
     * @return uint32_t reco1
     */ 
	const uint32_t GetReco1() { return reco1_; }

	/**
     * mutator to set DSTReco object
     * @param reco DSTReco first reconstruction
     */ 
	void SetReco1(DSTReco reco) { reco1_ = reco.GetDirection(); }

	/**
     * mutator to set DSTReco object
     * @param dir DSTReco first reconstruction direction
     */ 
	void SetReco1Direction(int32_t dir) { reco1_ = dir; }

	/**
     * mutator to set DSTReco object
     * @param dir DSTReco first reconstruction direction
     */ 
		void SetReco2Direction(int32_t dir) { reco2_ = dir; }

	/**
     * accessor to get DSTReco object
     * @return uint32_t
     */ 
	const uint32_t GetReco2() { return reco2_; }

	/**
     * mutator to set DSTReco object
     * @param reco DSTReco second reconstruction
     */ 
	void SetReco2(DSTReco reco) { reco2_ = reco.GetDirection(); }

	/**
     * accessor to get Reconstruction binary label
     * @return 8bit binary flag
     */ 
	inline uint8_t GetRecoLabel() const
	{
             return reco_label_;
        }

	/**
     * mutator bitwise "OR" a flag with current reco label
     * @param label reco label
     */ 
	inline void AddRecoLabel(uint8_t label) 
	{
             // bitwise OR labels
             reco_label_ = reco_label_ | label; 
        }

	/**
     * mutator set value of Reco Label
     * @param label reco label
     */ 
	inline void SetRecoLabel(uint8_t label) 
	{
             // bitwise OR labels
             reco_label_ = label; 
        }

	/**
     * accessor to get Trigger tag
     * @return 16bit binary flag
     */ 
	inline uint16_t GetTriggerTag() const
	{
             return triggertag_;
        }

	/**
     * mutator bitwise "OR" a flag with current triggertag
     * @param trigger
     */ 
	inline void AddTriggerTag(uint16_t trigger) 
	{
             // bitwise OR labels
             triggertag_ = triggertag_ | trigger; 
        }

	/**
     * mutator set value of triggertag
     * @param trigger
     */ 
	inline void SetTriggerTag(uint16_t trigger) 
	{
             // bitwise OR labels
             triggertag_ = trigger; 
        }

	/**
     * mutator to set logE  
     * @param logE
     */ 
	void SetLogE(double logE);

	/**
     * accessor to get logE  
     * @return logE
     */ 
	inline double GetLogE() 
	{
		// store logE as a bin index 
		if (logE_)
			  return logE_*DST_LOGE_BINSIZE + DST_LOGE_OFFSET;

		return logE_;
	}

	inline uint8_t GetTriggerTime(unsigned index) const
	{
		return triggertimes_.at(index);
	}

	inline void PushTriggerTime(uint8_t time) 
	{
		triggertimes_.push_back(time);
        }

	inline bool HasTriggerTimes() const
	{
		return triggertimes_.size();
        }

	/**
     * mutator to set time
     * @param time 64bit unsigned in units of 100 mus relative to first
     * event.
     */ 
	inline void SetTime(uint64_t time) 
	{
	        const uint64_t onebit = 1ul;
		t0_ = uint8_t(time/(onebit<<32));
		t1_ = uint32_t(time%(onebit<<32));
	}

	/**
     * accessor to get time
     * @return time 64bit unsigned in units of 100 mus relative to first
     * event.
     */ 
	inline uint64_t GetTime() const
	{
	        const uint64_t onebit = 1ul;
		return t0_*(onebit<<32) + uint64_t(t1_);
	}

        inline void SetHealPixNSide(uint16_t nside) { hpix_nside_ = nside; }
        inline void SetTriggers(vector<uint16_t> triggers) { triggers_ = triggers; }
        inline void SetRecos(vector<std::string> recos) { recos_ = recos; }

        inline uint16_t GetHealPixNSide() const { return hpix_nside_; }
        inline vector<uint16_t> GetTriggers() const { return triggers_; }
        inline vector<std::string> GetRecos() const { return recos_; }

};



I3_POINTER_TYPEDEFS(I3DST16);
BOOST_CLASS_VERSION(I3DST16,i3dst16_version_);

#endif
