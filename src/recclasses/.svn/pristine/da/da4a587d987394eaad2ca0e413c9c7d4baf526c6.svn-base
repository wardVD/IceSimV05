//  pybindings for dst-extractor
//  All the blame goes to Brad Madajczyk

#include <icetray/I3FrameObject.h>
#include <recclasses/I3DST.h>
#include <recclasses/I3DSTHeader.h>
#include <recclasses/I3DST13.h>
#include <recclasses/I3DSTHeader13.h>
#include <recclasses/I3DSTReco13.h>
#include <recclasses/I3DST16.h>
#include <recclasses/I3DSTHeader16.h>
#include <boost/python.hpp>
#include "wrap.h"

void register_I3DST()
{
    using namespace boost::python;

    //  Common DST objects
    wrap::I3FrameObject<I3DST>()
    #define PROPS (Time)(COG)(NHit)(NDOM)(NString)(NDir)(RecoLabel)(TriggerTag)
    #define RECOS (Reco1)(Reco2)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3DST, PROPS)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO_INTERNAL_REFERENCE, I3DST, RECOS)
    ;

    wrap::I3FrameObject<I3DSTHeader>()
    #define HEADER_PROPS (EventId)(RunId)(ModJulianDay)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3DSTHeader, HEADER_PROPS)
    ;

    class_<DSTReco, boost::shared_ptr<DSTReco> >("DSTReco")
    #define RECO_PROPS_RO (X)(Y)(Z) 
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, DSTReco, RECO_PROPS_RO)
    #define RECO_PROPS (Position)(Direction)(Distance) 
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, DSTReco, RECO_PROPS)
    ;

    class_<DSTPosition, boost::shared_ptr<DSTPosition> >("DSTPosition")
    #define POS_PROPS (X)(Y)(Z)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, DSTPosition, POS_PROPS)
    .def("SetPosition", &DSTPosition::SetPosition)
    ;

    //  DST13 objects
    /*
    class_<I3DST13, bases<I3FrameObject>, boost::shared_ptr<I3DST13> >("I3DST13")
    #define PROPS_13 (Time)(COG)(NDOM)(NString)(NDir)(RecoLabel)(TriggerTag)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3DST13, PROPS_13)
    ;
    register_pointer_conversions<I3DST13>();
    */

    class_<I3DSTHeader13, bases<I3DSTHeader>, boost::shared_ptr<I3DSTHeader13> >("I3DSTHeader13")
    #define HEADER_PROPS_13 (EventId)(RunId)(ModJulianDay) (HealPixNSide) 
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3DSTHeader13, HEADER_PROPS_13)
    ;
    register_pointer_conversions<I3DSTHeader13>();

    //  DST16 objects
    wrap::I3FrameObject<I3DST16>()
    #define PROPS_16 (Time)(COG)(NDOM)(NString)(NDir)(RecoLabel)(TriggerTag)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3DST16, PROPS_16)
    ;

    class_<I3DSTHeader16, bases<I3DSTHeader>, boost::shared_ptr<I3DSTHeader16> >("I3DSTHeader16")
    #define HEADER_PROPS_16 (EventId)(RunId)(ModJulianDay) (HealPixNSide) 
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, I3DSTHeader16, HEADER_PROPS_16)
    ;
    register_pointer_conversions<I3DSTHeader16>();

}

