/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id$
 *
 * @file I3Coincify.h
 * @version $Revision:$
 * @date $Date$
 */

#ifndef DOMTOOLS_I3COINCIFY_H_INCLUDED
#define DOMTOOLS_I3COINCIFY_H_INCLUDED

// header files
#include "icetray/I3ConditionalModule.h"
#include "icetray/I3Frame.h"
#include "icetray/I3Units.h"
#include "dataclasses/I3Map.h"
#include "DomTools/I3ResponseIter.h"
#include "DomTools/Utility.h"
#include <string>
#include <cmath>

/**
 * @brief IceTray module to apply a Local Coincidence window 
 * to I3RecoPulse, I3RecoHit, I3MCHit, I3DOMLaunch
 */
template <class responseClass> 
class I3Coincify : public I3ConditionalModule
{
  public:
  I3Coincify(const I3Context& ctx) : I3ConditionalModule(ctx) 
  {
    AddOutBox("OutBox");
  
    outputName_ = "Coincified";
    isoName_ = "";
    coincSpacing_ = 1;
    coincWindow_ = 800.*I3Units::ns;

    AddParameter("OutputName", "Name of the output RecoPulses (DEFAULT=Coincified)",outputName_);
    AddParameter("IsolatedResponseName","Name of the output that contains the responses that fail the coincidence condition (DEFAULT='')",isoName_);
    AddParameter("InputName", "Name of the input RecoPulses (no DEFAULT)",inputName_); 
    AddParameter("CoincidenceNeighbors", "How far to look for neighbors (DEFAULT: nearest only)",coincSpacing_);
    AddParameter("CoincidenceWindow", "Time window for local coincidence (DEFAULT=800 ns)", coincWindow_);

    Multiplicity_ = 1;

    frame_counter_ = 0;
    frame_with_missing_inputName_counter_ = 0;
 
  }

  void Configure()
  {
    GetParameter("OutputName", outputName_); 
    GetParameter("IsolatedResponseName", isoName_);
    GetParameter("InputName", inputName_); 
    GetParameter("CoincidenceNeighbors", coincSpacing_);
    GetParameter("CoincidenceWindow", coincWindow_);

    if (inputName_.empty())  log_fatal("The InputName cannot be empty");
    if (outputName_.empty()) log_fatal("The OutputName cannot be empty");
  }
  
  void Physics(I3FramePtr frame)
  {

    log_debug("(%s) - Commencing Coincification...",GetName().c_str());
  
    boost::shared_ptr<I3Map<OMKey,std::vector<responseClass> > > output(new I3Map<OMKey,std::vector<responseClass> >); // make empty output "hits/launches" frame object
    boost::shared_ptr<I3Map<OMKey,std::vector<responseClass> > > isoOut(new I3Map<OMKey,std::vector<responseClass> >); // make empty isolated "hits/launches" frame object

    frame_counter_++;
    if ( ! frame->Has(inputName_) ) {                      // check to be sure that specified input object exists in the frame before accessing it 
       frame_with_missing_inputName_counter_++;            // ... if not, report the situation appropriately, put in empty output frame objects, and return
       if ( frame_with_missing_inputName_counter_ == 1 ) {
          log_warn("(%s) - 1ST TIME - Frame %d does not have inputName_ (expected object name \"%s\") ... putting in empty output objects"
                   " ... report of reoccurances may be suppressed",
		    GetName().c_str(),
                    frame_counter_,
                    inputName_.c_str() );
       } else {
          log_debug("(%s) - %d TIME - Frame %d does not have inputName_ (expected object name \"%s\") ... putting in empty output objects",
		    GetName().c_str(),
                    frame_with_missing_inputName_counter_,
                    frame_counter_,
                    inputName_.c_str() );
       }
       frame->Put(outputName_,output);                     // insert empty output "coincified hits or launches" frame object
       if(isoName_ != "") frame->Put(isoName_,isoOut);     // insert empty output "isolated hits/launches" frame object
       PushFrame(frame,"OutBox");                          // push the frame to the next module
       return;                                             // return to invoker
    } // end of IF-block to properly check that the input frame object exists prior to using it

    const I3Map<OMKey, std::vector<responseClass> >& input = frame->template Get<I3Map<OMKey,std::vector<responseClass> > >(inputName_);

    I3ResponseIter<responseClass> iter(input);

    for(;!iter.End();iter++){
      const OMKey& key1 = iter.GetOMKey();
      int    string1 = key1.GetString();
      int    om1     = key1.GetOM();
      double t1      = iter.GetTime()/I3Units::ns;
      double w1      = iter.GetWidth()/I3Units::ns;
      
      log_debug("Investigating str %d om %d", key1.GetString(),key1.GetOM());
      
      int multiplicity = 0;
      const I3Map<OMKey, std::vector<responseClass> >& input2 = frame->template Get<I3Map<OMKey,std::vector<responseClass> > >(inputName_);
      for(int om2 = (om1 - coincSpacing_); om2 <= (om1 + coincSpacing_); om2++){

        OMKey key2(string1, om2);
	if(key2 == key1) continue;
        if(input2.find(key2) == input2.end()) continue; //om not found
	
        const std::vector<responseClass>& responseSeries2=input2.find(key2)->second;
        typename std::vector<responseClass>::const_iterator iter2;
	
        for(iter2=responseSeries2.begin(); iter2!=responseSeries2.end(); iter2++){
	  
          double t2  = domtools::GetTime(*iter2)/I3Units::ns;
          double w2  = domtools::GetWidth(*iter2)/I3Units::ns;
	  
          log_debug("Comparing to str %d om %d", key2.GetString(),key2.GetOM());
	  
          if(!std::isnan(coincWindow_)){ 
            bool dt_too_large=true;
            if(t1<=t2 && t1+w1>=t2) dt_too_large=false;        //second pulse starts "inside" first pulse
            if(t2<=t1 && t2+w2>=t1) dt_too_large=false;        //first pulse starts "inside" second pulse
            if(t1<=t2+w2 && t1+w1>=t2+w2) dt_too_large=false;  //second pulse ends "inside" first pulse
            if(t2<=t1+w1 && t2+w2>=t1+w1) dt_too_large=false;  //first pulse ends "inside" second pulse
            if(t1<t2 && t2-(t1+w1) < coincWindow_) dt_too_large=false; //first pulse ends shortly before second pulse
            if(t2<t1 && t1-(t2+w2) < coincWindow_) dt_too_large=false; //second pulse ends shortly before first pulse
            if(dt_too_large) continue;
          }
	  
          multiplicity++;   //a hit that satisfies the requirements survived
          log_trace("multiplicity is now %d",multiplicity);
          if(multiplicity >= Multiplicity_) break; //no need to check for more hits - multiplicty requirement already satisfied
        }
        if(multiplicity >= Multiplicity_) break; //no need to check for more hits - multiplicty requirement already satisfied
      }
      
      if(multiplicity < Multiplicity_){
        log_debug("This response doesn't pass the test!");
	if(isoName_ != ""){
	  responseClass response=iter.Get();
	  (*isoOut)[key1].push_back(response);
	}
        continue;
      }

      log_debug("This response passes the test!");
      
      //copy particular response from input to output
      responseClass response=iter.Get();
      (*output)[key1].push_back(response);
    }
    //put the output in the frame
    frame->Put(outputName_,output);
    if(isoName_ != "") frame->Put(isoName_,isoOut);
    
    PushFrame(frame,"OutBox");
  }
  
  private:
  I3Coincify();
  I3Coincify(const I3Coincify& source);
  I3Coincify& operator=(const I3Coincify& source);
  std::string inputName_, outputName_, isoName_; 
  double coincWindow_;
  int    coincSpacing_;
  int    Multiplicity_;

  int    frame_counter_;                        // counter of frames, used in debug/error messages
  int    frame_with_missing_inputName_counter_; // counter for number of events for which the inputName_ frame object was not found

  SET_LOGGER("I3Coincify");

};  // end of class I3Coincify

#endif //I3COINCIFY_H_INCLUDED
