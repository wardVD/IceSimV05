/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file HitHisto.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier, Hans Dembinski
 */

#ifndef _TOPSIMULATOR_HITHISTO_H_
#define _TOPSIMULATOR_HITHISTO_H_

#include <map>
#include <string>
#include <valarray>
#include <simclasses/I3MCPE.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3Particle.h>
#include <topsimulator/ExtendedI3Particle.h>

/**
 * \brief HitHisto groups photo-electrons that
 * arrive in a short time-window.
 *
 * It provides a simple histogram class that groups PEs,
 * while keeping track of the air shower component that
 * generated them (see ExtendedI3Particle).
 *
 * For more detailed comments, look into the source code.
 */

class HitHisto
{
  friend class HitHistoCollection;

  struct NPE: public std::valarray<uint32_t> {
    NPE(int n): std::valarray<uint32_t>(n) {}
  };
  typedef std::map<int, NPE> Histo;

  const double binWidth_;
  I3MCPESeries* peSeries_;
  std::vector<int> components_;
  std::map<int,int> index_; // the "inverse" of components_
  Histo histo_;
  unsigned npesum_; // count all PEs

  void FillBin(int bin, unsigned npe, const ExtendedI3Particle& p);

  // these pass internal data to HitHistoCollection
  void FillObject(I3MCPESeries&) const;
  void FillObject(I3MCHitSeries&) const;
  void FillObject(I3RecoPulseSeries&) const;
  void FillWithClassifiedPE(I3MCPESeries&) const;

  static std::vector<int> unique_sorted(std::vector<int> v);

public:

  HitHisto(double binWidth, I3MCPESeries* series, std::vector<int> components);
  
  double GetBinWidth() const { return binWidth_; }

  /// don't use this anymore, use one of the overloads below
  void Fill(double time, unsigned npe)
  __attribute__ ((deprecated))
  {
    Fill(time, npe, ExtendedI3Particle());
  }
  
  /// use this for the cherenkov histogram only
  void Fill(double time, unsigned npe, const ExtendedI3Particle& p);

  /// use this for the pe histogram, it allows HitHisto to merge I3MCPEs
  /// generated from the same particle at the same time effectively;
  /// note: we are passing a vector non-const, because it is sorted internally
  void Fill(std::vector<double>& times, const ExtendedI3Particle& p);

  bool HasHits() const { return GetNumHits() > 0; }

  unsigned GetNumHits() const { return npesum_; }
  
  void Scale(double factor);

  /// these are internal details and should be private...
  int GetBin(double time) const { return static_cast<int>(time / binWidth_); }

  double GetBinCenter(int bin) const { return (bin + 0.5) * binWidth_; }

  double GetBinLowEdge(int bin) const { return bin * binWidth_; }
};


/**
 * \brief Collection of the HitHisto instances per tank (OMKey)
 *
 * Has member functions to generate various frame objects from
 * the stored data.
 */

class HitHistoCollection
{
  typedef std::map<OMKey, HitHisto> HistoMap;

  const double binWidth_;
  I3MCPESeriesMapPtr peSeriesMap_;
  HistoMap histoMap_;
  std::vector<int> components_;

public:
  HitHistoCollection(double binWidth, bool compressPEs, std::vector<int> components);

  HitHisto& GetHitHisto(const OMKey& omKey);
  bool Empty() const { return histoMap_.empty(); }
  int GetEntries() const { return histoMap_.size(); }

  // for I3RecoPulseSeries, I3MCHitSeries, I3MCPESeries
  template<typename T>
  boost::shared_ptr<I3Map<OMKey, T> > GenerateMap() const
  {
    typedef I3Map<OMKey, T> OMap;
    boost::shared_ptr<OMap> map(new OMap());
    for (HistoMap::const_iterator
           iter = histoMap_.begin(), end = histoMap_.end();
         iter != end; ++iter)
    {
      if (iter->second.HasHits())
        iter->second.FillObject((*map)[iter->first]);
    }
    return map;
  }

  I3MCPESeriesMapPtr GeneratePEClassMap() const;
};


// template specialization for I3MCPESeries
template<>
I3MCPESeriesMapPtr HitHistoCollection::GenerateMap<I3MCPESeries>() const;


#endif
