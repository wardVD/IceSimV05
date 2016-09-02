/**
 *   Copyright  (C) 2006
 *   The IceCube collaboration
 *   $Id: $
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   Class: CrosssectionTableReader 
 *
 *   @brief CrosssectionTableReader implementation file
 *
 */
//////////////////////////////////////////////////////////////////////
#include "neutrino-generator/table-interface/CrosssectionTableReader.h"
#include <fstream>
#include <icetray/open.h>

using namespace std; 

namespace nugen {

//////////////////////////////////////////////////////////////////////
CrosssectionTableReader::CrosssectionTableReader(){

  Imax=0;      // number of bins for energy axis
  Xmin=0.0;    // energy minimum in log10
  Xmax=0.0;    // energy maximum in log10
  Jmax=0;      // number of bins per one energy bin, currently just 1
  Ymin=0.0;    // currently not used because Jmax = 1   
  Ymax=0.0;    // currently not used because Jmax = 1
  
  for(int x=0;x<SizeEnergyBin;++x)
    for(int y=0;y<SizeDimensionBin;++y) TableArray[x][y]=0.0; 
}
//////////////////////////////////////////////////////////////////////
CrosssectionTableReader::~CrosssectionTableReader(){
}
//////////////////////////////////////////////////////////////////////
void CrosssectionTableReader::ReadCrosssectionTableFillArray(const string& file_name){ 
  // Crosssection Table format :
  // Line 1: Imax Jmax Xmin Xmax (Ymin Ymax)
  // Line 2: crosssection
  
  log_trace("Read CrosssectionTable %s",file_name.c_str());
  boost::iostreams::filtering_istream tablefile;
  I3::dataio::open(tablefile, file_name);
  
  if (!tablefile){
    log_fatal("Failed to open file: %s", file_name.c_str());
    exit(EXIT_FAILURE);
  }
  tablefile >> Imax >> Jmax;
  tablefile >> Xmin >> Xmax;
  
  if (Jmax > 1) {   // if 2 dimensional table
    tablefile >> Ymin >> Ymax;
  } else  {         // 1 dimensional table
    Ymin = 0;       // filled with dummy values
    Ymax = 1;       // filled with dummy values
  }
  
  if(Imax>SizeEnergyBin||Jmax>SizeDimensionBin)log_fatal("Table Index too large, fix it!!");
  
  log_trace("Fill table of array size [Imax][Jmax] = [%d][%d]", Imax, Jmax);
  log_trace("Table range Xmin %f, Xmax %f", Xmin, Xmax);
  log_trace("Table range Ymin %f, Ymax %f", Ymin, Ymax);
  
  for (int j=0; j<Jmax; j++)
    for (int i=0; i<Imax; i++)
      tablefile >> TableArray[i][j];
  
  if (tablefile.fail()){
    log_fatal("Something has gone wrong with reading file: %s", file_name.c_str());
    exit(EXIT_FAILURE);
  }    

}
////////////////////////////////////////////////////////////////////////////
double CrosssectionTableReader::EvaluateCrosssection(double x, double y){
  
  if(x>Xmax||x<Xmin)
    log_debug("x value %f out of array boundaries %f, %f", x,  Xmin, Xmax);
  
  //log_trace("x-axis bin number: %d, y-axis bin number: %d", Imax, Jmax);

  int    ilow, ihigh, jlow, jhigh;
  double djlow, dilow;
  double eval;

  // calculating lower index 
  dilow = (x-Xmin)/(Xmax-Xmin)*(Imax - 1);  
  djlow = (y-Ymin)/(Ymax-Ymin)*(Jmax - 1);  
  
  ilow  = int (dilow);	       
  jlow  = int (djlow);	       
  
  ihigh = ilow + 1;                          
  jhigh = jlow + 1;
  
  //log_trace("Lower index %d, %d, higher index %d, %d", ilow, jlow, ihigh, jhigh);
  
  if (dilow == Imax-1)  // if x exactly on upper boundary 
    ihigh = ilow;
  if (djlow == Jmax-1)  // if y exactly on upper boundary 
    jhigh = jlow;
  if (dilow < 0 || djlow < 0 || dilow > Imax-1 || djlow > Jmax-1) {
    eval = 0;
    log_debug("index %f, %f outside array boundaries %d, %d", dilow, djlow, Imax, Jmax);
    // checking if index outside array boundaries
  }  else {// interpolating table
    // distance to interpolation 
    double di = dilow - ilow;  
    double dj = djlow - jlow;
    // interpolating the values
    eval  =  TableArray[ilow][jlow];
    eval += (TableArray[ihigh][jlow]-TableArray[ilow][jlow])*di;
    eval += (TableArray[ilow][jhigh]-TableArray[ilow][jlow])*dj;
  }
  return eval;
}
//////////////////////////////////////////////////////////////////////


}





