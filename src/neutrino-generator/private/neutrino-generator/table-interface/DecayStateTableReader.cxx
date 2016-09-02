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
 *   @brief DecayStateTableReader implementation file
 *
 */
//////////////////////////////////////////////////////////////////////
#include "neutrino-generator/table-interface/DecayStateTableReader.h"
#include <fstream>
#include <icetray/open.h>

using namespace std;

namespace nugen {

//////////////////////////////////////////////////////////////////////
DecayStateTableReader::DecayStateTableReader(){

  Nsample=0;// number of decay state sample default is 10000
  Ndecay=0; // number of final states default is 6

  for(int x=0;x<SizeSample;++x)
    for(int y=0;y<SizeDecayState;++y)TableArray[x][y]=0.0;
}
//////////////////////////////////////////////////////////////////////
DecayStateTableReader::~DecayStateTableReader(){
}
//////////////////////////////////////////////////////////////////////
void DecayStateTableReader::ReadDecayStateTableFillArray(const string& infile){

   log_trace("final file %s",infile.c_str());
   boost::iostreams::filtering_istream tablefile;
   I3::dataio::open(tablefile, infile);

   if (!tablefile){
     log_fatal("Failed to open file: %s", infile.c_str());
     exit(EXIT_FAILURE);
   }
   char tDum[64];
   tablefile >> Nsample;              
   tablefile >> Ndecay;                
   tablefile >> tDum;
   tablefile >> tDum >>tDum;

   for (int i=0; i<Nsample; i++)
     for (int j = 0; j < Ndecay; j++)
       tablefile >> TableArray[i][j];
   
   if (tablefile.fail()){
     log_fatal("Something has gone wrong with reading file: %s", infile.c_str());
     exit(EXIT_FAILURE);
   }

}
//////////////////////////////////////////////////////////////////////
void DecayStateTableReader::SampleDecayState(double randomx, double final[]){
   int randomentry= int (randomx*Nsample);
   
   for (int i=0; i < Ndecay; i++) {
     final[i] = TableArray[randomentry][i];     
     log_trace("SampleDecayState %d, %f", i, final[i]);

   }
}
//////////////////////////////////////////////////////////////////////


}




