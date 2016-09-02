/**
    $Id$
    copyright  (C) 2007
    the icecube collaboration

    @date $Date: 2008-02-18 $
    @author juancarlos@icecube.wisc.edu
*/

#ifndef DST_I3DSTHEADER_13_H_INCLUDED
#define DST_I3DSTHEADER_13_H_INCLUDED

#include <icetray/I3Units.h>
#include <icetray/I3FrameObject.h>
#include <recclasses/I3DSTHeader.h>
#include <recclasses/I3DST13.h>
#include <map>


using namespace std;
static const unsigned i3dstheader13_version_ = 7;

/**
 *  @brief Stores information relevant for I3DST events that follow it (see project dst-extractor).
 * 
 *  The information includes things such as runId, year, driving time,
 *  and parameters for building the binned sky map.
 */
class I3DSTHeader13: public I3DSTHeader
{
	private:
		// data elements
		uint32_t runId_;        // (4 B)
		uint32_t eventId_;      // (4 B) 
		uint16_t mjd_;          // (2 B)
		uint16_t hpix_nside_;       ///< healpix NSIDE parameter (1 B)
		vector<uint16_t> triggers_; ///< list of encoded triggers
		vector<std::string> recos_; ///< list of reconstructions

	public:

		I3DSTHeader13();

		// in order to serialize object
		template <typename Archive> void serialize(Archive&,unsigned);

        inline void SetEventId(uint32_t eventId) { eventId_ = eventId; }
        inline void SetRunId(uint32_t runId) { runId_ = runId; }
        inline void SetModJulianDay(uint16_t mjd) { mjd_ = mjd; }
        inline void SetHealPixNSide(uint16_t nside) { hpix_nside_ = nside; }
        inline void SetTriggers(vector<uint16_t> triggers) { triggers_ = triggers; }
        inline void SetRecos(vector<std::string> recos) { recos_ = recos; }

        inline uint32_t GetEventId() const { return eventId_; }
        inline uint32_t GetRunId() const { return runId_; }
        inline uint16_t GetModJulianDay() const { return mjd_; }
        inline uint16_t GetHealPixNSide() const { return hpix_nside_; }
        inline vector<uint16_t> GetTriggers() const { return triggers_; }
        inline vector<std::string> GetRecos() const { return recos_; }

};

I3_POINTER_TYPEDEFS(I3DSTHeader13);
BOOST_CLASS_VERSION(I3DSTHeader13,i3dstheader13_version_);

#endif
