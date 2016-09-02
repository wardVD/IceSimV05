#ifndef DIPLOPIA_POISSON_H_INLCUDED
#define DIPLOPIA_POISSON_H_INLCUDED

#include <fstream>
#include <string>
#include <set>
#include <icetray/I3Module.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Logging.h>
#include "phys-services/I3RandomService.h"
#include <icetray/I3Units.h>
#include <simclasses/I3MCPE.h>
#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "diplopia/DiplopiaUtils.h"
#include "sim-services/I3GeneratorService.h"

#include <cmath>
#include <assert.h>

/**
 * copyright  (C) 2006
 * The IceCube Collaboration
 *
 * @date: mar feb 16 14:26:39 CST 2010
 * @author Juan Carlos Diaz Velez
 *                                                                       
 *
 * @brief This modules implements the exponential distribution sampling exclusively.
 *	PoissonMerger assigns a random time to each event based on an
 *	exponential distribution with a mean time tau given as a configuration
 *	parameter. If two or more events fall within the time window delta_t, they are
 *	combined into a single event. Events are given an additional weight callde
 *	'DiplopiaWeight' which determines the likely hood of a combined event
 *	given a change in energy spectrum.
 *
 * @todo correctly evaluate weight correction for weighted energy spectrum
 */
class PoissonMerger : public I3Module
{
  /**
   *Constructor
   */
  PoissonMerger();
  PoissonMerger(const PoissonMerger&);

  /**
   *Assignment operator
   */
  PoissonMerger& operator=(const PoissonMerger&);

  /**
   *Names of frame objects to merge
   */
  I3GeneratorServicePtr backgroundService_;
  std::string mcTreeName_;
  std::string separateTree_;

  std::string primaryType_;
  I3Map<unsigned int,unsigned int> coincidenceHistogram_;

  double timeWindow_;
  double corsikaRate_;

  double tau_; // mean time between adjecent events
  char buffer[200];
  unsigned int counter_;
  bool corsika_primary_;

  // queue of events
  std::deque<I3FramePtr> frameBuffer_;

  I3RandomServicePtr randomService_; 

  public:

  /**
   *Constructor
   */
  PoissonMerger(const I3Context& ctx);

  /**
   *Destructor
   */
  virtual ~PoissonMerger() { }

  /**
   * Configures parameters
   */
  void Configure();

  void Finish();

  /**
   * Caches frames and merges MCTrees and MCInfoTrees 
   */
  void DAQ(I3FramePtr frame);

  SET_LOGGER("PoissonMerger");
};

#endif
