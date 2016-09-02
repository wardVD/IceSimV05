#ifndef NuGDECAYSTATETABLEREADER_H
#define NuGDECAYSTATETABLEREADER_H
/**
 *   Copyright  (C) 2006
 *   The IceCube collaboration
 *   $Id: $
 *   @version $Revision: $
 *   @date $Date: $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   Class: DecayStateTableReader
 *
 *   @brief  DecayStateTableReader assumes a table of 
 *           final states, of the structure Array[sample_number][6], 
 *           where Array[S][2] is assumed to be 
 *           
 *           selected given sample state S.
 * 
 */
///////////////////////////////////////////////////////
#include <icetray/I3Logging.h>

namespace nugen {

class DecayStateTableReader {

  //Maximum number of sampled decay states
  static const int SizeSample = 16384; //default 10000
  //Maximum number of final decay states
  static const int SizeDecayState = 8;//decay state 6

 public:
  DecayStateTableReader();
  virtual ~DecayStateTableReader();
  SET_LOGGER("I3NuG");
  
  void  ReadDecayStateTableFillArray(const std::string& filename);  

  void SampleDecayState(double x, double final[]); 

 private:
  // number of sampled states
  int       Nsample;    
  int       Ndecay;
  double    TableArray[SizeSample][SizeDecayState];
};

}
#endif //DECAYSTATETABLEREADER_H




