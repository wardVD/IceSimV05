/**
 * copyright  (C) 2011
 *
 * The Icecube Collaboration
 *
 * $Id$
 *
 * \file SparseHistogram.h
 *
 * \author Javier Gonzalez
 * \date 07 Aug 2013
 * \version $Revision$
 * Last changed by $LastChangedBy$ on $LastChangedDate$
 */

#ifndef _topsim_SparseHistogram_h_
#define _topsim_SparseHistogram_h_

static const char CVSId__SparseHistogram[] =
"$Id$";

#include<vector>
#include <boost/numeric/ublas/matrix_sparse.hpp>

namespace topsim {
  /**
     Simple structure to hold a pair of indices witha a convenience constructor.
   */
  struct SparseHistogramIndex {
    SparseHistogramIndex(int i, int j):x(i), y(j){}

    int x;
    int y;

  };

  inline bool operator ==(const SparseHistogramIndex& lhs, const SparseHistogramIndex& rhs)
  { return lhs.x==rhs.x && lhs.y==rhs.y; }

  /**
     \class SparseHistogram

     \brief Sparse 2d histograms with very specific characteristics.

     The histograms ranges and bin sizes are equal in both axes and
     both ranges are symmetric. Therefore, you can specify the length
     of one side and the number of bins.

     No default constructor provided and copy is disabled. It does not
     have uver/underflow bin.
   */

  class SparseHistogram {
  public:
    SparseHistogram(double side, int bins);
    ~SparseHistogram();

    /// Reset all entries to zero
    void Reset();

    void Resize(double side, int bins);

    /// As the name says... add one entry to the histogram
    void Fill(double x, double y, double weight=1.);

    /// Directly set the value of a given bin
    void Set(int i, int j, double v);

    double GetBinWidth() const
    { return binSize_; }

    int GetBins() const
    { return bins_; }

    /// Calculate the sum of the weighted entries (it is not cached)
    double GetEntries() const;

    /// Build a list of non-empty bins (it is not cached and might be expensive for large histograms)
    std::vector<SparseHistogramIndex> GetNonEmptyBins() const;

    /// Give the bin number. Same for both axes.
    int FindBin(double x) const;

    double At(int i, int j) const
    { return data_(i,j); }

    double GetBinCenter(int i)
    { return (i+0.5)*binSize_ - side_/2; }

  private:
    // disabling copying for now
    SparseHistogram(const SparseHistogram& other);
    SparseHistogram& operator=(const SparseHistogram& other);

    boost::numeric::ublas::compressed_matrix<double> data_;

    double side_;
    int bins_;
    double binSize_;

  };


  bool operator ==(const SparseHistogram& lhs, const SparseHistogram& rhs);

  void Convolve(const SparseHistogram& in, const SparseHistogram& kernel, SparseHistogram& out);
}


#endif // __SparseHistogram_h_

// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
