
#include <icetray/load_project.h>

#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

#include <payload-parsing/I3DAQDataTrimmer.h>

namespace bp = boost::python;

static bool
compare(const I3DAQDataTrimmer::span_list_t::value_type &lhs,
    const I3DAQDataTrimmer::span_list_t::value_type &rhs)
{
	return lhs.get<1>().GetStartTime() < rhs.get<1>().GetStartTime();
}

static I3MapKeyVectorIntPtr
GetEncodedSizes(const I3VectorChar &data)
{
	I3MapKeyVectorIntPtr sizes = boost::make_shared<I3MapKeyVectorInt>();
	I3DAQDataTrimmer::span_list_t spans
	    = I3DAQDataTrimmer::ParseLaunches(data);
	spans.sort(compare);
	BOOST_FOREACH(const I3DAQDataTrimmer::span_list_t::value_type &record,
	    spans) {
		const I3DAQDataTrimmer::span_t &span = record.get<2>();
		(*sizes)[record.get<0>()].push_back(span.second-span.first);
	}

	return sizes;
}

I3_PYTHON_MODULE(payload_parsing)
{

  load_project("payload-parsing", false); 

	bp::def("get_encoded_sizes", &GetEncodedSizes, "Return the on-disk size of "
	    "each DOMLaunch in a raw DAQ payload.");
}

