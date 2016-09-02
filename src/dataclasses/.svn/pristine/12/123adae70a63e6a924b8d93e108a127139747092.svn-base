#include <icetray/serialization.h>
#include <dataclasses/physics/I3MCList.h>

I3MCList::~I3MCList() {}

template <class Archive>
  void I3MCList::serialize(Archive& ar, unsigned version)
  {
    if (version>i3mclist_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3MCList class.",version,i3mclist_version_);

    ar & make_nvp("I3MCList", base_object< I3Vector<I3Particle> >(*this));
  }
  

I3_SERIALIZABLE(I3MCList);
