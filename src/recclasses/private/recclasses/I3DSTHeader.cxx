/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: dst.cxx 16031 2006-02-20 12:38:45Z troy $

    @version $Revision: 1.2 $
    @date $Date: 2006-02-20 06:38:45 -0600 (lun, 20 feb 2006) $
    @author juancarlos@icecube.wisc.edu
*/

#include <icetray/serialization.h>
#include <recclasses/I3DSTHeader.h>

#include <iostream>
#include <cmath>

using std::cout;

I3DSTHeader::I3DSTHeader():
		runId_(0),
		eventId_(0),
		mjd_(0)
{ }



template <class Archive>
void
I3DSTHeader::serialize(Archive& ar,unsigned version)
{
  if (version > i3dstheader09_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DSTHeader class.", 
                    version,i3dstheader09_version_);
  if (version >=  i3dstheader08_version_) {
     ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
     ar & make_nvp("runId",runId_);
     ar & make_nvp("eventId",eventId_);
     ar & make_nvp("mjd",mjd_);
  } else if (version ==  i3dstheader07_version_) {
     ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
     ar & make_nvp("version",version_);
     ar & make_nvp("year",year_);
     ar & make_nvp("runId",runId_);
     ar & make_nvp("eventId",eventId_);
     ar & make_nvp("time",time_);
     ar & make_nvp("idtheta",idtheta_);
     ar & make_nvp("idphi",idphi_);
     ar & make_nvp("coord_digits",coord_digits_);
     ar & make_nvp("dist_precision",dist_precision_);
  }
}

I3_SERIALIZABLE(I3DSTHeader);
