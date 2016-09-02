#include <I3Test.h>

#include <topsimulator/injectors/SparseHistogram.h>
#include <iostream>


TEST_GROUP(SPARSE_HISTOGRAM);

using namespace topsim;
using namespace std;

TEST(sparse_histogram)
{
  cout << endl;

  SparseHistogram histogram(10., 20);

  ENSURE(histogram.GetNonEmptyBins().size()==0);
  ENSURE(histogram.GetBins()==20, "Wrong number bins");
  ENSURE(histogram.GetBinWidth()==0.5, "Wrong bin width");

  histogram.Fill(0.5, -1.5, 2.5);
  ENSURE(histogram.GetEntries() == 2.5, "floating point weight failed");

  histogram.Reset();

  histogram.Fill(0.25, 1.25);
  histogram.Fill(0.5, -1.5, 2);

  std::vector<SparseHistogramIndex> bins = histogram.GetNonEmptyBins();

  for (unsigned int i = 0; i != bins.size(); ++i) {
    cout << "    bin " << i << ": " << bins[i].x << ", " << bins[i].y << endl;
  }

  ENSURE(bins.size()==2, "Wrong size of non-empty bin container");
  ENSURE(bins[0].x==10, "Wrong non-empty x index (0)");
  ENSURE(bins[0].y==12, "Wrong non-empty y index (0)");
  ENSURE(bins[1].x==11, "Wrong non-empty x index (1)");
  ENSURE(bins[1].y==7, "Wrong non-empty y index (1)");

  ENSURE(histogram.At(11,7)==2, "At failed in a non-empty bin");
  ENSURE(histogram.At(4,5)==0, "At failed in an empty bin.");
  ENSURE(histogram.GetNonEmptyBins().size()==2, "Number of non-empty bins increased after calling At.");

  histogram.Set(5, 5, 2);
  ENSURE(histogram.GetNonEmptyBins().size()==3, "Non empty bin container did not grow after setting manually.");

  histogram.Set(5, 7, 1);
  histogram.Set(0, 0, 1); // weird things can happen at the edges of the histogram...
  histogram.Set(19, 19, 1);

  cout << endl << "HISTOGRAM:" << endl;
  cout << "i\\j\t";
  for (int j = 0; j != histogram.GetBins(); ++j) {
    cout << j << "\t";
  }
  cout << endl;
  for (int i = 0; i != histogram.GetBins(); ++i) {
    cout << i << "\t";
    for (int j = 0; j != histogram.GetBins(); ++j) {
      cout << histogram.At(i,j) << "\t";
    }
    cout << endl;
  }

  ENSURE(histogram.FindBin(-3.)==4, "Wrong bin index.");
  ENSURE(histogram.FindBin(-6.)==-1, "Wrong bin index (our of range)");

  ENSURE(histogram.GetEntries() == 8.);
  ENSURE(histogram.GetBinCenter(0) == -4.75, "Wrong lowest bin");
  ENSURE(histogram.GetBinCenter(19) == 4.75, "Wrong highest bin");
  ENSURE(histogram==histogram, "Error in operator==");


  SparseHistogram kernel(3.5, 7);
  //kernel.Set(3,3,1);
  kernel.Set(3,4,1);
  kernel.Set(4,3,1);
  kernel.Set(2,3,1);
  kernel.Set(4,2,1);
  kernel.Set(4,5,1);
  kernel.Set(1,1,1);
  kernel.Set(3,6,1);

  cout << endl << "KERNEL:" << endl;
  cout << "i\\j\t";
  for (int j = 0; j != kernel.GetBins(); ++j) {
    cout << j << "\t";
  }
  cout << endl;
  for (int i = 0; i != kernel.GetBins(); ++i) {
    cout << i << "\t";
    for (int j = 0; j != kernel.GetBins(); ++j) {
      cout << kernel.At(i,j) << "\t";
    }
    cout << endl;
  }

  SparseHistogram convolution(13., 26);
  ENSURE(histogram.GetBinWidth()==convolution.GetBinWidth(), "Result must have the same bin width as input");
  ENSURE(histogram.GetBinWidth()==kernel.GetBinWidth(), "Kernel must have the same bin width as input");
  

  Convolve(histogram, kernel, convolution);

  cout << endl << "CONVOLUTION:" << endl;
  cout << "i\\j\t";
  for (int j = 0; j != convolution.GetBins(); ++j) {
    cout << j << "\t";
  }
  cout << endl;
  for (int i = 0; i != convolution.GetBins(); ++i) {
    cout << i << "\t";
    for (int j = 0; j != convolution.GetBins(); ++j) {
      cout << convolution.At(i,j) << "\t";
    }
    cout << endl;
  }

  std::vector<SparseHistogramIndex> cbins = convolution.GetNonEmptyBins();
  std::vector<SparseHistogramIndex> cbins2;

  const int nbins = histogram.GetBins()+kernel.GetBins()-1;
  SparseHistogram convolution2(nbins*histogram.GetBinWidth(), nbins);

  convolution2.Set(1, 1, 1);
  convolution2.Set(2, 3, 1);
  convolution2.Set(3, 4, 1);
  convolution2.Set(3, 6, 1);
  convolution2.Set(4, 2, 1);
  convolution2.Set(4, 3, 1);
  convolution2.Set(4, 5, 1);
  convolution2.Set(6, 6, 2);
  convolution2.Set(6, 8, 1);
  convolution2.Set(7, 8, 2);
  convolution2.Set(7, 10, 1);
  convolution2.Set(8, 9, 2);
  convolution2.Set(8, 11, 3);
  convolution2.Set(8, 13, 1);
  convolution2.Set(9, 7, 2);
  convolution2.Set(9, 8, 2);
  convolution2.Set(9, 9, 1);
  convolution2.Set(9, 10, 3);
  convolution2.Set(9, 12, 1);
  convolution2.Set(11, 13, 1);
  convolution2.Set(12, 8, 2);
  convolution2.Set(12, 15, 1);
  convolution2.Set(13, 10, 2);
  convolution2.Set(13, 16, 1);
  convolution2.Set(13, 18, 1);
  convolution2.Set(14, 11, 2);
  convolution2.Set(14, 13, 2);
  convolution2.Set(14, 14, 1);
  convolution2.Set(14, 15, 1);
  convolution2.Set(14, 17, 1);
  convolution2.Set(15, 9, 2);
  convolution2.Set(15, 10, 2);
  convolution2.Set(15, 12, 2);
  convolution2.Set(20, 20, 1);
  convolution2.Set(21, 22, 1);
  convolution2.Set(22, 23, 1);
  convolution2.Set(22, 25, 1);
  convolution2.Set(23, 21, 1);
  convolution2.Set(23, 22, 1);
  convolution2.Set(23, 24, 1);

//   for (int i = 0; i != convolution.GetBins(); ++i) {
//     for (int j = 0; j != convolution.GetBins(); ++j) {
//       if (convolution.At(i,j))
// 	cout << "  convolution2.Set(" << i << ", " << j << ", " << convolution.At(i,j) << ");" << endl;
//     }
//   }


  ENSURE(convolution.GetEntries()==histogram.GetEntries()*kernel.GetEntries(), "Wrong integral in convolution result");
  ENSURE(convolution.GetBins()==histogram.GetBins()+kernel.GetBins()-1, "Wrong number of bins in convolution result");
  ENSURE(convolution==convolution2, "Wrong convolution result");


  histogram.Reset();
  ENSURE(histogram.GetBins() == 20, "Wrong number of bins after reset");
  ENSURE(histogram.GetEntries() == 0., "Wrong integral after reset");
  ENSURE(histogram.GetNonEmptyBins().size() == 0, "Wrong number of non-empty bins after reset");



}
