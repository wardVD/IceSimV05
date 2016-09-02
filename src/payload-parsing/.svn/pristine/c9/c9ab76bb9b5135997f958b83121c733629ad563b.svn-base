/**
 * @file I3DAQDataTrimmer.cxx
 * @brief Remove hit records from a raw DAQ payload.
 *
 * (c) 2011 Jakob van Santen
 *          and the IceCube Collaboration
 *
 * $Id$
 * @version $Revision$
 * @date $Date$
 * @author Jakob van Santen <vansanten@wisc.edu>
 */

#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#include <payload-parsing/I3DAQDataTrimmer.h>
#include <payload-parsing/utility.h>
#include <payload-parsing/decode.h>


I3_MODULE(I3DAQDataTrimmer);

I3DAQDataTrimmer::I3DAQDataTrimmer(const I3Context &ctx)
  : I3ConditionalModule(ctx), buffer_("I3DAQData"),
  new_buffer_("I3TrimmedDAQData"), launches_("DSTLaunches"),
  remove_triggers_(false)
{
  AddParameter("Buffer", "Where to find the DAQ data", buffer_);
  AddParameter("Output", "Where to store stripped-down DAQ data", new_buffer_);
  AddParameter("TrimLaunches", "Where to find a map of DOMLaunches to be removed", launches_);
  AddParameter("RemoveTriggers", "Strip trigger records out of the payload", remove_triggers_);

  AddOutBox("OutBox");
};

void I3DAQDataTrimmer::Configure()
{
  GetParameter("Buffer", buffer_);
  GetParameter("Output", new_buffer_);
  GetParameter("TrimLaunches", launches_);
  GetParameter("RemoveTriggers", remove_triggers_);

}

void I3DAQDataTrimmer::DAQ(I3FramePtr frame)
{
  I3VectorCharConstPtr buffer =
    frame->Get<I3VectorCharConstPtr>(buffer_);
  I3DOMLaunchSeriesMapConstPtr launches =
    frame->Get<I3DOMLaunchSeriesMapConstPtr>(launches_);

  if (!buffer || !launches) {
    log_error("Couldn't find inputs!");
    PushFrame(frame);
    return;
  }

  I3VectorCharPtr newbuffer = TrimPayload(*buffer, *launches,
      remove_triggers_);

  frame->Put(new_buffer_, newbuffer);
  PushFrame(frame);
}

I3DAQDataTrimmer::span_list_t I3DAQDataTrimmer::ParseLaunches(
    const std::vector<char> &data)
{
  span_list_t span_list;

  if(data.size() < 16)
    log_fatal("expected at least 16 bytes for event, "
        "got %zd instead", data.size());

  using namespace payload_parsing;
  uint32_t payloadType = decode<uint32_t>(BIG, 4, data);

  if (payloadType != 21)
    log_fatal("I can only handle type-21 payloads (got %u)", payloadType);

  // Figure out which hits we want to keep
  unsigned pos = 34;
  uint32_t nrOfHitRecords = decode<uint32_t>(BIG, pos, data);
  pos += 4;

  NewDOMLaunches potemkin;
  DecodeConfiguration config(0, 0, false, false);

  for (unsigned i = 0; i < nrOfHitRecords; i++) {
    uint16_t payloadLength = decode<uint16_t>(BIG, pos, data);
    uint8_t hitRecordType = decode<uint8_t>(BIG, pos + 2, data);

    if (hitRecordType == 1)
      decode_payload<25>(potemkin, config, data, pos);
    else if (hitRecordType == 0)
      decode_payload<26>(potemkin, config, data, pos);
    else
      log_fatal("Can't handle hit record type %u", hitRecordType);

    // Consume the launch we just decoded.
    const OMKey &key = potemkin.begin()->first;
    const I3DOMLaunch &launch = potemkin.begin()->second;
    span_list.push_back(boost::make_tuple(key, launch,
          std::make_pair(pos, pos+payloadLength)));

    potemkin.clear();

    pos += payloadLength;
  }

  return span_list;
}

namespace {

  struct FindLaunches {
    const I3DOMLaunchSeriesMap &target_;

    FindLaunches(const I3DOMLaunchSeriesMap &launches) :
      target_(launches) {};

    struct CompareLaunches {
      const I3DOMLaunch &target_;
      CompareLaunches(const I3DOMLaunch &target) : target_(target) {};

      bool operator()(const I3DOMLaunch &rhs) { 
        return target_.GetStartTime() == rhs.GetStartTime(); 
      }
    };

    bool operator()(const I3DAQDataTrimmer::span_list_t::value_type &item)
    {
      const OMKey &key = item.get<0>();
      const I3DOMLaunch &launch = item.get<1>();
      I3DOMLaunchSeriesMap::const_iterator it;
      return (((it = target_.find(key)) != target_.end()) &&
          (std::find_if(it->second.begin(), it->second.end(),
                        CompareLaunches(launch)) != it->second.end()));
    }
  };

  template <typename InputIterator, typename IntType>
    static void
    WriteSize(InputIterator target, IntType value)
    {
      using namespace payload_parsing;

#if !defined(BOOST_BIG_ENDIAN)
      value = endian_swap(value);
#endif
      for (unsigned i=0; i < sizeof(value); i++, target++)
        (*target) = reinterpret_cast<char*>(
            boost::addressof(value))[i];
    }
}

I3VectorCharPtr I3DAQDataTrimmer::TrimPayload(const std::vector<char> &data,
    const I3DOMLaunchSeriesMap &launches, 
    bool remove_triggers)
{
  using namespace payload_parsing;

  I3VectorCharPtr reduced_data = boost::make_shared<I3VectorChar>();
  reduced_data->reserve(data.size());

  // Parse the hit records (throws unless payload is type 21)
  span_list_t spans = ParseLaunches(data);
  // Get the end of the hit-record block in the payload
  unsigned pos = spans.back().get<2>().second;
  // Remove the launches we don't want
  spans.remove_if(FindLaunches(launches));

  // Copy the entire header
  std::copy(data.begin(), data.begin()+34, std::back_inserter(*reduced_data));

  // Write the new number of hit records
  WriteSize(std::back_inserter(*reduced_data), uint32_t(spans.size()));

  // Copy over the non-rejected hits
  BOOST_FOREACH(const span_list_t::value_type &span, spans)
    std::copy(data.begin()+span.get<2>().first,
        data.begin() + span.get<2>().second,
        std::back_inserter(*reduced_data));

  // Rewrite the trigger block
  if (remove_triggers)
    WriteSize(std::back_inserter(*reduced_data), uint32_t(0));
  else
    std::copy(data.begin() + pos, data.end(), std::back_inserter(*reduced_data));

  // Rewrite the total size
  WriteSize(reduced_data->begin(), uint32_t(reduced_data->size()));
  return reduced_data;
}
