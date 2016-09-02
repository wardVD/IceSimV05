/**
  $Id$
  copyright  (C) 2007
  the icecube collaboration

  @version $Revision: 1.2 $
  @date $Date: 2006-02-20 06:38:45 -0600 (lun, 20 feb 2006) $
  @author juancarlos@icecube.wisc.edu
*/

#ifndef RECCLASSES_I3DST_13_H_INCLUDED
#define RECCLASSES_I3DST_13_H_INCLUDED

#include <icetray/I3Units.h>
#include <recclasses/I3DST.h>
#include <icetray/I3FrameObject.h>
#include "dataclasses/TriggerKey.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include <map>

#define SPEED_BIN_SIZE double(0.5/255)

using namespace std;
static const unsigned i3dst13_version_ = 7;

namespace DST13Utils {
  
  enum TriggerConfigID // enumeration of trigger configuration IDs
  {
       IN_ICE_SMT8 = 1006,      // InIce simple majority trigger SMT8
       IN_ICE_SMT3 = 1011,      // InIce simple majority trigger SMT3
       IN_ICE_STRING_CLUSTER = 1007, // cluster trigger
       IN_ICE_SLOW_MP = 22001,  // SLOW Monopole trigger
       ICE_TOP_SMT  = 102,      // IceTop SMT6
       IN_ICE_CYLINDER = 21001, // CylinderTrigger
       IN_ICE_MINBIAS = 101     // Minum Bias trigger
  };


  uint16_t TriggerOffset(int triggerIndex);

  int TriggerIndex(TriggerKey key, vector<uint16_t> triggerIDs);
}

/**
 *  @brief Stores DST data extracted from reconstructed events (see project dst-extractor).
 */
class I3DST13: public I3FrameObject
{
  protected:

    // Event time 1e-5 sec (5 B)
    uint8_t  t0_;   
    uint32_t t1_;   

    // These elements are not serialized
    uint32_t event_id_;      // (4 B) 
    uint16_t hpix_nside_;        // healpix NSIDE parameter (1 B)
    vector<uint16_t> triggers_; // list of encoded triggers
    vector<std::string> recos_; // list of reconstructions


  public:

    template <class Archive> void serialize(Archive&,unsigned);
    I3DST13();

    /**
     * mutator to set number of hit DOMs
     */ 
    inline void SetEventId(uint32_t eventid) 
    {
      event_id_ = eventid;
    }

    inline uint32_t GetEventId() const
    {
      return event_id_;
    }

        inline void SetHealPixNSide(uint16_t nside) { hpix_nside_ = nside; }
        inline void SetTriggers(vector<uint16_t> triggers) { triggers_ = triggers; }
        inline void SetRecos(vector<std::string> recos) { recos_ = recos; }

        inline uint16_t GetHealPixNSide() const { return hpix_nside_; }
        inline vector<uint16_t> GetTriggers() const { return triggers_; }
        inline vector<std::string> GetRecos() const { return recos_; }

    /**
     * mutator to set time
     * @param time 32bit unsigned in units of 100 mus relative to first
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
     * @return time 32bit unsigned in units of 100 mus relative to first
     * event.
     */ 
    inline uint64_t GetTime() const
    {
            const uint64_t onebit = 1ul;
      return t0_*(onebit<<32) + uint64_t(t1_);
    }

};

I3_POINTER_TYPEDEFS(I3DST13);
BOOST_CLASS_VERSION(I3DST13,i3dst13_version_);

#endif

