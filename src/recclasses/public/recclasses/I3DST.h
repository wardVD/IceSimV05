/**
	$Id$
    copyright  (C) 2007
    the icecube collaboration

    @version $Revision: 1.2 $
    @date $Date: 2006-02-20 06:38:45 -0600 (lun, 20 feb 2006) $
    @author juancarlos@icecube.wisc.edu
*/

#ifndef DST_I3DST_H_INCLUDED
#define DST_I3DST_H_INCLUDED

#include <icetray/I3Units.h>
#include <icetray/I3FrameObject.h>
#include "dataclasses/TriggerKey.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include <map>

#define DST_SIMPLE_MULTIPLICITY 1
#define DST_MIN_BIAS 2
#define DST_STRING 4
#define DST_VOLUME 8
#define DST_SPHERE 16
#define DST_FRAGMENT_MULTIPLICITY 32
#define DST_UNKNOWN_TYPE 64
#define DST_SPASE_2 1

#define DST_IN_ICE 1
#define DST_ICE_TOP 6
#define DST_AMANDA_TWR_DAQ 8
#define DST_SPASE 15

// Magic numbers
#define DST_LOGE_BINSIZE 0.027450980392156865
#define DST_LOGE_OFFSET  2.0 

#define MJD07 54101
#define MJD08 54466
#define MJD09 54832
#define MJD10 55197
#define MJD11 55571
#define MJD12 55927
#define MJD13 56293
#define MJD14 56658
#define MJD15 57023
#define MJD16 57388


using namespace std;
static const unsigned i3dst07_version_  = 0;
static const unsigned i3dst07a_version_ = 1;
static const unsigned i3dst07b_version_ = 2;
static const unsigned i3dst08_version_  = 3;
static const unsigned i3dst09_version_  = 4;
static const unsigned i3dst10_version_  = 5;

static const unsigned dstreco08_version_  = 0;
static const unsigned dstreco09_version_  = 1;
static const unsigned dstreco10_version_  = 1;

/**
 *  @brief Compact storage for a position.
 */
class DSTPosition// (3B)
{
    private: 
        int8_t x_;   
        int8_t y_;   
        int8_t z_;   
        /*
		friend class boost::serialization::access;
		template <class Archive> void save(Archive & ar, unsigned version) const;
		template <class Archive> void load(Archive & ar, unsigned version);
		BOOST_SERIALIZATION_SPLIT_MEMBER();
        */

    public: 

		template <typename Archive>
				void serialize(Archive&,unsigned);

        DSTPosition():x_(0),y_(0),z_(0){}

        DSTPosition(int8_t x ,int8_t y,int8_t z) { x_=x; y_=y; z_=z; }

        void SetPosition(int8_t x ,int8_t y,int8_t z) { x_=x; y_=y; z_=z; }

        inline int8_t GetX() const {return x_;}

        inline int8_t GetY() const {return y_;}

        inline int8_t GetZ() const {return z_;}
};

/**
 *  @brief Compact storage for a track.
 */
class DSTReco // (6B)
{
    private:
		int16_t direction;      //   (2 B)
		uint8_t distance;       // distance from ctr of detector  (1 B)
		DSTPosition position;   //  position of track (3 B)

    public:


        DSTReco();
        inline void SetDirection(int16_t dir) { direction = dir; }

        inline void SetDistance(uint8_t d) { distance = d; }

        inline void SetPosition(int8_t x,int8_t y, int8_t z) { position.SetPosition(x,y,z); }

        inline int16_t GetDirection() const { return direction; }

        inline uint8_t GetDistance() const { return distance; }

        inline int8_t GetX() const { return position.GetX(); }

        inline int8_t GetY() const { return position.GetY(); }

        inline int8_t GetZ() const { return position.GetZ(); }

        inline DSTPosition GetPosition() const { return position; }

        template <typename Archive>
               void serialize(Archive&,unsigned);
};

/**
 *  @brief Stores DST data extracted from reconstructed events (see project dst-extractor).
 */
class I3DST: public I3FrameObject
{
	protected:

		/// 1st reconstruction - LF (6 B)                 
		DSTReco reco1_;

		/// 2nd reconstruction - DP (6 B)
		DSTReco reco2_;

		/// Number of Hits (from FE) (3 B)
        uint8_t  nhit0_;   
        uint16_t nhit1_;   

		/// Number of DOMs (after cleaning) (2 B)
		uint16_t nchannel_;   

		/// Number of hit Strings (after cleaning) (1 B)
		uint8_t nstring_;   

		/// Event time difference 1/10000 sec (4 B)
		uint32_t time_;   

		/// COG (3 B)
		DSTPosition cog_;   

		/// Number of direct hits for second reco (1 B)
		uint8_t ndir_;   

		/// LDir (1 B)
		uint8_t ldir_;   

		/// Reduced LLH (1 B)
		uint8_t rlogl_;   

		/// logE Energy estimate   (1 B)
		uint8_t logE_;   

		/// reco labels (1 B)
		uint8_t reco_label_;   

		/// Trigger Tag   (2 B)
		uint16_t triggertag_;   

	private:

		friend class boost::serialization::access;
		template <class Archive> void save(Archive & ar, unsigned version) const;
		template <class Archive> void load(Archive & ar, unsigned version);
		BOOST_SERIALIZATION_SPLIT_MEMBER();

	public:
		// legacy variables
		int16_t  legacy_reco1_;  //   (2 B)
		int16_t  legacy_reco2_;  //   (2 B)
		uint8_t  legacy_distance_;  //   (1 B)
		uint16_t legacy_delta_t_;   
		uint8_t  legacy_twr_nhit0_;   
		uint16_t legacy_twr_nhit1_;   
		uint16_t legacy_twr_nchannel_;   


		I3DST();

		/**
         * mutator to set number of hits
         * @param nhit 32bit unsigned corresponding to number of hits
         */ 
		inline void SetNHit(uint32_t nhit) 
		{
			nhit0_ = nhit/(1<<16);
			nhit1_ = nhit%(1<<16);
		}

		/**
         * mutator to set number of twr hits
         * @param nhit 32bit unsigned corresponding to number of hits
         */ 
		inline void SetTWRNHit(uint32_t nhit) 
		{
			legacy_twr_nhit0_ = nhit/(1<<16);
			legacy_twr_nhit1_ = nhit%(1<<16);
		}
		/**
         * accessor to get number of hits
         * @return nhit 32bit unsigned corresponding to number of hits
         */ 
		inline uint32_t GetNHit() const
		{
			return nhit0_*(1<<16) + nhit1_;
		}

		/**
         * accessor to get number of twr hits
         * @return nhit 32bit unsigned corresponding to number of hits
         */ 
		inline uint32_t GetTWRNHit() const
		{
			return legacy_twr_nhit0_*(1<<16) + legacy_twr_nhit1_;
		}

		/**
         * accessor to get number of twr channels hit
         * @return nhit 32bit unsigned corresponding to number of channels
         */ 
		inline uint32_t GetTWRNChannel() const
		{
			return uint32_t(legacy_twr_nchannel_);   
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
         * @param nstring 16bit unsigned corresponding to number of hit strings
         */ 
		inline void SetNString(uint16_t nstring) 
		{
			nstring_= nstring;
		}

		/**
         * mutator to set time
         * @param time 32bit unsigned in units of 100 mus relative to first
         * event.
         */ 
		inline void SetTime(uint32_t time) 
		{
			time_ = time;
		}

		/**
         * accessor to get time
         * @return 32bit unsigned in units of 100 mus relative to first
         * event.
         */ 
		inline uint32_t GetTime() const
		{
			return time_;
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
         * @param ldir 8bit unsigned 
         */ 
		inline void SetLDir(uint8_t ldir) 
		{
			ldir_ = ldir;
		}

		/**
         * accessor to get number of direct hits for second reco 
         * @return ndir 8bit unsigned int 
         */ 
		inline uint8_t GetNDir() 
		{
			return ndir_;
		}

		/**
         * accessor to get length of direct hits for second reco 
         * @return ldir
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
         * @return 8bit unsigned int 
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
         * @return DSTReco 
         */ 
		const DSTReco& GetReco1() { return reco1_; }

		/**
         * mutator to set DSTReco object
         * @param reco DSTReco first reconstruction
         */ 
		void SetReco1(DSTReco reco) { reco1_ = reco; }

		/**
         * mutator to set DSTReco object
         * @param dir DSTReco first reconstruction direction
         */ 
		void SetReco1Direction(int16_t dir) { reco1_.SetDirection(dir); }

		/**
         * mutator to set DSTReco object
         * @param dir DSTReco second reconstruction direction
         */ 
		void SetReco2Direction(int16_t dir) { reco2_.SetDirection(dir); }

		/**
         * accessor to get DSTReco object
         * @return DSTReco 
         */ 
		const DSTReco& GetReco2() { return reco2_; }

		/**
         * mutator to set DSTReco object
         * @param reco DSTReco second reconstruction
         */ 
		void SetReco2(DSTReco reco) { reco2_ = reco; }

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
         * @param label 8bit reco label
         */ 
		inline void AddRecoLabel(uint8_t label) 
		{
             // bitwise OR labels
             reco_label_ = reco_label_ | label; 
        }

		/**
         * mutator set value of Reco Label
         * @param label 8bit reco label
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
         * @param trigger 16bit trigger
         */ 
		inline void AddTriggerTag(uint16_t trigger) 
		{
             // bitwise OR labels
             triggertag_ = triggertag_ | trigger; 
        }

		/**
         * mutator set value of triggertag
         * @param trigger 8bit trigger
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
};

/**
 * namespace with utility functions for performing operations on DST
 */

namespace DSTUtils {

		const uint8_t header_flag=1;   // bitmask for testing if frame is header

		template<typename T>
		inline uint8_t copy(char* stream ,T *obj, uint8_t position) 
		{
			strncpy(&stream[position], (char*) obj, sizeof(T));
			return position + sizeof(T);
		}

		template<typename T>
		inline uint8_t copy(T *obj, char* stream,uint8_t position)
		{
			strncpy((char*) obj,&stream[position], sizeof(T));
			return position + sizeof(T);
		}

		/**
         * @return true if first 16bit of binary dst object correspond to a
         * dst header
         */
		bool IsDSTHeader(char* stream);


		/**
         * @return true if trigger flag corresponds to an In-Ice Trigger
         */
		inline bool isInIce(uint16_t trigger) 
		{
			return trigger & 
					(((2 << (DST_ICE_TOP-DST_IN_ICE)) -1) << DST_IN_ICE);
		}

		/**
         * @return true if trigger flag corresponds to an IceTop Trigger
         */
		inline bool isIceTop(uint16_t trigger)
		{
			return trigger & 
				(((2 << (DST_AMANDA_TWR_DAQ-DST_ICE_TOP))-1)<< DST_ICE_TOP);
		}

		/**
         * @return true if trigger flag corresponds to a TWR Trigger
         */
		inline bool isTWR(uint16_t trigger)
		{
			return trigger & 
				(((2 << (DST_SPASE-DST_AMANDA_TWR_DAQ))-1)<< DST_AMANDA_TWR_DAQ);
		}

		/**
         * @return true if trigger flag corresponds to a SPASE Trigger
         */
		inline bool isSPASE(uint16_t trigger)
		{
			return trigger & (1 << DST_SPASE);
		}

		/**
         * @param trigger 16bit unsigned trigger flag
         * @return true if binary data corresponds is a header frame
         */
		inline bool isTimeCalibFrame(uint16_t trigger)
		{
			return trigger & (1 << (DST_SPASE+1));
		}

		/**
         * @param key TriggerKey
         * @return the number of bits to offset flag 
         */
		uint16_t TriggerOffset(TriggerKey key);

		/**
         * @param bits 16bit unsigned trigger flag
         * @return string representation of bits in trigger flag
         */
		string itoa(const uint16_t& bits);

		void PrintTrigger(I3TriggerHierarchyConstPtr triggers);
};


I3_POINTER_TYPEDEFS(I3DST);
BOOST_CLASS_VERSION(I3DST,i3dst09_version_);
BOOST_CLASS_VERSION(DSTReco,dstreco09_version_);

#endif
