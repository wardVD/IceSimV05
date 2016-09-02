/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */
#include <icetray/I3Frame.h>
#include <icetray/I3Context.h>
#include <icetray/I3Configuration.h>
#include <dataclasses/physics/I3EventHeader.h>

#include "phys-services/I3Splitter.h"

I3Splitter::I3Splitter(const I3Configuration& config) :
  last_daq(I3FramePtr((I3Frame *)(NULL))), config_(config)
{
}

I3Splitter::~I3Splitter() { }

I3FramePtr
I3Splitter::GetNextSubEvent(I3FramePtr daq) {
	if (last_daq != daq) {
		last_daq = daq;
		last_subevent_id = -1;
	}

	last_subevent_id++;

	I3FramePtr frame(new I3Frame(*daq));
	frame->purge(I3Frame::Physics); // Kill mixed-in P keys from earlier
	frame->SetStop(I3Frame::Physics);

	// Rewrite event header
	I3EventHeaderPtr header(new I3EventHeader(frame->Get<I3EventHeader>()));
	header->SetSubEventStream(config_.InstanceName());
	header->SetSubEventID(last_subevent_id);
	frame->Delete("I3EventHeader");
	frame->Put("I3EventHeader", header);

	return frame;
}

