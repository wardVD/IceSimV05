#ifndef DOMLAUNCHER_DOMLAUNCHER_H_INCLUDED
#define DOMLAUNCHER_DOMLAUNCHER_H_INCLUDED

// I3 headers
#include "icetray/I3ConditionalModule.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "phys-services/I3GSLRandomService.h"
#include "simclasses/I3MCPulse.h"

//DOMLauncher headers
#include "domlauncherutils.h"

namespace dlud = domlauncherutils::detail;
/**
 * class: DOMLauncher (Gyllenstierna)
 *
 * @version $Id: $
 *
 * @date: $Date: $
 *
 * @author Samuel Flis <samuel.d.flis@gmail.com>
 *
 * @brief A module that simulates the detector response.
 *
 * (c) 2011,2012 IceCube Collaboration
 */
class DOMLauncher : public I3ConditionalModule{
  public:

  DOMLauncher(const I3Context&);

  ~DOMLauncher() {};

  void Configure();
  void DAQ(I3FramePtr);
  void Calibration(I3FramePtr);
  void Geometry(I3FramePtr);
  void DetectorStatus(I3FramePtr);

 private:

  /// Extracts Discriminator Over Threshold (DOT) times from triggerStream_ and
  /// puts in Frame as an MCPulseSeriesMap.
  void DOTOutput(I3FramePtr, const domlauncherutils::DCStream& );
  void InitilizeDOMMap();

  DOMLauncher();

  DOMLauncher(const DOMLauncher&);

  DOMLauncher& operator=(const DOMLauncher&);

  /// Is DOM Map initialized
  bool domMapInitialized_;

  /// SuperNova Mode :
  /// Outputs discriminator over threshold (DOT) times
  bool snMode_;

  ///Does not reset the DOMs with each new frame.
  bool multiFrameEvents_;

  /// Pulse templates used in simulation are interpolated for better speed
  /// with the cost of worse precision and larger memory consumption.
  bool tabulatePTs_;

  /// If pulses in the past should be merged. Speeds up considerably if the
  /// number of pulses to process per DOM and frame is large.
  bool mergePulses_;

  /// If drooped SPE pulse templates should be used.
  bool droopedPulseTemplates_;
  
  /// If beacon launches should be simulated.
  bool beaconLaunches_;
  /// Rate of beacon launches.
  double beaconLaunchRate_;
  
  /// The name of the DOMLaunchSeriesMaps to be produced.
  std::string domLaunchMapName_;

  /// The name of the MCPulseSeriesMap to be processed.
  std::string mcPulseSeriesName_;

  std::map<OMKey, I3OMGeo> domGeo_;
  std::map<OMKey, I3DOMCalibration> domCal_;
  std::map<OMKey, I3DOMStatus> domStatus_;

  I3RandomServicePtr randomService_;

  std::map<OMKey, boost::shared_ptr<I3DOM> > domMap_;
  
  std::map<OMKey, boost::shared_ptr<I3DOM> > activeDOMsMap_;
  
  domlauncherutils::I3DOMGlobals globalSimState;
  
  ///The ID of the random service to use
  std::string randomServiceName_;
    
  SET_LOGGER("DOMLauncher");

};


#endif
