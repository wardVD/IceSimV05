/**
 * Copyright (c) 2014
 * Ben Jones <bjpjones@mit.edu>
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
 */

#include <string>
#include <vector>

#include "icetray/I3ConditionalModule.h"
#include <phys-services/I3RandomService.h>
#include "simclasses/I3MCPE.h"

/**
 *@brief Randomly downsample MCPEs from one collection to another
 */
class I3DownsampleMCPE : public I3ConditionalModule{
public:

  I3DownsampleMCPE(const I3Context& ctx);
  ~I3DownsampleMCPE(){};
  
  void Configure();
  void DAQ(I3FramePtr frame);
  void Finish(){};

private:
  std::string inputName_;
  std::string outputName_;
  double      sampleFrac_;
  I3RandomServicePtr rng_;

  SET_LOGGER("I3DownsampleMCPE");
};


I3DownsampleMCPE::I3DownsampleMCPE(const I3Context& ctx) :
  I3ConditionalModule(ctx),
  inputName_("I3MCPESeriesMap"),
  outputName_("DownsampledMCPEs"),
  sampleFrac_(0.95)
{
  AddParameter("InputName",
               "Name of the MCPE set to read in and downsample",
               inputName_);

  AddParameter("OutputName",
               "Name of the output MCPE series",
               outputName_);

  AddParameter("SampleFrac",
               "Fraction of MCPEs to accept",
               0.95);

  AddParameter("RandomService",
               "A random number generating service (derived from I3RandomService)."
               " The default is to get it from the context.",
               rng_);

  AddOutBox("OutBox");
}

void I3DownsampleMCPE::Configure()
{
  GetParameter("InputName", inputName_);
  GetParameter("OutputName", outputName_);
  GetParameter("SampleFrac", sampleFrac_);
  GetParameter("RandomService", rng_);
  if (!rng_) rng_ = context_.Get<I3RandomServicePtr>();
  if (!rng_)
    log_fatal("No random service available. Please add one.");

  if((sampleFrac_<0)||(sampleFrac_>1))
    log_fatal("Sample fraction is not a number between 0 and 1.");
}

void I3DownsampleMCPE::DAQ(I3FramePtr frame)
{

    I3MCPESeriesMapPtr output(new I3MCPESeriesMap);
    I3MCPESeriesMapConstPtr input = frame->Get<I3MCPESeriesMapConstPtr>(inputName_);

    if(!input){
      log_warn("Frame is missing input MCPE series. cannot downsample");
    }

    for (I3MCPESeriesMap::const_iterator map_iter = input->begin();
         map_iter != input->end(); map_iter++){

        const OMKey& omkey = map_iter->first;
        const I3MCPESeries &pe_series = map_iter->second;

        for (I3MCPESeries::const_iterator series_iter = pe_series.begin();
             series_iter != pe_series.end(); ++series_iter){
            // if the MCPE has only 1 pe (most cases), randomly decide
            //  whether to keep it or throw it
            if(series_iter->npe <= 1){
              if( rng_->Uniform(1) < sampleFrac_ )
                (*output)[omkey].push_back(*series_iter);
            }else{
              // If the MCPE has multiple pe (rare), draw the number to keep
              //  from a binomial distribution.
              int binomialSample = rng_->Binomial(series_iter->npe, sampleFrac_);
              I3MCPE newMCPE = *series_iter;
              newMCPE.npe = binomialSample;
              (*output)[omkey].push_back(newMCPE);
            }
        }
    }

    frame->Put(outputName_, output);
    PushFrame(frame,"OutBox");
}

I3_MODULE(I3DownsampleMCPE);
