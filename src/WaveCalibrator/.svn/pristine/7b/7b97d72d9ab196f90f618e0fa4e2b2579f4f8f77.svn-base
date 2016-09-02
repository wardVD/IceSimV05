/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Jakob van Santen <vansanten@wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include "icetray/I3ConditionalModule.h"
#include "dataclasses/physics/I3Waveform.h"

#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

class I3WaveformSplitter : public I3ConditionalModule {
public:
	I3WaveformSplitter(const I3Context&);
	void Configure();
	void DAQ(I3FramePtr frame);
private:
	std::string input_name_, atwd_hlc_name_, fadc_hlc_name_, slc_name_;
	bool split_atwd_channels_, split_atwd_chips_, highest_unsaturated_, force_;
	
	void PackATWDChannels(I3FramePtr, I3WaveformSeriesMapPtr);
	void PackATWDChips(I3FramePtr, I3WaveformSeriesMapPtr);
	void PackUnsaturatedATWD(I3FramePtr, I3WaveformSeriesMapPtr);
	unsigned GetChannel(const I3Waveform&);
};

I3_MODULE(I3WaveformSplitter);

I3WaveformSplitter::I3WaveformSplitter(const I3Context &ctx) : I3ConditionalModule(ctx)
{
	input_name_ = "CalibratedWaveforms";
	AddParameter("Input", "Name of input I3WaveformSeriesMap", input_name_);
	
	atwd_hlc_name_ = "CalibratedATWD";
	AddParameter("HLC_ATWD", "Where to store the HLC ATWD waveforms", atwd_hlc_name_);
	
	fadc_hlc_name_ = "CalibratedFADC";
	AddParameter("HLC_FADC", "Where to store the HLC FADC waveforms", fadc_hlc_name_);
	
	slc_name_ = "CalibratedSLC";
	AddParameter("SLC", "Where to store the SLC waveforms", slc_name_);
	
	force_ = false;
	AddParameter("Force", "Put all output maps in the frame, even if they're empty.", force_);
	
	split_atwd_channels_ = false;
	AddParameter("SplitATWDChannels",
	    "Split ATWD channels into separate maps numbered 0, 1, and 2", split_atwd_channels_);
	
	highest_unsaturated_ = false;
	AddParameter("PickUnsaturatedATWD",
	    "Emit only the highest-gain unsaturated ATWD channel, mimicking the "
	    "behavior of WaveCalibrator's CALIBRATE_UNSATURATED mode", highest_unsaturated_);
	
	split_atwd_chips_ = false;
	AddParameter("SplitATWDChips",
	    "Split ATWD chips into separate maps, marked _Chip0 and Chip1", split_atwd_chips_);
	
	AddOutBox("OutBox");
}

void
I3WaveformSplitter::Configure()
{
	GetParameter("Input", input_name_);
	GetParameter("HLC_ATWD", atwd_hlc_name_);
	GetParameter("HLC_FADC", fadc_hlc_name_);
	GetParameter("SLC", slc_name_);
	GetParameter("Force", force_);
	GetParameter("PickUnsaturatedATWD", highest_unsaturated_);
	GetParameter("SplitATWDChannels", split_atwd_channels_);
	GetParameter("SplitATWDChips", split_atwd_chips_);
	
	if (split_atwd_channels_ && split_atwd_chips_)
		log_fatal("SplitATWDChannels and SplitATWDChips are mutually exclusive!");
	if (split_atwd_chips_ && highest_unsaturated_)
		log_fatal("SplitATWDChips and PickUnsaturatedATWD are mutually exclusive!");
	if (split_atwd_channels_ && highest_unsaturated_)
		log_fatal("SplitATWDChannels and PickUnsaturatedATWD are mutually exclusive!");
}

void
I3WaveformSplitter::DAQ(I3FramePtr frame)
{
	I3WaveformSeriesMapConstPtr input_wfs = frame->Get<I3WaveformSeriesMapConstPtr>(input_name_);
	
	if (!input_wfs) {
		PushFrame(frame);
		return;
	}
	
	I3WaveformSeriesMapPtr atwd_hlc_out = boost::make_shared<I3WaveformSeriesMap>();
	I3WaveformSeriesMapPtr fadc_hlc_out = boost::make_shared<I3WaveformSeriesMap>();
	I3WaveformSeriesMapPtr slc_out = boost::make_shared<I3WaveformSeriesMap>();
	
	I3WaveformSeriesMap::const_iterator input_it = input_wfs->begin();
	for ( ; input_it != input_wfs->end(); input_it++) {
		I3WaveformSeries atwd_hlc_vec, fadc_hlc_vec, slc_vec;
		
		BOOST_FOREACH(const I3Waveform &wf, input_it->second)
			switch (wf.GetSource()) {
				case I3Waveform::ATWD:
					atwd_hlc_vec.push_back(wf);
					break;
				case I3Waveform::FADC:
					fadc_hlc_vec.push_back(wf);
					break;
				case I3Waveform::SLC:
					slc_vec.push_back(wf);
					break;
				default:
					break;
			}
		
		if (atwd_hlc_vec.size() > 0)
			atwd_hlc_out->operator[](input_it->first).swap(atwd_hlc_vec);
		if (fadc_hlc_vec.size() > 0)
			fadc_hlc_out->operator[](input_it->first).swap(fadc_hlc_vec);
		if (slc_vec.size() > 0)
			slc_out->operator[](input_it->first).swap(slc_vec);
	}
	
	if (split_atwd_channels_)
		PackATWDChannels(frame, atwd_hlc_out);
	else if (split_atwd_chips_)
		PackATWDChips(frame, atwd_hlc_out);
	else if (highest_unsaturated_)
		PackUnsaturatedATWD(frame, atwd_hlc_out);
	else if (force_ || atwd_hlc_out->size() > 0)
		frame->Put(atwd_hlc_name_, atwd_hlc_out);
	
	if (force_ || fadc_hlc_out->size() > 0)
		frame->Put(fadc_hlc_name_, fadc_hlc_out);
	if (force_ || slc_out->size() > 0)
		frame->Put(slc_name_, slc_out);
	
	PushFrame(frame);
}

unsigned
I3WaveformSplitter::GetChannel(const I3Waveform &wf)
{
	unsigned channel = 0;
	const std::vector<I3Waveform::StatusCompound> &winfo = wf.GetWaveformInformation();
	std::vector<I3Waveform::StatusCompound>::const_iterator it = winfo.begin();
	if (it != winfo.end())
		channel = it->GetChannel();
	else
		return channel;
		
	it++;
	for ( ; it != winfo.end(); it++)
		if (channel != unsigned(it->GetChannel()))
			log_error("Mixed channels %u and %d found while splitting ATWD channels!", channel, it->GetChannel());
			
	return channel;	
}

void
I3WaveformSplitter::PackATWDChannels(I3FramePtr frame, I3WaveformSeriesMapPtr atwds)
{
	std::vector<I3WaveformSeriesMapPtr> atwd_channels(3);
	BOOST_FOREACH(I3WaveformSeriesMapPtr &ptr, atwd_channels)
		ptr = boost::make_shared<I3WaveformSeriesMap>();
	
	I3WaveformSeriesMap::const_iterator input_it = atwds->begin();
	for ( ; input_it != atwds->end(); input_it++) {
		std::vector<std::vector<I3Waveform> > series(3);
		
		BOOST_FOREACH(const I3Waveform &wf, input_it->second) {
			const unsigned channel = GetChannel(wf);
			series[channel].push_back(wf);
		}
		
		unsigned i = 0;
		BOOST_FOREACH(std::vector<I3Waveform> &wfs, series) {
			if (wfs.size() > 0)
				atwd_channels[i]->operator[](input_it->first).swap(wfs);
			i++;
		}
	}
	
	unsigned i = 0;
	BOOST_FOREACH(I3WaveformSeriesMapPtr &atwd_channel, atwd_channels) {
		if (force_ || atwd_channel->size() > 0) {
			std::stringstream key;
			key << atwd_hlc_name_ << "_" << i;
			frame->Put(key.str(), atwd_channel);
		}
		i++;
	}
}

void
I3WaveformSplitter::PackATWDChips(I3FramePtr frame, I3WaveformSeriesMapPtr atwds)
{
	std::vector<I3WaveformSeriesMapPtr> atwd_chips(2);
	BOOST_FOREACH(I3WaveformSeriesMapPtr &ptr, atwd_chips)
		ptr = boost::make_shared<I3WaveformSeriesMap>();
	
	I3WaveformSeriesMap::const_iterator input_it = atwds->begin();
	for ( ; input_it != atwds->end(); input_it++) {
		std::vector<std::vector<I3Waveform> > series(2);
		
		BOOST_FOREACH(const I3Waveform &wf, input_it->second) {
			const unsigned chip = wf.GetSourceIndex();
			series[chip].push_back(wf);
		}
		
		unsigned i = 0;
		BOOST_FOREACH(std::vector<I3Waveform> &wfs, series) {
			if (wfs.size() > 0)
				atwd_chips[i]->operator[](input_it->first).swap(wfs);
			i++;
		}
	}
	
	unsigned i = 0;
	BOOST_FOREACH(I3WaveformSeriesMapPtr &atwd_chip, atwd_chips) {
		if (force_ || atwd_chip->size() > 0) {
			std::stringstream key;
			key << atwd_hlc_name_ << "_Chip" << i;
			frame->Put(key.str(), atwd_chip);
		}
		i++;
	}
}

void
I3WaveformSplitter::PackUnsaturatedATWD(I3FramePtr frame, I3WaveformSeriesMapPtr atwds)
{	
	I3WaveformSeriesMapPtr filtered = boost::make_shared<I3WaveformSeriesMap>();
	
	I3WaveformSeriesMap::iterator input_it = atwds->begin();
	for ( ; input_it != atwds->end(); input_it++) {
		I3WaveformSeries &series = filtered->operator[](input_it->first);
		
		/* Now things get tricky */
		I3WaveformSeries::const_iterator head = input_it->second.begin();
		I3WaveformSeries::const_iterator tail = input_it->second.begin();
		
		while (tail != input_it->second.end()) {
			double t0 = head->GetStartTime();
			/*
			 * Iterate until we hit an ATWD waveform with a different start time.
			 * If the wf pointed to by head is saturated, head is set to tail.
			 * This leaves head pointing to the highest-gain unsaturated wf
			 * at all times, even if channel 2 is saturated.
			 */
			while (tail != input_it->second.end() &&
			    !(fabs(tail->GetStartTime()-t0) > 10)) {
				
				if (head->GetStatus() & I3Waveform::SATURATED)
					head = tail;
					
				tail++;
			}
			
			/* Take the chosen wf, and reset the range pointers. */
			series.push_back(*head);
			head = tail;
		}
	}
	
	frame->Put(atwd_hlc_name_, filtered);
}
