/**
 * class: DOMLauncher (Gyllenstierna)
 *
 * @version $Id: $
 *
 * @date: $Date: $
 *
 * @author Samuel Flis <samuel.d.flis@gmail.com>
 *
 * (c) 2011,2012 IceCube Collaboration
*/

#include <iostream>
#include <fstream>
#include <cmath>

#include <boost/foreach.hpp>

#include <simclasses/I3ParticleIDMap.hpp>

#include "DOMLauncher/DOMLauncher.h"
#include "DOMLauncher/I3DOM.h"
#include "DOMLauncher/I3InIceDOM.h"
#include "DOMLauncher/I3IceTopDOM.h"

using namespace domlauncherutils;
I3_MODULE(DOMLauncher);

DOMLauncher::DOMLauncher(const I3Context& ctx)
  : I3ConditionalModule(ctx),
    domMapInitialized_(false),
    randomServiceName_("I3RandomService"){

    mcPulseSeriesName_ = "I3MCPulseSeriesMap";
    AddParameter("Input","Input is an I3MCPulseSeriesMap.",
                mcPulseSeriesName_);

    domLaunchMapName_ = "I3DOMLaunchSeriesMap";
    AddParameter("Output","The output name of the DOMLaunchSeries.",
                domLaunchMapName_);

    snMode_ = false;
    AddParameter("SuperNovaMode","Outputs discriminators in a I3MapKeyVectorDouble",
                snMode_);

    multiFrameEvents_ = false;
    AddParameter("MultiFrameEvents","Does not reset the DOMs on each frame",
                multiFrameEvents_);

    tabulatePTs_ = true;
    AddParameter("UseTabulatedPT","Pulse templates used in simulation are interpolated for better "
                "speed performance with the cost of worse precision and larger memory consumption.",
                tabulatePTs_);

    mergePulses_ = true;
    AddParameter("MergePulses","If pulses in the past should be merged. Speeds up considerably "
                "if the number of pulses per DOM to process is large.",
                mergePulses_);

    droopedPulseTemplates_ = true;
    AddParameter("DroopedPulses","If drooped SPE pulse templates should be used.",
                droopedPulseTemplates_);

    beaconLaunches_ = true;
    AddParameter("BeaconLaunches","If beacon launches should be simulated.",
                beaconLaunches_);
    
    
    beaconLaunchRate_ = 0.6*I3Units::hertz;
    AddParameter("BeaconLaunchRate","The rate of beacon launches.",
                beaconLaunchRate_);
    
    AddParameter("RandomServiceName",
                "Name of the random service in the context.",
                randomServiceName_);

    AddOutBox("OutBox");
}

void DOMLauncher::Configure(){
    GetParameter("Input",mcPulseSeriesName_);
    GetParameter("Output",domLaunchMapName_);
    GetParameter("SuperNovaMode", snMode_);
    GetParameter("UseTabulatedPT", tabulatePTs_);
    GetParameter("MergePulses", mergePulses_);
    GetParameter("DroopedPulses",droopedPulseTemplates_);
    GetParameter("RandomServiceName",randomServiceName_);
    GetParameter("MultiFrameEvents",multiFrameEvents_);
    GetParameter("BeaconLaunches",beaconLaunches_);
    GetParameter("BeaconLaunchRate",beaconLaunchRate_);
    
    I3DOM::beaconLaunchRate = beaconLaunchRate_;
    
    randomService_ = context_.Get<I3RandomServicePtr>(randomServiceName_);
    if(!randomService_) log_fatal("No random service available");
}

void DOMLauncher::DetectorStatus(I3FramePtr frame){
    const I3DetectorStatus& detectorStatus = frame->Get<I3DetectorStatus>();
    domStatus_ = detectorStatus.domStatus;
    domMapInitialized_ = false;
    PushFrame(frame, "OutBox");
}

void DOMLauncher::Geometry(I3FramePtr frame){
    const I3Geometry& geometry = frame->Get<I3Geometry>();
    domGeo_ = geometry.omgeo;
    domMapInitialized_ = false;
    PushFrame(frame, "OutBox");
}

void DOMLauncher::Calibration(I3FramePtr frame){
    const I3Calibration& calibration = frame->Get<I3Calibration>();
    domCal_ = calibration.domCal;
    domMapInitialized_ = false;
    PushFrame(frame, "OutBox");
}

void DOMLauncher::DAQ(I3FramePtr frame){
    // (Re-)Initializing the DOMMap if a new Geometry, Calibration,
    // or a Detector frame has occured.
    if(!domMapInitialized_) InitilizeDOMMap();

    log_debug("Getting MCPulseSeries.");
    I3MCPulseSeriesMapConstPtr pulseSeriesMap =
        frame->Get<I3MCPulseSeriesMapConstPtr>(mcPulseSeriesName_);

    if(!pulseSeriesMap){
        log_debug("No I3MCPulseSeriesMap found in this frame...skipping.");
        PushFrame(frame);
        return;
    }

    log_debug("Simulating discriminators.");

    domlauncherutils::DCStream dcStream;
    // Simulating the discriminator and so determing the discriminator crossings for the entire
    // event or more generally for an entire frame for each DOM.
    double frameStart = DBL_MAX, frameEnd = -DBL_MAX;
    
    BOOST_FOREACH(I3MCPulseSeriesMap::const_reference kv_pair, *pulseSeriesMap){
        const OMKey& omkey = kv_pair.first;
        const I3MCPulseSeries& pulses = kv_pair.second;

        I3DOMMap::iterator DOMit = domMap_.find(omkey);
        if(DOMit != domMap_.end()){
            //The frame end and start time is needed for calculating
            //the time window of beacon launches.
            if(!pulses.empty()){
                int64_t end = pulses.size()-1;
                frameStart = pulses[0].time < frameStart ? pulses[0].time:frameStart;
                frameEnd   = pulses[end].time > frameEnd ? pulses[end].time:frameEnd;
            }
            
            DOMit->second->Discriminator(pulses, dcStream);
            activeDOMsMap_[omkey] = DOMit->second;
        }
        else{
            log_fatal("PMT pulses exist on DOM %s, but there is no entry in the DOMMap.",
                    omkey.str().c_str());
        }
    }
    
    
    if(frameEnd-frameStart> 60*I3Units::second){
        log_warn("MCPulses cover a time span larger than 1 minute (%lf min)",
                 (frameEnd-frameStart)/I3Units::minute);
    }
    //Adding beacon launches to the simulation.
    if(beaconLaunches_){
        log_debug("Adding beacon launches.");
        BOOST_FOREACH(const I3DOMMap::value_type& pair, domMap_){
            if(pair.second->AddBeaconLaunches(frameStart, frameEnd, dcStream))
                activeDOMsMap_[pair.first] = pair.second;
        }
    }
    
    log_debug("Number of discriminator crossings to process: %d",int(dcStream.size()));

    if(snMode_) DOTOutput(frame, dcStream);

    log_debug("Time ordering discriminator crossings.");
    sort(dcStream.begin(), dcStream.end(), domlauncherutils::detail::dcCompare);
    // The major part of the simulation takes place here. Triggers previously
    // simulated by the Discriminator() function of each I3DOM object have been
    // put in triggerStream, which *needs* time ordered (sorted) so that it
    // becomes a stream. This stream is then fed back to the I3DOM objects which
    // when recieving a trigger depending on the information it gets from its
    // 'neighboring' I3DOM objects decides if it will launch and what kind of
    // launch etc. At the end, all I3DOMs are triggered to launch if they still have
    // some triggers that haven't got any launch decision yet.  The main reason for
    // this happening is the way LC is working in the detector since it is in general
    // impossible to determine at launch time if the LC condition is true.
    log_debug("Simulating LC logic.");
    BOOST_FOREACH(DCStream::value_type& discrx, dcStream){
        if(domMap_.find(discrx.DOM) != domMap_.end())
        domMap_.at(discrx.DOM)->AddTrigger( discrx );
    }
    
    log_debug("Finishing Frame and triggering trailing launches.");
    bool force = !multiFrameEvents_;
    BOOST_FOREACH(const I3DOMMap::value_type& pair, activeDOMsMap_)
        pair.second->TriggerLaunch(force);

    // Transfer all launches in the I3DOMMap to the output I3DOMLaunchSeriesMap
    log_debug("Gather DOMLaunches.");
    // This is the object to be filled and put in to frame
    I3DOMLaunchSeriesMapPtr launchMap(new I3DOMLaunchSeriesMap);
    BOOST_FOREACH(const I3DOMMap::value_type& pair, activeDOMsMap_)
        if(! pair.second->GetDOMLaunches().empty() )
            (*launchMap)[pair.first] = pair.second->GetDOMLaunches();

    log_debug("Putting DOMLaunchSeriesMap to frame.");
    frame->Put(domLaunchMapName_, launchMap);

    // Reset the DOMs in the domMap_ if not in SuperNova mode
    // I'm assuming the SN group wants to be able to combine consecutive
    // frames and doesn't want the DOMs reset on each pass.
    log_debug("Reseting the DOMMap.");
    BOOST_FOREACH(const I3DOMMap::value_type& pair, activeDOMsMap_)
        pair.second->Reset(!multiFrameEvents_);
    // Removing inactive DOMs from the activeDOMsMap. 
    std::map<OMKey, boost::shared_ptr<I3DOM> >::iterator itr = activeDOMsMap_.begin();
    while (itr != activeDOMsMap_.end()) {
        if (! itr->second->IsActive()) {
            activeDOMsMap_.erase(itr++);
        } else {
            ++itr;
        }
    }
        
    log_debug("Pushing frame.");
    PushFrame(frame, "OutBox");
}

/**
 * This function provides Discriminator Over Threshold (DOT) output, a feature which
 * was asked by the sn-wg group as they base their analysis on discriminator crossings.
 */
void DOMLauncher::DOTOutput(I3FramePtr frame, const DCStream& dcStream){
  log_debug("Making DOT output");
  I3MapKeyVectorDoublePtr thresholdCrossingsMap(new I3MapKeyVectorDouble);
  if (dcStream.empty()) return;

  BOOST_FOREACH(const DCStream::value_type& discrx, dcStream)
    (*thresholdCrossingsMap)[discrx.DOM].push_back(discrx.time);

  frame->Put(domLaunchMapName_ + "_DOTOutput", thresholdCrossingsMap);
}

void DOMLauncher::InitilizeDOMMap(){
    log_info("Configuring DOM Object map for DOMLauncher. If tabulation of the SPE pulse templates"
            " is used this can take a while.");
    domMap_.clear();

    BOOST_FOREACH(const I3DOMCalibrationMap::value_type& p, domCal_){
        const OMKey& omkey = p.first;
        const I3DOMCalibration& domcal = p.second;

        I3DOMStatusMap::iterator stat_iter = domStatus_.find(omkey);
        if(stat_iter != domStatus_.end()){
            const I3DOMStatus& domstat = stat_iter->second;
            if (domstat.pmtHV==0.) {
                log_debug("DOM %s is disabled (pmtHV==0). Skipping.", omkey.str().c_str());
                continue;
            } else if ((domstat.pmtHV<0.) || std::isnan(domstat.pmtHV)) {
                log_fatal("DOM %s has invalid pmtHV. (pmtHV=%fV). Fix your GCD file!",
                        omkey.str().c_str(),
                        domstat.pmtHV/I3Units::V);
            }

            I3OMGeoMap::iterator geo_iter = domGeo_.find(omkey);
            if(geo_iter != domGeo_.end()){
                const I3OMGeo& omgeo = geo_iter->second;
                if(omgeo.omtype == I3OMGeo::IceCube ){
                    I3InIceDOMPtr domobj = I3InIceDOMPtr(new I3InIceDOM(randomService_, 
                                                                        omkey,  
                                                                        globalSimState.globalTime_,
                                                                        globalSimState.speTemplateMap_
                                                                        ));
                    domobj->SetUseTabulation(tabulatePTs_);
                    domobj->SetMergePulses(mergePulses_);
                    domobj->SetUseDroopedTemplate(droopedPulseTemplates_);
                    if(domobj->Configure(domcal, domstat))
                        domMap_[omkey] = domobj;
                    else
                        log_error("DOM %s was not successfully configured.  Skipping.",omkey.str().c_str());
                }
                else if(omgeo.omtype == I3OMGeo::IceTop){
                    I3IceTopDOMPtr domobj = I3IceTopDOMPtr(new I3IceTopDOM(randomService_, 
                                                                            omkey,
                                                                            globalSimState.globalTime_,
                                                                            globalSimState.speTemplateMap_
                                                                            ));
                    domobj->SetUseTabulation(tabulatePTs_);
                    domobj->SetMergePulses(mergePulses_);
                    domobj->SetUseDroopedTemplate(droopedPulseTemplates_);
                    if(domobj->Configure(domcal, domstat))
                        domMap_[omkey] = domobj;
                    else
                        log_error("DOM %s was not successfully configured.  Skipping.",omkey.str().c_str());
                }
                else{
                    log_error("DOM %s is of an unsupported type %d!", omkey.str().c_str(), omgeo.omtype);
                }
            }
            else{
                log_error("DOM %s does not exist in the geometry.", omkey.str().c_str());
            }
        }
        else{
            log_debug("DOM %s does not exist in the detector status.", omkey.str().c_str());
        }
    }

    // When the map of DOMs (detector) is filled we go through the map again to
    // set up the LC lines with information from detector status.
    for(I3DOMMap::iterator domMapIterator = domMap_.begin();
        domMapIterator != domMap_.end(); domMapIterator++){
        domMapIterator->second->CreateLCLinks(domMap_);
    }

    domMapInitialized_ = true;
    log_debug("Done initializing.");
}

