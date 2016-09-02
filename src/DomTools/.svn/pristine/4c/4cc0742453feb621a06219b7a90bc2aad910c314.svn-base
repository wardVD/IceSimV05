/**
 * copyright  (C) 2007
 * the IceCube collaboration
 * Version $Id: $
 * Author: wikstrom
 */


#include "DomTools/I3LCCleaning.h"

#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/I3MapOMKeyMask.h>

I3_MODULE(I3LCCleaning);

I3LCCleaning::I3LCCleaning(const I3Context& context)
  : I3ConditionalModule(context),
    inIceInput_("InIceRawData"),
    inIceOutput_("CleanInIceRawData"),
    inIceOutputSLC_(""),
    writeSLCOutput_(false)
{
  AddOutBox("OutBox");
  
  AddParameter("InIceInput",
               "input inice DOMLaunches",
               inIceInput_);
  
  AddParameter("InIceOutput",
               "output inice HLC DOMLaunches",
               inIceOutput_);
  
  AddParameter("InIceOutputSLC",
               "output inice SLC DOMLaunches; \"\" to disable (default)",
               inIceOutputSLC_);
}

void I3LCCleaning::Configure()
{
  GetParameter("InIceInput",
               inIceInput_);
  
  GetParameter("InIceOutput",
               inIceOutput_);
  
  GetParameter("InIceOutputSLC",
               inIceOutputSLC_);
  if(!inIceOutputSLC_.empty()){
    writeSLCOutput_ = true;
  }
}

void I3LCCleaning::DAQ(I3FramePtr frame)
{
    if (!frame->Has(inIceInput_)) {
        PushFrame(frame);
        return;
    }

    I3DOMLaunchSeriesMapConstPtr inIceResponse = 
      frame->Get<I3DOMLaunchSeriesMapConstPtr>(inIceInput_);
    
    I3DOMLaunchSeriesMapPtr newInIceResponse(new I3DOMLaunchSeriesMap);
    I3DOMLaunchSeriesMapPtr newInIceResponseSLC(new I3DOMLaunchSeriesMap);
    
    I3DOMLaunchSeriesMap::const_iterator iter;
    I3DOMLaunchSeries::const_iterator liter;
    for(iter = inIceResponse->begin() ; 
        iter != inIceResponse->end() ; 
        iter++)
      {
        I3DOMLaunchSeries newLS;
        I3DOMLaunchSeries newLSSLC;
        for(liter = iter->second.begin(); liter != iter->second.end(); liter++){
          if((*liter).GetLCBit()){
            newLS.push_back(*liter);
          }
          else{
            newLSSLC.push_back(*liter);
          }
        }
        if( !newLS.empty()) (*newInIceResponse)[iter->first] = newLS;
        if( !newLSSLC.empty()) (*newInIceResponseSLC)[iter->first] = newLSSLC;
      }
    
    if(newInIceResponse)
      frame->Put(inIceOutput_,newInIceResponse);
    if(writeSLCOutput_ && newInIceResponseSLC)
      frame->Put(inIceOutputSLC_,newInIceResponseSLC);
    PushFrame(frame,"OutBox");
}

class I3LCPulseCleaning : public I3ConditionalModule {
	public:
		I3LCPulseCleaning(const I3Context &);
		void Configure();
		void Physics(I3FramePtr fr);
	private:
		static bool IsHLC(const OMKey &key, unsigned idx,
		    const I3RecoPulse &pulse)
		{
			return pulse.GetFlags() & I3RecoPulse::LC;
		}
		static bool IsSLC(const OMKey &key, unsigned idx,
		    const I3RecoPulse &pulse)
		{
			return !(pulse.GetFlags() & I3RecoPulse::LC);
		}
		
		std::string input, output_hlc, output_slc;
};

I3_MODULE(I3LCPulseCleaning)

I3LCPulseCleaning::I3LCPulseCleaning(const I3Context &context) :
    I3ConditionalModule(context)
{
	AddParameter("Input", "Name of input pulse series", "OfflinePulses");
	AddParameter("OutputHLC", "Name of HLC pulses (blank to disable)",
	    "OfflinePulsesHLC");
	AddParameter("OutputSLC", "Name of SLC pulses (blank to disable)",
	    "OfflinePulsesSLC");

	AddOutBox("OutBox");
}

void
I3LCPulseCleaning::Configure()
{
	GetParameter("Input", input);
	GetParameter("OutputHLC", output_hlc);
	GetParameter("OutputSLC", output_slc);
}

void
I3LCPulseCleaning::Physics(I3FramePtr frame)
{
	// Run only if input found
	if (!frame->Has(input)) {
		PushFrame(frame);
		return;
	}

	// Make masks by applying a callback to each pulse.
	I3RecoPulseSeriesMapMaskPtr hlc(new
	    I3RecoPulseSeriesMapMask(*frame, input, IsHLC));
	I3RecoPulseSeriesMapMaskPtr slc(new
	    I3RecoPulseSeriesMapMask(*frame, input, IsSLC));
		
	// Add to frame, if requested
	if (output_hlc.size() > 0)
		frame->Put(output_hlc, hlc);
	if (output_slc.size() > 0)
		frame->Put(output_slc, slc);

	PushFrame(frame);
}

