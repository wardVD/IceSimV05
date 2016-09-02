#ifndef I3_SLOW_MONOPOLE_TRIGGER_H
#define I3_SLOW_MONOPOLE_TRIGGER_H

#include "icetray/I3Module.h"
#include "dataclasses/physics/I3Trigger.h"
#include "dataclasses/physics/I3Particle.h"
#include <dataclasses/geometry/I3Geometry.h>
#include "trigger-sim/algorithms/SlowMPHit.h"
#include "trigger-sim/algorithms/TriggerContainer.h"

/*
 * slow monopole trigger
 */

class SlowMonopoleTrigger : public I3Module
{
public:

    SlowMonopoleTrigger(const I3Context& ctx);
    ~SlowMonopoleTrigger();

    void Configure();
    
    void DAQ(I3FramePtr frame);
    void DetectorStatus(I3FramePtr frame);
    void Finish();

private:

    SlowMonopoleTrigger();
    
    int GetTrigStatusSetting(const I3TriggerStatus&, const std::string&);
    void ConfigureFromDetStatus(const I3DetectorStatus& detstatus);
    

    void RunTrigger(SlowMPHitVector *one_hit_list__, 
		    SlowMPHitVector *two_hit_list__, 
		    double *muon_time_window__,
		    SlowMPHit new_hit, 
		    const I3GeometryConstPtr &geo);
    bool HLCPairCheck(SlowMPHit hit1, SlowMPHit hit2);
    void CheckTriggerStatus(SlowMPHitVector *two_hit_list__, const I3GeometryConstPtr &geo);
    void CheckTriple(SlowMPHit hit1, SlowMPHit hit2, SlowMPHit hit3, 
		     const I3GeometryConstPtr &geo);
    double getDistance(SlowMPHit hit1, SlowMPHit hit2, const I3GeometryConstPtr &geo);
    std::vector<I3ParticlePtr> additional_info(SlowMPHit hit1, SlowMPHit hit2, SlowMPHit hit3);


    TriggerContainerVector trigger_container_vector;
    std::vector<I3Trigger> trigger_list;

    /* Here follow the variables the module needs */
    // Name of dom launch series
    int configIDParam_;
    boost::optional<int> configID_;
    
    std::string triggerName_;
    std::string dataReadoutName_;
    std::string domSetsName_;

    boost::optional<int> domset_;
    // should additional info about the 3-tuples be saved in the frame?
    bool save_additional_info_; 

    TriggerKey triggerKey_;

    double t_proximity_;
    double t_min_; // time difference between two LC pairs
    double t_max_;
    boost::optional<double> deltad_;
    boost::optional<double> alpha_min_;
    double cos_alpha_min_;
    boost::optional<bool> dc_algo_;
    double relv_;
    int min_tuples_;
    double max_event_length_;    

    SET_LOGGER("SlowMonopoleTrigger");

};	// end of class 

#endif //I3_SLOWMP_TRIGGER_NEW_H
