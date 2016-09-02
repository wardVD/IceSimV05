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
 *   @brief FinalStateTableReader implementation file
 *
 */
//////////////////////////////////////////////////////////////////////
#include "neutrino-generator/table-interface/FinalStateTableReader.h"
#include <fstream>
#include <icetray/open.h>

using namespace std;

namespace nugen {

//////////////////////////////////////////////////////////////////////
FinalStateTableReader::FinalStateTableReader(){
  Imax=0;      // number of steps in the energy range
  Nfinal=0;    // number of final states per energy
  Ndim=0;      // number of dimensions of final state
  Emin=0.0;    // minimum of the energy range
  Emax=0.0;    // maximum of the energy range
}
//////////////////////////////////////////////////////////////////////
FinalStateTableReader::~FinalStateTableReader(){
}
//////////////////////////////////////////////////////////////////////
void FinalStateTableReader::ReadFinalStateTableFillArray(const string& infile){
   // final file table format:
   // line 1 : Nfinal Ndim Imax Emin Emax
   // line 2 to end : (Bjorken)x  (Bjorken)y

   log_trace("final file %s",infile.c_str());
   boost::iostreams::filtering_istream tablefile;
   I3::dataio::open(tablefile, infile);

   if (!tablefile)
     log_fatal("Failed to open file: %s", infile.c_str());
   
   tablefile >> Nfinal;              // number of final states per energy
   tablefile >> Ndim;                // number of dimensions of final state
   tablefile >> Imax;                // number of energy steps and
   tablefile >> Emin >> Emax;


   log_trace("Table array size [Imax][Nfinal][Ndim] = [%d][%d][%d]", Imax, Nfinal, Ndim);
   log_trace("Maximum Table array size [%d][%d][%d]",  MaxEnergyBins, MaxFinalStates, MaxDimensions);

   if(Imax>MaxEnergyBins)log_fatal("table index too large %d, %d",Imax,MaxEnergyBins);
   if(Nfinal>MaxFinalStates)log_fatal("table index too large %d, %d",Nfinal,MaxFinalStates);
   if(Ndim>MaxDimensions)log_fatal("table index too large %d, %d", Ndim,MaxDimensions);
  
   boost::array<TableType::index, 3> shape = {{ Imax, Nfinal, Ndim }};
   TableArray.resize(shape);

   for (int i=0;i<Imax;i++)
     for (int j = 0; j < Nfinal; j++)
       for (int k=0; k < Ndim; k++)
	 tablefile >> TableArray[i][j][k];
   
   if (tablefile.fail())
     log_fatal("Something has gone wrong with reading file: %s", infile.c_str());

}    
//////////////////////////////////////////////////////////////////////
void  FinalStateTableReader::SampleFinalState(const double log_e, double randomx, double randomy, double final[]) {

    int    energy_index;
    int    random_state;

    double dilow = (log_e-Emin)/(Emax-Emin)*(Imax - 1);  // calculating lower index  
    int ilow = int (dilow);
    if (ilow<0){
	energy_index = 0;
    } else if (ilow >= Imax-1){
	energy_index = ilow;
    } else {
	energy_index = ilow+1;
	if (randomx > dilow - ilow)
	    energy_index = ilow;
    }
    if (log_e <= Emax && log_e >= Emin){
	random_state = int (randomy*Nfinal);
	for (int i=0; i < Ndim; i++) {
	    final[i]=TableArray[energy_index][random_state][i];
	}
    } else {
	for (int i=0; i < Ndim; i++) 
	    final[i] = 0;    // set finalstate to 0
    }
 }
//////////////////////////////////////////////////////////////////////

}

