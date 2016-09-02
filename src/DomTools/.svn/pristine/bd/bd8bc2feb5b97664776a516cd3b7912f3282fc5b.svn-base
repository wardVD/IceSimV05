/**
 * copyright  (C) 2005
 * the icecube collaboration
 * Some functionalities of this module have been taken from the Sieglinde SLHitSel(ector) module
 * $Id$
 *
 * @file I3IsolatedHitsCutModule.cxx
 * @version $Revision: 1.45 $
 * @date $Date$
 * @author Ralf Ehrlich
 */

#include <cmath>
#include <dataclasses/I3MapOMKeyMask.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <DomTools/I3IsolatedHitsCutModule.h>

using boost::shared_ptr;

template <class responseClass> 
I3IsolatedHitsCutModule<responseClass>::I3IsolatedHitsCutModule(const I3Context& context) : 
  I3ConditionalModule(context) 
{
  AddOutBox("OutBox");
  
  RTRadius_       = NAN;
  RTTime_         = NAN;
  RTMultiplicity_ = 1;
  UseWidth_       = -1;
  
  AddParameter("InputResponse",
               "Name of the input reco pulse series (no DEFAULT)",
               inputResponse_); 
  AddParameter("OutputResponse",
               "Name of the output reco pulse series (no DEFAULT)",
               outputResponse_); 
  AddParameter("RTRadius",
               "count pulses within this radius towards multiplicity (DEFAULT=NAN) - \
                  NAN=no cut on spacial isolation",
               RTRadius_); 
  AddParameter("RTTime",
               "count pulses within this time range towards multiplicity (DEFAULT=NAN) - \
                  NAN=no cut on temporal isolation",
               RTTime_); 
  AddParameter("RTMultiplicity",
               "reject all pulses that have a smaller count (spacial/temporal) than RTMultiplicity",
               RTMultiplicity_); 
  AddParameter("UseWidth",
               "width in ns to be used for calculation (-1 (default): use width of hit object)",
               UseWidth_);
  AddParameter("Stream", "Stream on which to run", I3Frame::Physics);
}

template <class responseClass> 
void 
I3IsolatedHitsCutModule<responseClass>::Configure()
{
  GetParameter("InputResponse", inputResponse_); 
  GetParameter("OutputResponse", outputResponse_); 
  GetParameter("RTRadius", RTRadius_); 
  RTRadius_=RTRadius_*RTRadius_*I3Units::m2;
  GetParameter("RTTime", RTTime_);
  RTTime_=RTTime_*I3Units::ns; 
  GetParameter("RTMultiplicity", RTMultiplicity_);
  GetParameter("UseWidth",UseWidth_);
  
  I3Frame::Stream stream;
  GetParameter("Stream",stream);
  Register(stream, &I3IsolatedHitsCutModule<responseClass>::HitCleaning);
  
  if (std::isnan(RTRadius_)) 
    log_fatal("You must specify a value for the RTRadius");
  if (std::isnan(RTTime_))   
    log_fatal("You must specify a value for the RTTime");
  
  if(inputResponse_.empty())  
    log_fatal("You must specify a name for the input response");
  if(outputResponse_.empty()) 
    log_fatal("You must specify a name for the output response");
}

template <class responseClass> 
void 
I3IsolatedHitsCutModule<responseClass>::HitCleaning(I3FramePtr frame)
{
  if (!frame->Has(inputResponse_)) {
    PushFrame(frame);
    return;
  }

  const I3Geometry &geometry = frame->template Get<I3Geometry>();
  const I3OMGeoMap &geo = geometry.omgeo;

  const I3Map<OMKey, std::vector<responseClass> >& input = 
    frame->template Get<I3Map<OMKey,std::vector<responseClass> > >(inputResponse_);
  
  shared_ptr<I3Map<OMKey,std::vector<responseClass> > > 
    output(new I3Map<OMKey,std::vector<responseClass> >);

  if (!distPtr || &(distPtr->getGeo())!=&geo){
    log_info("Loading New Geometry");
    distPtr=shared_ptr<I3DistanceMap>(new I3DistanceMap(geo));
  }

  for(I3ResponseIter<responseClass> iter(input); 
      !iter.End() ; 
      iter++){
    const OMKey &key = iter.GetOMKey();
    double t1  = iter.GetTime();
    double w1;
    if ( UseWidth_ == -1)
      w1  = iter.GetWidth();
    else 
      w1  = UseWidth_;
    
    log_debug("Investigating str %d om %d", key.GetString(),key.GetOM());

    //perform hit selection based on spatial and temporal isolation
    
    int multiplicity=0;

    for(I3ResponseIter<responseClass> iter2(input);
        !iter2.End();
        iter2++){
      const OMKey &key2  = iter2.GetOMKey();
      double t2   = iter2.GetTime();
      
      double w2;
      if ( UseWidth_ == -1)
        w2  = iter2.GetWidth();
      else
        w2  = UseWidth_;
      
      log_debug("Comparing to str %d om %d", key2.GetString(),key2.GetOM());

      double distance = distPtr->getDistance(key,key2);
      if(distance>RTRadius_) 
        continue;  //distance too large

      bool dt_too_large=true;
      if(t1<=t2 && t1+w1>=t2) dt_too_large=false;                //second pulse starts "inside" first pulse
      else if(t2<=t1 && t2+w2>=t1) dt_too_large=false;           //first pulse starts "inside" second pulse
      else if(t1<=t2+w2 && t1+w1>=t2+w2) dt_too_large=false;     //second pulse ends "inside" first pulse
      else if(t2<=t1+w1 && t2+w2>=t1+w1) dt_too_large=false;     //first pulse ends "inside" second pulse
      else if(t1<t2 && t2-(t1+w1) < RTTime_) dt_too_large=false; //first pulse ends shortly before second pulse
      else if(t2<t1 && t1-(t2+w2) < RTTime_) dt_too_large=false; //second pulse ends shortly before first pulse
      if(dt_too_large) 
        continue;
      
      multiplicity++;
      if(multiplicity > RTMultiplicity_){
        //copy particular response from input to output
        responseClass response=iter.Get();
        (*output)[key].push_back(response);
        break;
      }
    }
  }
    
  //put the output in the frame
  WriteToFrame(frame, output);
  PushFrame(frame,"OutBox");
}

template <class responseClass>
void I3IsolatedHitsCutModule<responseClass>::WriteToFrame(I3FramePtr frame, 
                                                          shared_ptr<I3Map<OMKey,
                                                          std::vector<responseClass> > > output) {
  frame->Put(outputResponse_,output);
}

template <>
void I3IsolatedHitsCutModule<I3RecoPulse>::WriteToFrame(I3FramePtr frame, 
                                                        I3RecoPulseSeriesMapPtr output) {
  I3RecoPulseSeriesMapMaskPtr mask(new I3RecoPulseSeriesMapMask(*frame, inputResponse_, *output));
  frame->Put(outputResponse_,mask);
}


I3_MODULE(I3IsolatedHitsCutModule<I3RecoPulse>);

