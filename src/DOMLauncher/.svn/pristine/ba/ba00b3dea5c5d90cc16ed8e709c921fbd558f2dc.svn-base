#ifndef DOMLAUNCHER_I3ICETOPDOM_H_INCLUDED
#define DOMLAUNCHER_I3ICETOPDOM_H_INCLUDED

#include "DOMLauncher/I3DOM.h"

class OMKey;
class I3RandomService;
class I3DOMLaunch;
class I3DOMCalibration;
class I3DOMStatus;
class I3OMGeo;
class DOMTrigger;
/**
  * class: I3IceTopDOM
  *
  * @version $Id: $
  *
  * @date: $Date: $
  *
  * @author Samuel Flis <samuel.d.flis@gmail.com>
  *
  * \brief A class that provides a software representation of a DOM for the purpose of a detector
  *        response simulation.
  * (c) 2011,2012 IceCube Collaboration
  */

class I3IceTopDOM : public I3DOM {
  public:

  I3IceTopDOM();
  I3IceTopDOM(boost::shared_ptr<I3RandomService>, const OMKey& );
  I3IceTopDOM(boost::shared_ptr<I3RandomService> rng, 
              const OMKey& om, 
              double& globlaTime,
              domlauncherutils::PulseTemplateMap &speTemplateMap);
  ///Correction time of the LC time window due to finite LC signal speed for regular IceTop DOMs
  static const double cableCorrectionIceTop;

  bool Configure(const I3DOMCalibration&, const I3DOMStatus&);
  void CreateLCLinks(const I3DOMMap&);


 protected:

  void MakeCoarseChargeStamp(const dlud::DiscCross&, int chip, I3DOMLaunch&);

};

typedef boost::shared_ptr<I3IceTopDOM> I3IceTopDOMPtr;

#endif
