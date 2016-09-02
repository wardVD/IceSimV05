#ifndef DOMLAUNCHER_I3INICEDOM_H_INCLUDED
#define DOMLAUNCHER_I3INICEDOM_H_INCLUDED

#include "DOMLauncher/I3DOM.h"

class OMKey;
class I3RandomService;
class I3DOMLaunch;
class I3DOMCalibration;
class I3DOMStatus;
class I3OMGeo;
class DOMTrigger;
/**
  * class: I3InIceDOM
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
class I3InIceDOM : public I3DOM {
public:
    
    I3InIceDOM();
    I3InIceDOM(boost::shared_ptr<I3RandomService>, const OMKey& om);
    I3InIceDOM(boost::shared_ptr<I3RandomService> rng, 
               const OMKey& om, 
               double &globlaTime,
               domlauncherutils::PulseTemplateMap &speTemplateMap);
    
    ///Correction time of the LC time window due to finite LC signal speed for regular InIce DOMs
    static const double cableCorrectionInIce;

    ///Correction time of the LC time window due to finite LC signal speed for DeepCore DOMs
    static const double cableCorrectionDeepCore;
    
    /// Configures the I3InIceDOM with information from the GCD file
    bool Configure(const I3DOMCalibration&, const I3DOMStatus&);
    void CreateLCLinks(const I3DOMMap&);

protected:
    
    void MakeCoarseChargeStamp(const dlud::DiscCross&, int chip, I3DOMLaunch&);
    
};

typedef boost::shared_ptr<I3InIceDOM> I3InIceDOMPtr;

#endif
