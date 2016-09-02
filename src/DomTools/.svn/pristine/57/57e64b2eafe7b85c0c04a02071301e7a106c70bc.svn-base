/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id$
 *
 * @file I3TimeWindowCleaning.h
 * @version $Revision:$
 * @date $Date$
 */

#ifndef DOMTOOLS_I3TIMEWINDOWCLEANING_H_INCLUDED
#define DOMTOOLS_I3TIMEWINDOWCLEANING_H_INCLUDED

#include <icetray/I3ConditionalModule.h>
#include <icetray/OMKey.h>

#include <icetray/I3Frame.h>
#include <vector>
#include <dataclasses/I3Map.h>
#include <dataclasses/I3MapOMKeyMask.h>
#include <sstream>
#include <DomTools/Utility.h>



template <class SignalType>
class I3TimeWindowCleaning : public I3ConditionalModule
{ 
  SET_LOGGER("I3TimeWindowCleaning");

 public:
  I3TimeWindowCleaning(const I3Context& context);

  void Physics(I3FramePtr frame);

  void Configure();

 private:
  double timeWindow_;
  bool useCharge_;
  std::string inputResponseName_;
  std::string outputResponseName_;
  
  struct HitDomPair
  {
    OMKey key;
    SignalType hit;
    bool kept;

    HitDomPair() : key(),hit(),kept(false){}
  };

  static bool sortHits(const HitDomPair& lhs,const HitDomPair& rhs)
  {
    return domtools::GetTime(lhs.hit) < domtools::GetTime(rhs.hit);
  }

  /*
   * Private functor to find the first hit which time is bigger
   * then a specified time in the classes constructor.
   */
  class HitTime_greater : public std::unary_function<HitDomPair, bool>
  {
    public:
      explicit HitTime_greater( double time ) : minTime_(time) {}
      bool operator() (const HitDomPair& hdPair)
      {
        return ( domtools::GetTime( hdPair.hit ) > minTime_ );
      }
    private:
      double minTime_;
  };
    
  static std::string logTraceKeptHits(std::vector<HitDomPair>&);
};



template <class SignalType>
I3TimeWindowCleaning<SignalType>::I3TimeWindowCleaning(const I3Context& context) : 
  I3ConditionalModule(context),
  timeWindow_(4000),
  useCharge_(false),
  inputResponseName_("hits"),
  outputResponseName_("hitsClean")
{
  AddOutBox("OutBox");
  
  AddParameter("InputResponse","The detector response object to read in",
               inputResponseName_);
  AddParameter("OutputResponse","The detector response object to read in",
               outputResponseName_);
  AddParameter("TimeWindow","The window that is used in cleaning out DOMs.",
               timeWindow_);
  AddParameter("UseCharge","If true, the charge will be used to determine the "
                           "best time window, if false, pulses are merily counted. "
                           "Has no influence if not used with I3RecoPulses.",
               useCharge_);
}



template <class SignalType>
void I3TimeWindowCleaning<SignalType>::Configure()
{
  GetParameter("InputResponse",
               inputResponseName_);
  GetParameter("OutputResponse",
               outputResponseName_);
  GetParameter("TimeWindow",
               timeWindow_);
  GetParameter("UseCharge",
               useCharge_);
  if(useCharge_)
  {
    log_warn("UseCharge is set to True but has no effect because charge "
             "is only available for I3RecoPulses.");
  }
}

// specialization for I3RecoPulses; here, UseCharge is not useless
template <>
void I3TimeWindowCleaning<I3RecoPulse>::Configure()
{
  GetParameter("InputResponse",
               inputResponseName_);
  GetParameter("OutputResponse",
               outputResponseName_);
  GetParameter("TimeWindow",
               timeWindow_);
  GetParameter("UseCharge",
               useCharge_);
}

template <class SignalType>
void I3TimeWindowCleaning<SignalType>::Physics(I3FramePtr frame)
{
  if (!frame->Has(inputResponseName_))
  {
    log_debug("Frame does not contain the necessary input response.");
    PushFrame(frame,"OutBox");
    return;
  }
  
  // 'template' keyword is work-around for gcc 3.2 compiler bug
  const I3Map<OMKey,std::vector<SignalType> >& hits = 
    frame->template Get<I3Map<OMKey,std::vector<SignalType> > >(inputResponseName_);
  
  boost::shared_ptr<I3Map<OMKey,std::vector<SignalType> > > 
    newMap(new I3Map<OMKey,std::vector<SignalType> > ());
  
  std::vector<HitDomPair> extractedHits;
  extractedHits.reserve(100);
  
  log_trace("extracting hits into my own structure");
  for(typename I3Map<OMKey,std::vector<SignalType> >::const_iterator iter = 
      hits.begin(); iter != hits.end(); ++iter)
  {
    for(typename std::vector<SignalType>::const_iterator thisHit = 
       iter->second.begin(); thisHit != iter->second.end(); ++thisHit)
    {
      if(std::isfinite(domtools::GetTime(*thisHit)))
      {
        HitDomPair thehit;
        thehit.key = iter->first;
        thehit.hit = *thisHit;
        extractedHits.push_back(thehit);
      }
    }
  }
  
  log_trace("sorting them by time");
  sort(extractedHits.begin(),extractedHits.end(),sortHits);
  log_trace("done sorting them");
  typename std::vector<HitDomPair>::iterator windowStart=extractedHits.begin();
  
  // We select the inital last hit as being the hit which still fits into the
  // time window which begins with the first hit. 
  double maxTime = domtools::GetTime(windowStart->hit) + timeWindow_;
  typename std::vector<HitDomPair>::iterator windowEnd =
    find_if( extractedHits.begin(), extractedHits.end(), HitTime_greater(maxTime) ) - 1;
  
  typename std::vector<HitDomPair>::iterator bestWindowStart;
  typename std::vector<HitDomPair>::iterator bestWindowEnd;
  int bestWindowCount = -1;
  
  if(extractedHits.size() > 0)
  {
    log_trace("gonna find the best hits");
    
    // move along hits.  make window of last timeWindow_ ns and
    // consider if it has the max number of hits in timeWindow_
    while(windowEnd != extractedHits.end())
    {
      double window = 
        domtools::GetTime(windowEnd->hit) - domtools::GetTime(windowStart->hit);
      log_trace("current window: %f",window);
      
      if(window > timeWindow_)
      {
        log_trace("incrementing the window start time");
        
        // find a new starting hit by searching the first hit which would make the
        // timewindow again smaller than the maximum allowed time window
        double minTime = domtools::GetTime(windowEnd->hit) - timeWindow_;
        windowStart = find_if( windowStart, windowEnd, HitTime_greater(minTime) );
        
        window = domtools::GetTime(windowEnd->hit) - 
        domtools::GetTime(windowStart->hit);
      }
      
      // Get the number of hits in the time window.
      // As the iterators are random number iterators, we can calculate
      // the number of hits using the overloaded subtraction operator.
      // We need to ad one, as the result is a onesided open range.
      int thisWindowCount = windowEnd - windowStart + 1;
      
      if(thisWindowCount > bestWindowCount)
      {
        log_trace("found a better number %d in window %f, %f",
        thisWindowCount,
        domtools::GetTime(windowStart->hit),
        domtools::GetTime(windowEnd->hit));
        
        bestWindowStart = windowStart;
        bestWindowEnd = windowEnd;
        bestWindowCount = thisWindowCount;
      }
      
      ++windowEnd;
    }
    
    log_trace("final hit window and number of hits: %f to %f, %d hits",
              domtools::GetTime(bestWindowStart->hit),
              domtools::GetTime(bestWindowEnd->hit),
              bestWindowCount);
    
    
    // gotta make this point one past the window just like vector::end()
    ++bestWindowEnd;
    
    for(typename std::vector<HitDomPair>::iterator iter = bestWindowStart; 
        iter != bestWindowEnd; ++iter)
    {
      (*newMap)[iter->key].push_back(iter->hit);
      iter->kept = true;
    }
  }
  
  log_trace("%s", logTraceKeptHits(extractedHits).c_str());
  
  frame->Put(outputResponseName_,newMap);
  
  PushFrame(frame,"OutBox");
}



// specialization for I3RecoPulses; necessary as hits do not have charge
// template <class SignalType>
template <>
void I3TimeWindowCleaning<I3RecoPulse>::Physics(I3FramePtr frame)
{
  if (!frame->Has(inputResponseName_))
  {
    log_debug("Frame does not contain the necessary input response.");
    PushFrame(frame,"OutBox");
    return;
  }
  
  const I3RecoPulseSeriesMap& hits = 
    frame->Get<I3RecoPulseSeriesMap>(inputResponseName_);
    
  I3RecoPulseSeriesMapPtr newMap(new I3RecoPulseSeriesMap);
  
  std::vector<HitDomPair> extractedHits;
  extractedHits.reserve(20);   // insignificant magic number
  
  log_trace("extracting hits into my own structure");
  for(I3RecoPulseSeriesMap::const_iterator iter = 
      hits.begin(); iter != hits.end(); ++iter)
  {
    for(I3RecoPulseSeries::const_iterator thisHit = 
       iter->second.begin(); thisHit != iter->second.end(); ++thisHit)
    {
      if(std::isfinite(domtools::GetTime(*thisHit)))
      {
        HitDomPair thehit;
        thehit.key = iter->first;
        thehit.hit = *thisHit;
        extractedHits.push_back(thehit);
      }
    }
  }
  
  if(extractedHits.empty())
  {
    log_debug("Frame does not contain pulses.");
    I3RecoPulseSeriesMapMaskPtr output(new I3RecoPulseSeriesMapMask(*frame,
        inputResponseName_));
    output->SetNone();
    frame->Put(outputResponseName_,output);
    PushFrame(frame,"OutBox");
    return;
  }
  
  log_trace("sorting them by time");
  sort(extractedHits.begin(),extractedHits.end(),sortHits);
  log_trace("done sorting them");
  std::vector<HitDomPair>::iterator windowStart=extractedHits.begin();
  
  // We select the inital last pulse as being the pulse which still fits into the
  // time window which begins with the first pulse. 
  double maxTime = domtools::GetTime(windowStart->hit) + timeWindow_;
  std::vector<HitDomPair>::iterator windowEnd =
    find_if( extractedHits.begin(), extractedHits.end(), HitTime_greater(maxTime) ) - 1;
  
  std::vector<HitDomPair>::iterator bestWindowStart;
  std::vector<HitDomPair>::iterator bestWindowEnd;
  
  // the variables are also used for the pulse count if useCharge_ == false
  double bestWindowCharge = 0.;
  double windowCharge = 0.;
  // this initialization loop does not include the end pulse,
  // because its charge is added in the next loop
  if(useCharge_)
  {
    for(std::vector<HitDomPair>::const_iterator iter =
        windowStart; iter != windowEnd; ++iter)
    {
       windowCharge += iter->hit.GetCharge();
    }
  }
  else
  {
    windowCharge = windowEnd - windowStart + 1;
  }
  
  if(extractedHits.size() > 0)
  {
    log_trace("gonna find the best hits");
    
    // move along pulses.  make window of last timeWindow_ ns and
    // consider if it has the max number of hits in timeWindow_
    while(windowEnd != extractedHits.end())
    {
      if(useCharge_)
        windowCharge += windowEnd->hit.GetCharge();
      else
        ++windowCharge;
      
      double windowEndTime = windowEnd->hit.GetTime();
      double window = windowEndTime - windowStart->hit.GetTime();
      log_trace("current window: %f",window);
      
      while(window > timeWindow_)
      {
        log_trace("incrementing the window start time");
        
        if(useCharge_)
          windowCharge -= windowStart->hit.GetCharge();
        else
          --windowCharge;
        ++windowStart;
        window = windowEndTime - windowStart->hit.GetTime();
      }
      
      if(windowCharge > bestWindowCharge)
      {
        log_trace("found a better charge/count value %f in window %f, %f",
                  windowCharge,
                  domtools::GetTime(windowStart->hit),
                  domtools::GetTime(windowEnd->hit));
        
        bestWindowStart = windowStart;
        bestWindowEnd = windowEnd;
        bestWindowCharge = windowCharge;
      }
      
      ++windowEnd;
    }
    
    log_trace("final window and total charge/count in window: %f to %f, %f PE",
              domtools::GetTime(bestWindowStart->hit),
              domtools::GetTime(bestWindowEnd->hit),
              bestWindowCharge);
    
    
    // gotta make this point one past the window just like vector::end()
    ++bestWindowEnd;
    
    for(std::vector<HitDomPair>::iterator iter = bestWindowStart; 
        iter != bestWindowEnd; ++iter)
    {
      (*newMap)[iter->key].push_back(iter->hit);
      iter->kept = true;
    }
  }
  
  log_trace("%s", logTraceKeptHits(extractedHits).c_str());

  I3RecoPulseSeriesMapMaskPtr output(new I3RecoPulseSeriesMapMask(*frame,
    inputResponseName_, *newMap));
  frame->Put(outputResponseName_,output);
  
  PushFrame(frame,"OutBox");
}



template <class SignalType>
std::string I3TimeWindowCleaning<SignalType>::
logTraceKeptHits(std::vector<HitDomPair>& extractedHits)
{
  log_trace("Dumping cleaned hits");
  for(typename std::vector<HitDomPair>::iterator iter = extractedHits.begin(); 
      iter != extractedHits.end(); ++iter)
  {
    std::ostringstream message;
    message << iter->key << " " << domtools::GetTime(iter->hit);
    if(iter->kept)
      message << "<---- kept";
    else
      message << "<---- cleaned";
    log_trace("%s",message.str().c_str());
  }  
  return "^------- That's a summary of the hit cleaning";
}

#endif
