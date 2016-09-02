/**
    $Id$
    copyright  (C) 2007
    the icecube collaboration

    @date $Date: 2008-02-18 $
    @author juancarlos@icecube.wisc.edu
*/

#ifndef DST_I3DSTHEADER_09_H_INCLUDED
#define DST_I3DSTHEADER_09_H_INCLUDED

#include <icetray/I3Units.h>
#include <icetray/I3FrameObject.h>
#include <recclasses/I3DST.h>
#include <map>


using namespace std;
static const unsigned i3dstheader07_version_ = 0;
static const unsigned i3dstheader08_version_ = 3;
static const unsigned i3dstheader09_version_ = 3;

/**
 *  @brief Stores information relevant for I3DST events that follow it (see project dst-extractor).
 * 
 *  The information includes things such as runId, year, driving time,
 *  and parameters for building the binned sky map.
 */
class I3DSTHeader: public I3FrameObject
{
    private:
        // data elements
        uint32_t runId_;        // (4 B)
        uint32_t eventId_;      // (4 B) 
        uint16_t mjd_;          // (2 B)

    public:

        // legacy
        uint8_t  version_;
        uint8_t  year_;   
        uint32_t time_;   
        uint16_t idtheta_;   
        uint16_t idphi_;   
        uint8_t  coord_digits_;   
        uint8_t  dist_precision_;   


        I3DSTHeader();

        // in order to serialize object
        template <typename Archive> void serialize(Archive&,unsigned);

        inline void SetEventId(uint32_t eventId) { eventId_ = eventId; }
        inline void SetRunId(uint32_t runId) { runId_ = runId; }
        inline void SetModJulianDay(uint16_t mjd) { mjd_ = mjd; }

        inline uint32_t GetEventId() const { return eventId_; }
        inline uint32_t GetRunId() const { return runId_; }
        inline uint64_t GetModJulianDay() const { return mjd_; }
};

I3_POINTER_TYPEDEFS(I3DSTHeader);
BOOST_CLASS_VERSION(I3DSTHeader,i3dstheader09_version_);

#endif
