#ifndef FINALSTATETABLEREADER_H
#define FINALSTATETABLEREADER_H
/**
 *   Copyright  (C) 2006
 *   The IceCube collaboration
 *   $Id: $
 *   @version $Revision: $
 *   @date $Date: $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   Class: FinalStateTableReader
 *
 *   @brief  FinalStateTableReader assumes a table of 
 *           final states, of the structure Array[energy][entriesperenergy][2], 
 *           where Array[E][S][2] is assumed to be x and y for now for given 
 *           energy E and selected given sample state S.
 *
 *           See for example, sample states distributions for CC/NC interactions 
 *           resources/fig/CTEQ5-FinalStates_x_y-Anis-I3Anis.gif
 *           resources/fig/CTEQ5-FinalStates_Theta_E-Anis-I3Anis.gif
 * 
 */
//////////////////////////////////////////////
#include <icetray/I3Logging.h>
#include <boost/multi_array.hpp>

namespace nugen {

class FinalStateTableReader {
  
  //These are not intended as hard limits, just as rough sanity checks to catch malformed table files
  static const int MaxEnergyBins = 1<<10;
  static const int MaxFinalStates = 1<<15;
  static const int MaxDimensions = 1<<2;
  
 public:
  // constructor
  FinalStateTableReader();
  // deconstructor
  virtual ~FinalStateTableReader();
  // logging
  SET_LOGGER("I3NuG");
  
  /**
   * Read a final state table file and fill TableArray
   *
   */
  void ReadFinalStateTableFillArray(const std::string &filename);
  /**
   * function to evaluate by interpolating 3D table 
   * at x between Xmin and Xmax, and y between Ymin and Ymax 
   */   
  void SampleFinalState(const double log_energy, double x, double y, double final[]);  
  
 private:

  // number of steps in the energy range
  int       Imax;      
  // number of final states per energy bin
  int       Nfinal;    
  // number of dimensions of final state
  int       Ndim;
  // minimum of the energy range
  double    Emin;
  // maximum of the energy range
  double    Emax;
  // this is where we store all the data
  typedef boost::multi_array<double, 3> TableType;
  TableType TableArray;

};

I3_POINTER_TYPEDEFS(FinalStateTableReader);

}
#endif //FINALSTATETABLEREADER_H




