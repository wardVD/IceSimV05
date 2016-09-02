#ifndef MP_I3POLYPLOPIAEXP_H_INLCUDED
#define MP_I3POLYPLOPIAEXP_H_INLCUDED

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
 *	I3PolyplopiaExp assigns a random time to each event based on an
 *	exponential distribution with a mean time tau given as a configuration
 *	parameter. If two or more events fall within the time window delta_t, they are
 *	combined into a single event. Events are given an additional weight callde
 *	'DiplopiaWeight' which determines the likely hood of a combined event
 *	given a change in energy spectrum.
 *
 * @todo correctly evaluate weight correction for weighted energy spectrum
 */
class I3PolyplopiaExp : public I3Module
{
  /**
   *Constructor
   */
  I3PolyplopiaExp();
  I3PolyplopiaExp(const I3PolyplopiaExp&);

  /**
   *Assignment operator
   */
  I3PolyplopiaExp& operator=(const I3PolyplopiaExp&);

  /**
   *Names of frame objects to merge
   */
  std::string mcTreeName_;
  std::string mmcTrackName_;
  std::string weightSumName_;
  std::string hitSeriesName_;

  // for legacy support
  bool mergePEs_;

  // this is used for weighting coincidences 
  double fluxsum_r_;

  // times between events will be set with a uniform distribution 
  // around +/- timeWindow_/2
  double timeWindow_; 
  double dynamicWindow_; 
  double tau_; // mean time between adjecent events
  double delta_t_; // mean time between adjecent events
  double clock_; // time relative to drivingTime
  double corsika_rate_; // rate given by counting weights/TimeScale
  unsigned int eventCounter_;
  unsigned int mergedEventCounter_; 
  double fixed_length_interval_;
  double last_zero_diff_; // is needed to shift the zero point of the fixed_length interval
  double zero_diff_sum_;
  double fixed_length_interval_start_;
  
  
  std::string weightMap_;
  char buffer[200];
  std::deque<double> deltas_;

  // queue of events
  std::deque<I3FramePtr> coinc_frames_;

  double timeScaleSingle_;  

  I3RandomServicePtr randomService_; 
  I3Map<std::string,std::string> names_;
  I3Map<unsigned int,unsigned int> coincidenceHistogram_;
  

  public:

  /**
   *Constructor
   */
  I3PolyplopiaExp(const I3Context& ctx);

  /**
   *Destructor
   */
  virtual ~I3PolyplopiaExp() { }

  /**
   * Configures parameters
   */
  void Configure();

  void Finish();

  /**
   * Caches frames and merges MCTrees and MCInfoTrees 
   */
  void DAQ(I3FramePtr frame);
  void Merge(I3FramePtr frame);

  SET_LOGGER("I3PolyplopiaExp");
};

#endif
