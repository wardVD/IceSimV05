/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: dst.cxx 16031 2006-02-20 12:38:45Z troy $

    @version $Revision: 1.2 $
    @date $Date: 2006-02-20 06:38:45 -0600 (lun, 20 feb 2006) $
    @author juancarlos@icecube.wisc.edu
*/

#include <icetray/serialization.h>
#include <recclasses/I3DSTHeader13.h>

#include <iostream>
#include <cmath>

using std::cout;

I3DSTHeader13::I3DSTHeader13():
		runId_(0),
		eventId_(0),
		mjd_(0)
{ }



template <class Archive>
void
I3DSTHeader13::serialize(Archive& ar,unsigned version)
{
  switch (version) {
    case 5: 
       ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
       ar & make_nvp("runId",runId_);
       ar & make_nvp("eventId",eventId_);
       ar & make_nvp("mjd",mjd_);
       ar & make_nvp("HealpixNSide",hpix_nside_);
       ar & make_nvp("Triggers",triggers_);
       break;
    default: 
       if (version > i3dstheader13_version_)
           log_fatal("Attempting to read version %u from file but running version %u of I3DSTHeader class.", 
                    version,i3dstheader13_version_);
       ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
       ar & make_nvp("runId",runId_);
       ar & make_nvp("eventId",eventId_);
       ar & make_nvp("mjd",mjd_);
       ar & make_nvp("HealpixNSide",hpix_nside_);
       ar & make_nvp("Triggers",triggers_);
       ar & make_nvp("RecosList",recos_);
   }
}

I3_SERIALIZABLE(I3DSTHeader13);
