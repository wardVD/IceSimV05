#include "recclasses/I3OpheliaParticle.h"
#include "recclasses/I3OpheliaFirstGuessTrack.h"
#include "recclasses/I3OpheliaRecoResult.h"
#include "../recclasses/converter/I3OpheliaFirstGuessTrackConverter.h"
#include "wrap.h"
#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <tableio/converter/pybindings.h>

namespace boost { namespace python {

  object GetDaughters(const I3OpheliaParticle& op)
  {
    list lst;
    BOOST_FOREACH(I3OpheliaParticlePtr ptr, op.GetDaughters())
      lst.append(ptr);
    return tuple(lst); // return tuple to hint at constness...
  }

  object GetVelocity(const I3OpheliaFirstGuessTrack& t)
  {
    return make_tuple(t.GetVelocityX(),
                      t.GetVelocityY(),
                      t.GetVelocityZ());
  }

  void SetVelocity(I3OpheliaFirstGuessTrack& t, object obj)
  {
    t.SetVelocity(
      extract<double>(obj[0]),
      extract<double>(obj[1]),
      extract<double>(obj[2])
    );
  }

  object GetCenterOfBrightness(const I3OpheliaFirstGuessTrack& t)
  {
    return make_tuple(t.GetCenterOfBrightnessX(),
                      t.GetCenterOfBrightnessY(),
                      t.GetCenterOfBrightnessZ());
  }

  void SetCenterOfBrightness(I3OpheliaFirstGuessTrack& t, object obj)
  {
    t.SetCenterOfBrightness(extract<double>(obj[0]),
                            extract<double>(obj[1]),
                            extract<double>(obj[2]));
  }

  object GetLargestNpeDomPos(const I3OpheliaFirstGuessTrack& t)
  {
    return make_tuple(t.GetLargestNPEDOMpositionX(),
                      t.GetLargestNPEDOMpositionY(),
                      t.GetLargestNPEDOMpositionZ());
  }

  void SetLargestNpeDomPos(I3OpheliaFirstGuessTrack& t, object obj)
  {
    t.SetLargestNPEDOMposition(extract<double>(obj[0]),
                               extract<double>(obj[1]),
                               extract<double>(obj[2]));
  }
}}

void register_I3OpheliaParticle() {
  using namespace boost::python;

  wrap::I3FrameObject<I3OpheliaParticle>()
    .add_property("particle",
      make_function(&I3OpheliaParticle::GetConstI3Particle,
                    return_value_policy<copy_const_reference>()),
      &I3OpheliaParticle::SetI3Particle)
    .def("add_daughter", &I3OpheliaParticle::AddDaughter)
    .def("clear_daughters", &I3OpheliaParticle::ClearDaughters)
    .add_property("daughters", GetDaughters)
  ;

  class_<
    I3OpheliaFirstGuessTrack
    , bases<I3OpheliaParticle>
    , I3OpheliaFirstGuessTrackPtr
  >("I3OpheliaFirstGuessTrack")
    .def(dataclass_suite<I3OpheliaFirstGuessTrack>())
    .add_property("velocity",
                  GetVelocity,
                  SetVelocity)
    .add_property("center_of_brightness",
                  GetCenterOfBrightness,
                  SetCenterOfBrightness)
    .add_property("largest_npe_dom_position",
                  GetLargestNpeDomPos,
                  SetLargestNpeDomPos)
    .add_property("fit_quality",
      &I3OpheliaFirstGuessTrack::GetFitQuality,
      &I3OpheliaFirstGuessTrack::SetFitQuality)
    .add_property("fit_successful",
      &I3OpheliaFirstGuessTrack::IsFitSuccessful,
      &I3OpheliaFirstGuessTrack::SetFitSuccessful)
  ;

  register_pointer_conversions<I3OpheliaFirstGuessTrack>();

  class_<
    I3OpheliaRecoResult
    , bases<I3OpheliaParticle>
    , I3OpheliaRecoResultPtr
  >("I3OpheliaRecoResult")
    .def(dataclass_suite<I3OpheliaRecoResult>())
    .add_property("first_guess_track",
      make_function(&I3OpheliaRecoResult::GetFirstGuessTrack,
                    return_value_policy<copy_const_reference>()),
      &I3OpheliaRecoResult::SetFirstGuessTrack)
    .add_property("stochastic_cascade",
      make_function(&I3OpheliaRecoResult::GetStochasticCascade,
                    return_value_policy<copy_const_reference>()),
      &I3OpheliaRecoResult::SetStochasticCascade)
  ;

  register_pointer_conversions<I3OpheliaRecoResult>();

  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT(I3OpheliaFirstGuessTrackConverter,
                     "Dumps the ophelia particle to the table");
}
