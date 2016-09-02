/**
 *  Copyright (C) 2011
 *  Nathan Whitehorn	<nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */
#ifndef ICETRAY_I3SPLITTERMODULE_H_INCLUDED
#define ICETRAY_I3SPLITTERMODULE_H_INCLUDED

#include <icetray/I3Configuration.h>
#include <icetray/I3Frame.h>
#include <string>

/**
 * @brief This class is meant to be a mix-in base for modules that split
 * DAQ frames into multiple physics frames.
 *
 */

class I3Splitter
{
	public:
		I3Splitter(const I3Configuration& config);
		~I3Splitter();

		I3FramePtr GetNextSubEvent(I3FramePtr daq);

	private:
		I3FramePtr last_daq;
		const I3Configuration& config_;
		int last_subevent_id;
};

#endif
