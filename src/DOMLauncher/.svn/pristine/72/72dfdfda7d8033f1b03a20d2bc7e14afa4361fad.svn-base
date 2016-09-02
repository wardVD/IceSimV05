#ifndef DOMLAUNCHER_I3DOM_H_INCLUDED
#define DOMLAUNCHER_I3DOM_H_INCLUDED

// I3 headers
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/geometry/I3OMGeo.h"
#include "dataclasses/calibration/I3DOMCalibration.h"
#include "simclasses/I3MCPulse.h"
//std headers
#include <vector>


#include <boost/weak_ptr.hpp>
//DOMLauncher headers
#include "domlauncherutils.h"
#include "DOM.h"
#include "InterpolatedSPETemplate.h"
namespace dlutils = domlauncherutils;
namespace dlud = dlutils::detail;

/**
 * class: I3DOM
 *
 * @version $Id: $
 *
 * @date: $Date: $
 *
 * @author Samuel Flis <samuel.d.flis@gmail.com>
 *
 * @brief This is the base class for for an IceCube DOM used in
 *        DOMLauncher simulation. The InIce and IceTop DOMS are derived from this class.
 *
 *
 * (c) 2012 IceCube Collaboration
 */

class I3DOM : DOM {

 public:
    ///Default constructor. Thread-unsafe. No random sercice is set.    
    I3DOM();
    
    ///Constructor mostly for python convinience. 
    ///NOTE: It is thread-unsafe since the state of the simulation and the
    ///is static pulse template map are static. It will in particular not work if
    ///one wants two I3DOMs to be identical except one having tabulated pulse templates
    ///and the other non-tabulated pulse templates. 
    ///\param rng a pointer to an I3RandomService
    ///\param om the OMKey/om number of the particular DOM.
    I3DOM(boost::shared_ptr<I3RandomService> rng, const OMKey& om);
    
    ///This is the prefered constructor since it is threadsafe. The user
    /// have to provide references to the state variables.
    ///\param rng a pointer to an I3RandomService
    ///\param om the OMKey/om number of the particular DOM.
    ///\param globalTime a reference to a double that holds the globalTime state.
    ///\param speTemplateMap a reference to a PulseTemplateMap that stores the pulse templates.
    I3DOM(boost::shared_ptr<I3RandomService> rng, 
          const OMKey& om, 
          double &globalTime,
          domlauncherutils::PulseTemplateMap &speTemplateMap);

    virtual ~I3DOM(){}

    ///This function provides a configuration method for an I3DOM. Here all internal
    ///properties and variables should be set using information from the G,C and D frames.
    bool Configure(const I3DOMCalibration&, const I3DOMStatus&, double);

    virtual void CreateLCLinks(const std::map<OMKey, boost::shared_ptr<I3DOM> > &domMap) = 0;

    virtual void MakeCoarseChargeStamp(const dlud::DiscCross&, int chip, I3DOMLaunch&) = 0;

    struct DOMCalibration{
      double frontEndImpedance;
      double atwdGains[3];
      double atwdDeltaT[2];
      double atwdBinCalib[2][3][128];
      double atwdBeaconBaseline[2][128];
      double fadcGain;
      double fadcDeltaT;
      double fadcBeaconBaseline;

      void Configure(const I3DOMCalibration&);
    };

    struct DOMStatus{
      double lcWindowPre;
      double lcWindowPost;
      I3DOMStatus::OnOff statusATWDa;
      I3DOMStatus::OnOff statusATWDb;
      I3DOMStatus::LCMode lcMode;
      I3DOMStatus::LCMode txMode;
      unsigned lcSpan;
      I3DOMStatus::DOMGain domGainType;

      void Configure(const I3DOMStatus&);
    };

    double speMean_;
    double transitTime_;
    double atwdSamplingRate_[2];

    DOMCalibration domCal_;
    DOMStatus domStat_;

    ///The time it takes to readout and digitize one ATWD channel
    static const double ATWDReadoutDigitizeTime;

    ///This is the time it takes the DOM to readout the FADC after an HLC launch. It is
    ///roughly the deadtime of the DOM after an HLC launch if in ping-pong mode.
    static const double HLCReadoutTime;

    ///The time it takes to restart the ATWD after a recording
    static const double ATWDRestartTime;

    ///The time it takes to reset one ATWD channel.
    static const double ATWDClearTime;

    ///Length of an FPGA clock cycle
    static const double clockCycle;

    ///Length of the DOM delay line in ns
    static const double delayLine;

    /// Length of the dead time for LC signals.
    static const double lcWait;

    static const unsigned int nFADCBins;

    static const unsigned int nATWDBins;

    static const unsigned int nFADCCoarseChargeStampBins;

    static const unsigned int ATWDThreshold;

    static const unsigned int digitizerDynamicRange;

    static const double FADCNoiseMean;

    static const double FADCNoiseVariance;

    static const double ATWDNoiseMean;

    static const double ATWDNoiseVariance;

    static double beaconLaunchRate;
    
    double GetDiscriminatorThreshold(){ return discriminatorThreshold_; }
    double GetDiscriminatorThresholdFraction(){
      return discriminatorThresholdFraction_;
    }

    double  GetRapCalTimeShift(){return rapCalShift_;}
    void SetRapCalTimeShift(double shift){rapCalShift_ = shift;}

    void SetUseTabulation(bool tabulation){ useTabulation_ = tabulation; }
    void SetMergePulses(bool merge){ mergePulses_ = merge; }
    void SetUseDroopedTemplate(bool droop){ droopedPulseTemplates_ = droop; }
    
    bool GetUseTabulation(){return useTabulation_;}
    bool GetMergePulses(){return mergePulses_;}
    bool GetUseDroopedTemplate(){return droopedPulseTemplates_;}

    ///This function holds the discriminator simulation. From the input of I3MCPulses,
    ///discriminator crossings are determined. This can in general be done
    void Discriminator(const std::vector<I3MCPulse>&, dlutils::DCStream& dcStream);
    ///A function to feed back triggers previuosly simulated by Discriminator().
    ///
    ///To simulate inter-DOM behavior where the DOMs signal each other, local coincidence (LC)
    ///for instance, this function is used. Triggers are fed back to the I3DOMs in a time ordered
    ///stream.
    void AddTrigger(dlud::DiscCross &);
    ///This function triggers DOMs that have potential triggers left which would cause a launch (only SLC).
    ///In general the launch decision can't be made at the time when the trigger is received if no
    ///neighbor DOMs are in LC. If the I3DOM object doesn't receive more triggers or LC signals in
    ///a frame, the trigger will not result in a DOM launch unless this function is called.
    ///If force is true all potential triggers will cause a launch given that the DOM isn't busy.
    void TriggerLaunch(bool force){ EvaluateLaunchPossibility(false, force); }

    ///This function will add beacon launches between in a specified time window to 
    ///the dc-stream according to the beacon launch rate which is set.
    ///\param startTime the start time of the window
    ///\param endTime the end time of the window
    ///\param dcStream the discriminator crossing stream
    bool AddBeaconLaunches(double startTime, double endTime, dlutils::DCStream& dcStream);
    
    ///Resets all dynamic properties such as deadtimes, stored triggers and launches giving you a
    ///'fresh DOM. No DOM specifc configuration settings are reset!
    ///\param full when true a full reset is done, i.e all properties are reset. Otherwise only the launches are
    ///cleared 
    void Reset(bool full  = true);
    
    bool IsActive(){return !discrXings_.empty();}
    
    const std::vector<dlud::DiscCross>& GetTriggers() const { return discrXings_; }
    const std::vector<I3DOMLaunch>& GetDOMLaunches() const { return domLaunches_; }
    const std::vector<I3DOM* >& GetNeighbors() const
    { return domNeighbors_; }

    std::vector<dlud::DiscCross>& GetTriggers() { return discrXings_; }
    std::vector<I3DOMLaunch>& GetDOMLaunches() { return domLaunches_; }
    std::vector<I3DOM* >& GetNeighbors() { return domNeighbors_; }

    ///Returns the DOM id (an OMKey)
    const OMKey& GetOMKey() const { return domId_; }
    
    domlauncherutils::BufferedWaveformEvaluator waveform_;
//     domlauncherutils::WaveformEvaluator waveform_;
protected:

    boost::shared_ptr<I3RandomService> rng_;

    OMKey domId_;

    /// Stores the actual cable correction used by this particular I3InIceDOM
    double cableCorrection_;

    bool mergePulses_;

    /// Vector containing pointers to DOMs that are physical DOM neighbors of this DOM.
    std::vector<I3DOM* > domNeighbors_;

    ///Pointer to discriminator pulse template
    InterpolatedSPETemplatePtr discSPETemplatePtr_;

    ///Pointers to ATWD pulse templates
    InterpolatedSPETemplatePtr atwdSPETemplatePtr_[3];

    ///Pointer to FADC pulse template
    InterpolatedSPETemplatePtr fadcSPETemplatePtr_;

    /// Stores I3MCPulses for FADC and ATWD digitization
    const std::vector<I3MCPulse>* pulses_;
    
    
    /** \brief Simulates the ATWD digitization and readout.
    *\param trigger The trigger which the launch is based on.
    *\param channel The ATWD channel which is to be digitized
    *\param digitizedReadOut The vector in which to put the digitized waveform
    *\return returns a bool. If  true the ATWD waveform crossed the 2/3 threshold which should
             initiate digitization of the next channel.
    */
    bool ATWDDigitization(const dlud::DiscCross& trigger, int channel,
                          int chip, std::vector<int> &digitizedReadOut);

 private:

    std::map<I3DOMCalibration::DroopedSPETemplate, InterpolatedSPETemplatePtr>
      &speTemplateMap_;
    ///If no pulse template map is provided it will fall back to the static one.  
    static std::map<I3DOMCalibration::DroopedSPETemplate, InterpolatedSPETemplatePtr>
      speTemplateMapStatic_;

    /// This is where most of the launch decisions will be made. This function
    /// also returns the time of the last LC condition of that DOM. DOMs
    /// use this function to check their neighbors if they are in LC.
    /// While returning the LC status it will also go through the DiscCrosss
    /// to find out whether the I3InIceDOM can make any new launch decisions.
    /// Therefore DOMs also use this method on themselves when an added
    /// trigger results in LC with another DOMobject but older potential triggers
    /// still haven't got a decision.
    /// \return time of the last LC condition.
    double CheckLCHigh(){ EvaluateLaunchPossibility(true, false); return lcHighTime_;}
    
    void EvaluateLaunchPossibility(bool dom, bool force);
    
    /// \brief Sets the DroopedSPETemplates and the interpolated templates of the I3InIceDOM
    ///
    /// To save memory when tabulating the DroopedSPETemplates each unique tabulation is only
    /// saved once in a static map:
    ///  std::map<I3DOMCalibration::DroopedSPETemplate, InterpolatedSPETemplate*> speTemplateMap,
    /// containing all unique DroopedSPETemplates. For each DroopedSPETemplate it checks if that
    /// template isn't already in the map. If it isn't, it's added to the map and a pointer is set
    /// to it so the I3InIceDOM can utilize it, otherwise only the pointer is set.
    void FetchSPETemplates(const I3DOMCalibration&);

    /// Update the state of the DOM with respect to time 't'.
    ///
    /// Updates the state of the DOM by checking if the DOM is still busy
    /// doing with digitization or any other DOM operation with respect to the time t.
    void UpdateState(double t);

    /// This function is called when the DOM has taken the decision to launch. The function 
    /// processes the launch by calling the appropriate digitization functions and also 
    /// calculates how long the DOM and the specific ATWD chip will be busy.
    /// \param trigger the trigger which caused the launch. The type of launch (HLC,SLC) and which
    ///        ATWD if(HLC) should be set in the trigger.
    /// \return no return.
    void Launch(const dlud::DiscCross& trigger);

    /** \brief Simulates the FADC digitization and readout.
    *\param trigger The trigger which the launch is based on.
    *\param digitizedReadOut The vector in which to put the digitized waveform
    *\return no return
    */
    void FADCDigitization(const dlud::DiscCross& trigger,
                          std::vector<int> &digitizedReadOut);

    /// Time of the last LC signal.
    double lcHighTime_;

    /// This is the global time which is the time of the last added trigger
    /// from the trigger stream.
    double &globalTime_;
    
    /// If no global time object (double) is provided at construction it will fallback
    /// to this static one. NOTE: this is thread-unsafe.
    static double globalTimeStatic_;
    
    /// This is "local" time which is set with @see UpdateState() when the DOM
    /// iterates through triggers.
    double domTime_;

    /// Indicates if the ATWD-A chip is busy or not with respect to global or DOMtime
    bool ATWDa_alive_;

    /// Indicates if the ATWD-B chip is busy or not with respect to global or DOMtime
    bool ATWDb_alive_;

    /// If true the DOM (I3InIceDOM) is busy and cannot initiate another launch at this time.
    bool busy_;

    /// The time at which the I3InIceDOM gets idle. Is used to determine if the DOM can
    /// initiate a launch.
    double busyTo_;

    /// ATWDa is busy/dead to time = ATWDaDeadTo_;
    double ATWDaDeadTo_;

    /// ATWDb is busy/dead to time = ATWDbDeadTo_;
    double ATWDbDeadTo_;

    ///Max amplitude of the normalized pulse template
    double pulseTemplatePeak_;

    ///The time at which the pulse template has it's maximum
    double timeOfPulseTemplatePeak_;

    ///The threshold of the discriminator. Should be given as a fraction of a PE
    double discriminatorThreshold_;
    double discriminatorThresholdFraction_;

    double discriminatorDelay_;

    ///The uncertainty of the RapCal time calibration for this DOM
    double rapCalUncertainty_;

    ///The actual shift due to the RapCal uncertainty
    double rapCalShift_;

    double currentClockPhase_;
    
    double beaconLaunchPhase_;
    /// Stores discriminator crossing that which haven't caused launch yet.
    std::vector<dlud::DiscCross> discrXings_;

    /// Stores DOMLaunches
    std::vector<I3DOMLaunch> domLaunches_;

    bool useTabulation_;

    bool droopedPulseTemplates_;

};

typedef boost::shared_ptr<I3DOM> I3DOMPtr;
typedef std::vector<I3DOMPtr> I3DOMVector;
typedef std::map<OMKey,I3DOMPtr> I3DOMMap;

#endif
