#ifndef I3DAQDATATRIMMER_H
#define I3DAQDATATRIMMER_H

/**
 * @file I3DAQDataTrimmer.h
 * @brief Remove hit records from a raw DAQ payload.
 *
 * (c) 2011 Jakob van Santen
 *          and the IceCube Collaboration
 *
 * $Id: I3DAQDataTrimmer.h 136616 2015-08-20 14:10:06Z rmaunu $
 * @version $Revision: 136616 $
 * @date $Date: 2015-08-20 09:10:06 -0500 (Thu, 20 Aug 2015) $
 * @author Jakob van Santen <vansanten@wisc.edu>
 */

#include <dataclasses/I3Vector.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <icetray/I3ConditionalModule.h>
#include <boost/tuple/tuple.hpp>

class I3DAQDataTrimmer : public I3ConditionalModule {
public:
	I3DAQDataTrimmer(const I3Context &ctx);
	void Configure();
	void DAQ(I3FramePtr frame);
	
	static I3VectorCharPtr TrimPayload(const std::vector<char> &data,
	    const I3DOMLaunchSeriesMap &launches, bool remove_triggers);

	typedef std::pair<off_t, off_t> span_t;
	typedef std::list<boost::tuple<OMKey, I3DOMLaunch, span_t> > span_list_t;
	static span_list_t ParseLaunches(const std::vector<char> &data);
private:
	std::string buffer_, new_buffer_, launches_;
	bool remove_triggers_;
	
	SET_LOGGER("I3DAQDataTrimmer");
};

#endif
