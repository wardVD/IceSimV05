/**
 * Copyright (c) 2013
 * Claudio Kopper <claudio.kopper@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * $Id$
 *
 * @file I3CombineMCPE.cxx
 * @version $Revision$
 * @date $Date$
 * @author Claudio Kopper
 */

#include <string>
#include <vector>

#include "icetray/I3ConditionalModule.h"

#include "simclasses/I3MCPE.h"

/**
 * @brief Combines several I3MCPEHitSeriesMaps into one.
 */
class I3CombineMCPE : public I3ConditionalModule
{
public:
  I3CombineMCPE(const I3Context& ctx);
  ~I3CombineMCPE(){};

  void Configure();
  void DAQ(I3FramePtr frame);
  void Finish(){};

private:
  std::vector<std::string> inputResponses_;
  std::string outputResponse_;

private:
  SET_LOGGER("I3CombineMCPE");
};

namespace{
  bool time_ordered(const I3MCPE& lhs, const I3MCPE& rhs){
    return (lhs.time < rhs.time);
  }
}

I3CombineMCPE::I3CombineMCPE(const I3Context& ctx) :
I3ConditionalModule(ctx),
outputResponse_("CombinedMCPEs")
{
  AddParameter("InputResponses",
               "Vector of names of the input response serieses to combine (no DEFAULT)",
               inputResponses_);

  AddParameter("OutputResponse",
               "Name of the output response series",
               outputResponse_);

  AddOutBox("OutBox");
}

void I3CombineMCPE::Configure()
{
    GetParameter("InputResponses", inputResponses_);
    GetParameter("OutputResponse", outputResponse_);

    if(inputResponses_.empty())
      log_fatal("No input responses set.");
}

void I3CombineMCPE::DAQ(I3FramePtr frame)
{
    I3MCPESeriesMapPtr output(new I3MCPESeriesMap);

    for(std::vector<std::string>::const_iterator initer = inputResponses_.begin();
        initer != inputResponses_.end();
        initer++)
    {
        I3MCPESeriesMapConstPtr input = frame->Get<I3MCPESeriesMapConstPtr>(*initer);

        if(!input)
        {
          log_warn("Frame is missing an input response '%s'. Combining the ones I can find.",
                   initer->c_str());
            continue;
        }

        for (I3MCPESeriesMap::const_iterator map_iter = input->begin();
             map_iter != input->end(); map_iter++)
        {
            const OMKey& omkey = map_iter->first; 
            const I3MCPESeries &pe_series = map_iter->second;

            for (I3MCPESeries::const_iterator series_iter = pe_series.begin();
                 series_iter != pe_series.end(); ++series_iter)
            {
                // this will insert an empty vector automatically in case there is none
                (*output)[omkey].push_back(*series_iter);
            }

        }
    }

    // Make sure the resultant reponses are in time order
    for (I3MCPESeriesMap::iterator map_iter = output->begin();
         map_iter != output->end(); map_iter++)
    {
        I3MCPESeries::iterator beginning = map_iter->second.begin();
        I3MCPESeries::iterator ending = map_iter->second.end();
        std::sort(beginning,ending,time_ordered);
    }

    frame->Put(outputResponse_, output);
    PushFrame(frame,"OutBox");
}

I3_MODULE(I3CombineMCPE);
