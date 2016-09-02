/**
 * copyright  (C) 2011
 *
 * The Icecube Collaboration
 *
 * $Id$
 *
 * \file SparseHistogram.cxx
 *
 * \author Javier Gonzalez
 * \date 07 Aug 2013
 * \version $Revision$
 * Last changed by $LastChangedBy$ on $LastChangedDate$
 */

// static const char CVSId[] = "$Id$";


#include <topsimulator/injectors/SparseHistogram.h>
#include <icetray/I3Logging.h>

using namespace std;
using namespace topsim;

SparseHistogram::SparseHistogram(double side, int bins):
  data_(bins, bins),
  side_(side),
  bins_(bins<0?0:bins),
  binSize_(side_/bins_)
{
  Reset();
}


SparseHistogram::~SparseHistogram()
{
}


void
SparseHistogram::Resize(double side, int bins)
{
  data_.resize(bins, bins, false);
  bins_ = (bins<0?0:bins);
  side_ = side;
  binSize_ = side_/bins_;
  Reset();
}


void
SparseHistogram::Reset()
{
  data_.clear();
}


void
SparseHistogram::Fill(double x, double y, double weight)
{
  const int i = FindBin(x);
  const int j = FindBin(y);
  if (i < bins_ && i > 0 && j < bins_ && j > 0)
    data_(i, j) += weight;
}


void
SparseHistogram::Set(int i, int j, double v)
{
  if (i < bins_ && i >= 0 && j < bins_ && j >= 0)
    data_(i,j) = v;
  else
    log_warn("Ignoring invalid bin (%d, %d). Allowed values in (0, %d)", i, j, bins_);
}


int
SparseHistogram::FindBin(double x)
  const
{
  if (x < -side_/2 || x > side_/2)
    return -1;
  // do I need the 0.5? I always wonder...
  return static_cast<int>(floor((x+side_/2)/binSize_) + 0.5);
}


double
SparseHistogram::GetEntries()
  const
{
  double sum = 0;
  for (int i = 0; i != bins_; ++i) {
    for (int j = 0; j != bins_; ++j) {
      sum += data_(i,j);
    }
  }
  return sum;
}


std::vector<SparseHistogramIndex> SparseHistogram::GetNonEmptyBins()
  const
{
  typedef boost::numeric::ublas::compressed_matrix<double>::const_iterator1 it1_t;
  typedef boost::numeric::ublas::compressed_matrix<double>::const_iterator2 it2_t;

  std::vector<SparseHistogramIndex> bins;
  for (it1_t it1 = data_.begin1(); it1 != data_.end1(); ++it1) {
    for (it2_t it2 = it1.begin(); it2 != it1.end(); ++it2) {
      bins.push_back(SparseHistogramIndex(it2.index1(), it2.index2()));
    }
  }
  return bins;
}


namespace topsim {

  bool
  operator ==(const SparseHistogram& lhs, const SparseHistogram& rhs)
  {
    const bool ok = lhs.GetBins() == rhs.GetBins() &&
      lhs.GetBinWidth() == rhs.GetBinWidth() &&
      lhs.GetEntries() == rhs.GetEntries();
    if (!ok)
      return false;

    const std::vector<SparseHistogramIndex> bins = lhs.GetNonEmptyBins();
    for (unsigned int i = 0; i != bins.size(); ++i) {
      if (lhs.At(bins[i].x, bins[i].y) != rhs.At(bins[i].x, bins[i].y))
        return false;
    }
    return true;
  }

  void
  Convolve(const SparseHistogram& in, const SparseHistogram& kernel, SparseHistogram& out)
  {
    log_warn("check bin sizes here?");
    const int out_bins = in.GetBins()+kernel.GetBins()-1;
    out.Resize(out_bins*in.GetBinWidth(), out_bins);
    // const int nbins = out.GetBins();
    const std::vector<SparseHistogramIndex> kernel_bins = kernel.GetNonEmptyBins();
    const std::vector<SparseHistogramIndex> in_bins = in.GetNonEmptyBins();

    for (unsigned int i = 0; i != in_bins.size(); ++i) {
      for (unsigned int j = 0; j != kernel_bins.size(); ++j) {
        const int x = in_bins[i].x+kernel_bins[j].x;
        const int y = in_bins[i].y+kernel_bins[j].y;
        const double v = kernel.At(kernel_bins[j].x, kernel_bins[j].y)*in.At(in_bins[i].x, in_bins[i].y);
        out.Set(x, y, out.At(x,y)+v);
      }
    }
  }

}


// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
