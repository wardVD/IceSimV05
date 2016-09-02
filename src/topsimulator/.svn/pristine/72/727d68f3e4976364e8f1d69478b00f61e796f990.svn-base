/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file CorsikaSubBlock.h
 * @version $Rev: $
 * @date $Date: $
 * @author Ralf Ulrich, Peter Niessen
 */


#ifndef _TOPSIMULATOR_CORSIKASUBBLOCK_H_
#define _TOPSIMULATOR_CORSIKASUBBLOCK_H_

#include <fstream>
#include <iostream>

#include <icetray/I3Logging.h>

///
/// \union CORSIKA_WORD
/// Data structure to convert (32bit)FLOAT to 4*CHAR
/// \ingroup CONVERTER
union CORSIKA_WORD
{
    float FLOAT;
    char  CHAR[4];
};


///////////////////////////////////////////////////////////////////////
/// \class CorsikaSubBlock
/// \brief This class defines one sublock from a CORSIKA data file 
/// \ingroup CONVERTER
/// by simple streaming operators (>>) this class can read 
/// subblocks of a CORSIKA file. The SubBlock data and type can easily  
/// accesed.
///////////////////////////////////////////////////////////////////////
class CorsikaSubBlock
{
public:
    
    // Define enumeration types
    enum Block_Type
    { 
	// Run Header
	RUNH, 
	// Run End
	RUNE, 
	// Event Header
	EVTH, 
	// Event End
	EVTE, 
	// Particle Block
	PART, 
	// Longitudinal Block
	LONG,
	// Unknown
	UNKNOWN
    };
    

    CorsikaSubBlock(int BlockSize=0, int AdditionalCharacters=0);
    ~CorsikaSubBlock();
    
    void Reset(int BlockSize=0, int AdditionalCharacters=0);
    
    /**
     * Clears the buffer 
     */
    void Clear();
    
    /// operator[] to access the data of the stored SubBlock
    inline float operator[](int i) const {return m_Buffer[i-1].FLOAT;}
    
    
    Block_Type BlockType();   ///< return SubBlock type of actual SubBlock
    
    void SubBlockCounter(int n) { m_SubBlockCounter=n; }
    int SubBlockCounter() { return m_SubBlockCounter; }
    
    void Dump();

    int findRUNH();
    
    
    int    m_BlockSize;            ///< Actual size of one CORSIKA SubBlock
    int    m_AdditionalCharacters; ///< Number of Additional CHARs after 
    ///  each Block (21 SubBlocks)
    int    m_SubBlockCounter;      ///< Counter of SubBlocks read from file
    
    CORSIKA_WORD *m_Buffer;       ///< The Buffer to store one SubBlock

    
    static const int BLOCKSIZE_THINNED;
    static const int BLOCKSIZE_UNTHINNED;
};


///
/// \fn inline std::ifstream& operator>>( std::ifstream& s, CorsikaSubBlock& b)
/// \brief This function defines the operator>> to stream in CORSIKA data into an CorsikaSubBlock 
/// \ingroup CONVERTER
///
inline std::ifstream& operator>>(std::ifstream& s, CorsikaSubBlock& b)
{
    // based on the code in COAST v4
    s.clear();

    // BLOCK begin
    if (!(b.m_SubBlockCounter%21)) s.ignore(b.m_AdditionalCharacters);
    
    s.read(reinterpret_cast<char*>(b.m_Buffer), b.m_BlockSize*sizeof(float));

    // BLOCK end
    if (!((b.m_SubBlockCounter+1)%21)) s.ignore(b.m_AdditionalCharacters);

    ++b.m_SubBlockCounter;
    
    if(s.bad() || (s.fail() && !s.eof()))
    {
	log_error("An error occured while fetching sub-block %d.", b.m_SubBlockCounter);
	b.Clear();
    }
    
    return s;
}


#endif
