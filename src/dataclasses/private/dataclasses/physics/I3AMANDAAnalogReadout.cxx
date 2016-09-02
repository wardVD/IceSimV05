/**
 * copyright  (C) 2005
 * the IceCube Collaboration
 * $Id: I3AMANDAAnalogReadout.cxx 94949 2012-11-04 16:40:30Z nwhitehorn $
 *
 * @file dataclasses/private/dataclasses/physics/I3AMANDAAnalogReadout.cxx
 * @version $Revision: 94949 $
 * @date $Date: 2012-11-04 10:40:30 -0600 (Sun, 04 Nov 2012) $
 * @author deyoung
 */

#include <icetray/serialization.h>
#include "dataclasses/physics/I3AMANDAAnalogReadout.h"
#include <algorithm>
#include <functional>

using namespace std;

I3AMANDAAnalogReadout::~I3AMANDAAnalogReadout() {}

template <class Archive> 
void 
I3AMANDAAnalogReadout::save(Archive& ar, unsigned version) const
{
  if (version>i3amandaanalogreadout_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3AMANDAAnalogReadout class.",version,i3amandaanalogreadout_version_);

  ar & make_nvp("LEs", LEs_);
  ar & make_nvp("TEs", TEs_);
  ar & make_nvp("HitNumbers", HitNumbers_);
  ar & make_nvp("ParentIDs", ParentIDs_);
  ar & make_nvp("ADC", adc_);
  ar & make_nvp("Overflow", overflow_);
  ar & make_nvp("Calib", is_calib_);
}

template <class Archive> 
void 
I3AMANDAAnalogReadout::load(Archive& ar, unsigned version)
{
  if (version>i3amandaanalogreadout_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3AMANDAAnalogReadout class.",version,i3amandaanalogreadout_version_);

  ar & make_nvp("LEs", LEs_);
  ar & make_nvp("TEs", TEs_);
  ar & make_nvp("HitNumbers", HitNumbers_);
  ar & make_nvp("ParentIDs", ParentIDs_);
  ar & make_nvp("ADC", adc_ );
  ar & make_nvp("Overflow", overflow_ );
  ar & make_nvp("Calib", is_calib_ );
    
  processFirstLE_ = true;
  processTOTs_ = true;
}

I3_SPLIT_SERIALIZABLE(I3AMANDAAnalogReadout);
I3_SERIALIZABLE(I3AMANDAAnalogReadoutMap);

double I3AMANDAAnalogReadout::GetFirstLE() const
{
  if(LEs_.empty()) return NAN;

  if(processFirstLE_)
  {
    vector<double> tmp = LEs_;
    sort(tmp.begin(),tmp.end());
    firstLE_ = tmp[0];
    processFirstLE_ = false;
  }
  
  return firstLE_;
}

const vector<double>& I3AMANDAAnalogReadout::GetTOTs() const
{
  if(processTOTs_)
  {
    // this is not true for AMANDA data, but this is fixed in the
    // f2k reader
    if (LEs_.size() != TEs_.size())
      log_fatal("LEs.size() and TEs.size() should have same size");

    TOTs_.resize(LEs_.size());
    transform(TEs_.begin(), TEs_.end(), LEs_.begin(), TOTs_.begin(), minus<double>());
    processTOTs_ = false;
  }
                                                                                                                            
  return TOTs_;
}

