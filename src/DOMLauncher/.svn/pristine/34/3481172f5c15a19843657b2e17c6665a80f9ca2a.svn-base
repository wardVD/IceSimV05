#ifndef DOMLAUNCHER_DOMLAUNCHERUTILS_H_INCLUDED
#define DOMLAUNCHER_DOMLAUNCHERUTILS_H_INCLUDED

#include "InterpolatedSPETemplate.h"

#include "dataclasses/calibration/I3Calibration.h"
#include "simclasses/I3MCPulse.h"
#include "icetray/OMKey.h"

#include <boost/shared_ptr.hpp>
#include <map>
#include <algorithm>
#include <list>
#include <queue>

#include <iostream>
class I3DOM;
class I3RandomService;

/**
  * namespace: domlauncherutils
  *
  * @version $Id: $
  *
  * @date: $Date: $
  *
  * @author Samuel Flis <samuel.d.flis@gmail.com>
  *
  * \brief Provides tools and objects for DOMLauncher simulation.
  * (c) 2012 IceCube Collaboration
  */

namespace domlauncherutils{
    typedef std::map<I3DOMCalibration::DroopedSPETemplate, InterpolatedSPETemplatePtr>
    PulseTemplateMap;
    ///A struct to store global state variable for the DOMLauncher simulation  
    struct I3DOMGlobals{
        PulseTemplateMap speTemplateMap_;
        double globalTime_;
    };
    
    namespace detail{
        ///Trigger types for the DiscCross object.
        enum LaunchType{HLC, SLC, Discriminator, CPU_REQUESTED};

        ///ATWD chip selector.
        enum ATWDChip{ATWDa = 0, ATWDb = 1};

        ///The DiscCross is a representaion of a discriminator crossing.
        struct DiscCross{

            DiscCross():time(0),type(Discriminator){}

            DiscCross(OMKey i,double s):DOM(i),time(s){}
            ///The DOM which the trigger belongs to.
            OMKey DOM;
            ///Trigger start time.
            double time;
            ///Type of trigger.
            LaunchType type;

            bool operator==(const domlauncherutils::detail::DiscCross& rhs){
                return this->DOM == rhs.DOM
                && this->time == rhs.time
                && this->type == rhs.type;
            }
        };

        /// A helper function to sort DiscCrosss in ascending time (time ordered)
        inline bool dcCompare(DiscCross dc1, DiscCross dc2){
            return (dc1.time < dc2.time);
        }
    }//detail
    
    
    /// \class  WaveformEvaluator
    /// 
    /// \author Samuel Flis <samuel.d.flis@gmail.com
    /// 
    /// \brief A helper class that efficiantly evaluates the waveform composed out of a sereis of MCPulses.
    ///        To speed up waveform evaluation, pulses in the past (default is set to 200 ns) are merged together.
    /// 
    ///  
    class WaveformEvaluator{
    public:

        ///Constructor that defines the properties of the WaveformEvaluator
        ///\param timeBin the size of the bins for the merged pulses
        ///\param padding the time window in which pulses are directly evaluated (instead of merged)
        ///\param cutoffTime the time window in which pulses will be evaluated. Pulses further in the past will be discarded.
        WaveformEvaluator(double timeBin = 25*I3Units::ns, 
                          double padding = 200*I3Units::ns,
                          double cutoffTime = 500*I3Units::microsecond):
        timeBin_(timeBin),
        padding_(padding),
        cutoffTime_(cutoffTime),
        lastCutoffindex_(0),
        pulsesSize_(0){}
        
        void SetPulses(const std::vector<I3MCPulse> &pulses){
            Clear();
            currentPulses_=&pulses;
            double firstPastPulseTime = NAN;
            if(!currentPulses_->empty())
                firstPastPulseTime = (*currentPulses_)[0].time;
            
            std::queue<const I3MCPulse *> nearPastPulses;
            pulsesSize_ = currentPulses_->size();
            for(uint i = 0; i < pulsesSize_; i++){
                const I3MCPulse &pulse = (*currentPulses_)[i];
                if(pulse.time < firstPastPulseTime + timeBin_ ){
                    nearPastPulses.push(&pulse);
                }
                else{
                    MergedPulse tmp;
                    while(!nearPastPulses.empty()){
                        tmp.charge += nearPastPulses.front()->charge;
                        tmp.time += nearPastPulses.front()->time * nearPastPulses.front()->charge ;
                        nearPastPulses.pop();
                    }
                    tmp.lastIndex = i-1;
                    tmp.time/=tmp.charge; //the time of the merged hit is calculated
                    //as a weighted mean, weighted by the charge.
                    mergedPulses_.push_back(tmp);
                    
                    nearPastPulses.push(&pulse);
                    firstPastPulseTime = pulse.time;
                    
                }
            }
            
            if(!nearPastPulses.empty()){
                    MergedPulse tmp;
                    while(!nearPastPulses.empty()){
                        tmp.charge += nearPastPulses.front()->charge;
                        tmp.time += nearPastPulses.front()->time * nearPastPulses.front()->charge ;
                        nearPastPulses.pop();
                    }
                    tmp.lastIndex = currentPulses_->size()-1;
                    tmp.time/=tmp.charge; //the time of the merged hit is calculated
                   
                   //as a weighted mean, weighted by the charge.
                    mergedPulses_.push_back(tmp);
            }
        }
        
        void Clear(){mergedPulses_.clear(); pulsesSize_ = 0;lastCutoffindex_ = 0;}
        
        double WaveFormAmplitude(double time, InterpolatedSPETemplate & templ){
            double amplitude = 0;
            uint startIndex = 0;
            //Check that the last cut off index is still valid
            if(!mergedPulses_.empty())
                lastCutoffindex_ = (time > mergedPulses_[lastCutoffindex_].time) ? lastCutoffindex_ : 0; 

            //First building up the waveform amplitude from merged pulses in the past
            //until the padding window
            for(uint i = lastCutoffindex_; i < mergedPulses_.size(); i++){
                if(time - mergedPulses_[i].time < padding_){
                    if(i>0)
                        startIndex = mergedPulses_[i-1].lastIndex;
                    break;
                }
                
                if(time - mergedPulses_[i].time > cutoffTime_){
                   lastCutoffindex_ = i;
                }
                amplitude += mergedPulses_[i].charge * templ(time - mergedPulses_[i].time);
            }
            
            //Continue to sum the amplitude within the padding window with the
            //actual pulses
            for(uint i = startIndex; i < pulsesSize_; i++){
                //Only consider pulses happening after the padding so that 
                //we don't double count pulses that have been merged
                if(time - (*currentPulses_)[i].time > padding_)
                    continue;
                else if(time - (*currentPulses_)[i].time < 0)//We also don't care about pulses in the future
                    break;
                    
                amplitude += (*currentPulses_)[i].charge * templ(time - (*currentPulses_)[i].time);
            }
            return amplitude;
        }
        
        
    private:
        ///
        struct MergedPulse: public I3MCPulse{
            MergedPulse():I3MCPulse(0,0),lastIndex(0){}
            uint lastIndex;
        };
        
        double timeBin_;
        double padding_;
        const std::vector<I3MCPulse> *currentPulses_;
        std::vector<MergedPulse> mergedPulses_;
        double cutoffTime_;
        uint64_t lastCutoffindex_;
        uint64_t pulsesSize_;
        
    };
  
    
    
  class BufferedWaveformEvaluator{
    public:
        BufferedWaveformEvaluator(double timeBin = 25*I3Units::ns, 
                                  double padding = 200*I3Units::ns,
                                  double cutoffTime = 500*I3Units::microsecond,
                                  double bufferTime = 500*I3Units::microsecond):
              currentWaveform_(timeBin,padding,cutoffTime),
              bufferedWaveform_(timeBin,padding,cutoffTime),
              currentPulses_(0),
              bufferTime_(bufferTime){}
        
        void SetPulses(const std::vector<I3MCPulse> &pulses){
            currentWaveform_.SetPulses(pulses);  
            currentPulses_ = &pulses;
        }
        
        
        double WaveFormAmplitude(double time, InterpolatedSPETemplate & templ){
             return bufferedWaveform_.WaveFormAmplitude(time, templ) + 
             currentWaveform_.WaveFormAmplitude(time, templ);             
        }
        
        void Buffer(double start_time){
            pulseBuffer_.clear();
            bufferedWaveform_.Clear();
            if(currentPulses_ == 0){
                    return;
            }
            const std::vector<I3MCPulse> &cp = *currentPulses_;
            std::list<const I3MCPulse*> tmpPulses; 
            for(int64_t i = cp.size()-1; i>-1; i--){
                if(start_time - cp[i].time > bufferTime_){
                    break;
                }
                tmpPulses.push_front(&cp[i]);
            }  
            pulseBuffer_.resize(tmpPulses.size());
            std::list<const I3MCPulse*>::iterator mit = tmpPulses.begin();
            
            for(uint i = 0; mit != tmpPulses.end(); ++mit, i++){
                  pulseBuffer_[i] = *(*mit);
            }
            
            bufferedWaveform_.SetPulses(pulseBuffer_);
            currentPulses_ = 0;
            currentWaveform_.Clear();
        }
        
        void Clear(){currentPulses_ = 0;currentWaveform_.Clear(); bufferedWaveform_.Clear();}
    private:
        std::vector<I3MCPulse> pulseBuffer_;
        WaveformEvaluator currentWaveform_;
        WaveformEvaluator bufferedWaveform_;
        const std::vector<I3MCPulse> *currentPulses_;
        double bufferTime_;
  };
  typedef std::vector<detail::DiscCross> DCStream;
 
}//domlauncherutils

#endif
