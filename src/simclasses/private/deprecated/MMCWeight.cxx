#include <icetray/serialization.h>
#include "icetray/I3FrameObject.h"

/**
 * @brief A class to store weights from MMC when cross-section reweighting is turned on
 */
class MMCWeight : public I3FrameObject{

public:
  double weight;
  double distToModIntPoint;
  virtual ~MMCWeight(){};

 private:

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned version);

};

template <class Archive>
void MMCWeight::serialize(Archive& ar, unsigned version)
{

  ar & make_nvp("I3FrameObject",     base_object<I3FrameObject>(*this));
  ar & make_nvp("weight",            weight );
  ar & make_nvp("distToModIntPoint", distToModIntPoint );

}

BOOST_CLASS_VERSION(MMCWeight, 1);
I3_SERIALIZABLE(MMCWeight);
