#ifndef COINC_AFTER_PROCESSING_H
#define COINC_AFTER_PROCESSING_H
/**
 * class: CoincidenceAfterProcessing
 *
 * Version $Id: $
 *
 * date: $Date: $
 *
 * @author Gluesenkamp
 *
 * (c) 2012 IceCube Collaboration
 */

#include <icetray/I3Module.h>
#include <string>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <dataclasses/physics/I3EventHeader.h>

class CoincidenceAfterProcessing : public I3Module
{
  CoincidenceAfterProcessing();
  CoincidenceAfterProcessing(const CoincidenceAfterProcessing&);
  CoincidenceAfterProcessing& operator=(const CoincidenceAfterProcessing&);

public:

  const static I3Frame::Stream Simulation;

  CoincidenceAfterProcessing(const I3Context&);
  
  ~CoincidenceAfterProcessing() {};

  void Configure();

  void DAQ(I3FramePtr frame);

private:

  std::string input_mcpulses_;
  std::string input_mcpes_;
  std::string particle_id_map_name_;
  std::string input_domlaunches_;
  std::string input_triggerhierarchy_;
  std::string input_mctree_;
  std::string input_mmctracklist_;
  int signal_nch_minimum_;
  bool enforce_primaries_;
  double fluxsum0_;
  std::string drivingtime_name_;
  int event_id_increment_;
  double total_timescale_;
  
  std::vector<I3FramePtr> all_frames_;
  std::vector<std::string> frame_keys_;
     
  I3DetectorStatusConstPtr detector_status_;
  
  void ProcessCorsika(I3FramePtr frame);
  void ProcessNugenCorsika(I3FramePtr frame);
  
  void AddDTimeAndHeader(I3FramePtr frame_to_add, 
                         I3EventHeaderConstPtr event_header, 
                         double first_trig_time, 
                         double trig_length, 
                         double very_first_trig_start, 
                         I3Time dtime);
                         
  
  void AddDomLaunches(I3FramePtr frame_to_add, 
                      I3DOMLaunchSeriesMapConstPtr original_launchmap, 
                      double first_trig_time, 
                      double last_trig_time);


  void GetReadoutWindows(I3TriggerHierarchyPtr t_hierarchy, 
                         std::vector< std::pair<double, double> >& readout_windows);

  void copy_additional_frame_objects(I3FramePtr frame, I3FramePtr subframe);
  SET_LOGGER("CoincidenceAfterProcessing");
};

const I3Frame::Stream CoincidenceAfterProcessing::Simulation('S');

#endif //COINC_AFTER_PROCESSING_H
