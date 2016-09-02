#ifndef NuGCROSSSECTIONTABLEREADER_H
#define NuGCROSSSECTIONTABLEREADER_H
/**
 *   Copyright  (C) 2006
 *   The IceCube collaboration
 *   $Id: $
 *   @version $Revision: $
 *   @date $Date: $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   Class: CrosssectionTableReader
 *
 *   @brief  CrosssectionTableReader
 *           read and fill tables of the structure Array[energy]. 
 *           See for example, crosssection distributions 
 *           for CC/NC interactions 
 *           CTEQ5-Crosssection-Anis-I3Anis.gif
 *
 */
////////////////////////////////////////////////////////
#include <icetray/I3Logging.h>

namespace nugen {

class CrosssectionTableReader {

   static const int SizeEnergyBin =  128;//Default CTEQ5 file use 121
   static const int SizeDimensionBin =  2;//Default CTEQ5 file use 1

 private:
   
   // Main storage matrix
   double   TableArray[SizeEnergyBin][SizeDimensionBin];
   /** 
    * x-axis information 
    */
   //number of x bins
   int      Imax;
   //corresponding x-axis range
   double   Xmin;
   double   Xmax;

   /** 
    * y-axis information 
    */
   // if Jmax = 1 this is 1D table
   // otherwise number of y bins
   int      Jmax;       
   // if Jmax > 1 Corrsponding y-axis range
   double   Ymin;       
   double   Ymax;
   
 public:
   
   // constructor
   CrosssectionTableReader();
   // deconstructor
   virtual ~CrosssectionTableReader();
   // logging util
   SET_LOGGER("I3NuG");

   /**
    * Read a crosssection table file and fill TableArray
    *
    */
   void   ReadCrosssectionTableFillArray(const std::string &filename);
   /**
    * function to evaluate by interpolating 2D table 
    * at x between Xmin and Xmax, and y between Ymin and Ymax 
    */
   double EvaluateCrosssection(double x, double y);
   /**
    * function to evaluate by interpolating 1D table
    */
   double EvaluateCrosssection(double x){return EvaluateCrosssection(x,Ymin);};
   
};

I3_POINTER_TYPEDEFS(CrosssectionTableReader);

}
#endif //CROSSSECTIONTABLEREADER_H







