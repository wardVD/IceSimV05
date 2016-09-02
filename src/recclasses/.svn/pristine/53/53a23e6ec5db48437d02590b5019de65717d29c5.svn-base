#include <icetray/serialization.h>
#include <recclasses/I3ShieldHitRecord.h>
#include <dataclasses/I3Vector.h>

template <class Archive>
void I3ShieldHitRecord::serialize(Archive& ar, unsigned version){

    if (version>i3shieldhitrecord_version_)
        log_fatal("Attempting to read version %u from file but running version %u of I3ShieldHitRecord class.",version,i3shieldhitrecord_version_);

	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("time", timeResidual);
	ar & make_nvp("dist", distance);
	if (version > 0)
	    ar & make_nvp("DOMkey", DOMkey);
}

bool operator==(const I3ShieldHitRecord& r1, const I3ShieldHitRecord& r2){
	return(r1.GetTimeResidual()==r2.GetTimeResidual() && r1.GetDistance()==r2.GetDistance() && r1.GetDOMkey()==r2.GetDOMkey());
}

I3_SERIALIZABLE(I3ShieldHitRecord);
