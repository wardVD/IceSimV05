/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file HitHisto.cxx
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier, Hans Dembinski
 */


#include <topsimulator/HitHisto.h>
#include <icetray/OMKey.h>
#include <dataclasses/physics/I3Particle.h>
#include <stdexcept>
#include <algorithm>
#include <boost/foreach.hpp>

using namespace std;


std::vector<int> HitHisto::unique_sorted(std::vector<int> v)
{
  sort(v.begin(), v.end());
  return std::vector<int>(v.begin(), unique(v.begin(), v.end()));
}

HitHisto::HitHisto(double binWidth,
                   I3MCPESeries* peSeries,
                   std::vector<int> components):
  binWidth_(binWidth),
  peSeries_(peSeries),
  components_(unique_sorted(components)),
  npesum_(0)
{
  for (unsigned int i = 0; i != components_.size(); ++i) {
    index_[components_[i]] = i;
  }
}

void HitHisto::Fill(double time, unsigned npe, const ExtendedI3Particle& p)
{
  if (npe > 0)
    FillBin(GetBin(time), npe, p);
}

void HitHisto::Fill(vector<double>& times, const ExtendedI3Particle& p)
{
  if (times.empty())
    return;

  // sort to make times in same bin adjacent;
  // note that using set<double> for times is slower
  sort(times.begin(), times.end());

  int cur = GetBin(times[0]);
  int32_t npe = 0;
  BOOST_FOREACH(double t, times) {
    const int bin = GetBin(t);
    if (bin != cur) {
      FillBin(cur, npe, p);
      cur = bin;
      npe = 0;
    }
    ++npe;
  }
  // fill last bin
  FillBin(cur, npe, p);
}

void HitHisto::FillBin(int bin, unsigned npe, const ExtendedI3Particle& p)
{
  assert(index_.find(p.GetAirShowerComponent()) != index_.end());
  if (histo_.find(bin) == histo_.end()) {
    histo_.insert(pair<int, NPE>(bin, NPE(components_.size())));
  }
  histo_.find(bin)->second[index_[p.GetAirShowerComponent()]] += npe;
  npesum_ += npe;

  if (peSeries_) {
    I3MCPE pe(p);
    pe.time = GetBinCenter(bin);
    pe.npe = npe;
    peSeries_->push_back(pe);
  }
}

void HitHisto::Scale(double factor)
{
  npesum_ = 0;
  BOOST_FOREACH(Histo::value_type& kv, histo_) {
    for (unsigned i = 0; i < components_.size(); ++i) {
      // round to nearest integer
      kv.second[i] = uint32_t(factor * kv.second[i] + 0.5);
      npesum_ += kv.second[i];
    }
  }

  // also need to scale peSeries
  if (peSeries_) {
    BOOST_FOREACH(I3MCPESeries::value_type& v, *peSeries_)
      v.npe = uint32_t(factor * v.npe + 0.5);
  }
}


void HitHisto::FillObject(I3MCPESeries& series) const
{
  if (peSeries_)
    throw std::runtime_error("FillObject(I3MCPESeries&) may not be called with peSeries_ != NULL");

  // clear possible previous content
  I3MCPESeries(histo_.size()).swap(series);
  unsigned i = 0;
  BOOST_FOREACH(const Histo::value_type& kv, histo_) {
    I3MCPE& pe = series[i++];
    pe.time = GetBinCenter(kv.first);
    pe.npe = kv.second.sum();
  }
}


void HitHisto::FillWithClassifiedPE(I3MCPESeries& series) const
{
  if (peSeries_)
    throw std::runtime_error("FillWithClassifiedPE may not be called with peSeries_ != NULL");

  // clear possible previous content
  I3MCPESeries().swap(series);
  BOOST_FOREACH(const Histo::value_type& kv, histo_) {
    // loop over PE classes
    for (unsigned i = 0; i < components_.size(); ++i) {
      uint32_t npe = kv.second[i];
      if (npe == 0)
        continue;
      I3MCPE pe;
      pe.ID.majorID = 0; // special major_ID to show that this does not reflect a real particle
      pe.ID.minorID = components_[i]; // encode class in minor_ID, don't use 0
      pe.time = GetBinCenter(kv.first);
      pe.npe = npe;
      series.push_back(pe);
    }
  }
}

void HitHisto::FillObject(I3MCHitSeries& series) const
{
  // deleting any previous content
  I3MCHitSeries(histo_.size()).swap(series);
  unsigned i = 0;
  BOOST_FOREACH(const Histo::value_type& kv, histo_) {
    I3MCHit& hit = series[i++];
    hit.SetTime(GetBinCenter(kv.first));
    hit.SetNPE(kv.second.sum());
  }
}

void HitHisto::FillObject(I3RecoPulseSeries& recoPulseSeries) const
{
  I3RecoPulse pulse;
  // This gives a more realistic number but for testing it is sometimes better to directly look at NumHits()
  //pulse.SetCharge(GetNumHits()*I3PMTConstants::MEAN_NORMALIZED_PE);
  pulse.SetCharge(GetNumHits());
  pulse.SetTime(GetBinCenter(histo_.begin()->first));
  
  recoPulseSeries.push_back(pulse); 
}

//////////////////////////////////////////////////////////////////////////////


HitHistoCollection::HitHistoCollection(double binWidth,
                                       bool compressPEs,
                                       std::vector<int> components):
  binWidth_(binWidth),
  peSeriesMap_(),
  components_(components)
{
  if (!compressPEs)
    peSeriesMap_.reset(new I3MCPESeriesMap());
}

HitHisto& HitHistoCollection::GetHitHisto(const OMKey& omKey)
{
  map<OMKey, HitHisto>::iterator iter = histoMap_.find(omKey);
  if (iter == histoMap_.end()) {
    iter = histoMap_.insert(make_pair(
      omKey,
      HitHisto(binWidth_, (peSeriesMap_ ? &((*peSeriesMap_)[omKey]) : NULL), components_)
    )).first;
  }
  return iter->second;
}

// sort PESeries by time
bool I3MCPE_cmp(const I3MCPE& a, const I3MCPE& b) { return a.time < b.time; }

// template specialization for I3MCPESeries
template<>
I3MCPESeriesMapPtr HitHistoCollection::GenerateMap<I3MCPESeries>()
  const 
{
  I3MCPESeriesMapPtr map(new I3MCPESeriesMap());
  if (peSeriesMap_) {
    BOOST_FOREACH(const I3MCPESeriesMap::value_type& kv, *peSeriesMap_) {
      if (kv.second.size()) {
        (*map)[kv.first] = kv.second;
        sort((*map)[kv.first].begin(), (*map)[kv.first].end(), I3MCPE_cmp);
      }
    }
  } else {
    BOOST_FOREACH(const HistoMap::value_type& kv, histoMap_) {
      if (kv.second.HasHits())
        kv.second.FillObject((*map)[kv.first]);
    }
  }
  return map;
}

I3MCPESeriesMapPtr HitHistoCollection::GeneratePEClassMap()
  const
{
  I3MCPESeriesMapPtr map(new I3MCPESeriesMap());
  BOOST_FOREACH(const HistoMap::value_type& kv, histoMap_) {
    if (kv.second.HasHits())
      kv.second.FillWithClassifiedPE((*map)[kv.first]);
  }
  return map;
}
