/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file CorsikaSubBlock.cxx
 * @version $Rev: $
 * @date $Date: $
 * @author Ralf Ulrich, Peter Niessen
 */


///////////////////////////////////////////////////////////
/*
  This file provides all the nasty and ugly stuff about CORSIKA
  files. I am sure you don't really want to know...
*/
///////////////////////////////////////////////////////////


#include "topsimulator/injectors/CorsikaSubBlock.h"
#include <cstring>
using namespace std;


const int CorsikaSubBlock::BLOCKSIZE_THINNED   = 312;
const int CorsikaSubBlock::BLOCKSIZE_UNTHINNED = 273;


CorsikaSubBlock::CorsikaSubBlock(int BlockSize, int AdditionalCharacters) 
 :m_Buffer(NULL)
{
    Reset(BlockSize, AdditionalCharacters); 
}

CorsikaSubBlock::~CorsikaSubBlock()
{
    Reset();
}

void CorsikaSubBlock::Reset(int BlockSize, int AdditionalCharacters) 
{
    // BlockSize the size of on CORSIKA SubBlock in REAL*4 (float)
    // AdditionalCharacters the number of extra CHARs after each CORSIKA Block (21 SubBlocks)
    
    m_BlockSize = BlockSize;
    m_AdditionalCharacters = AdditionalCharacters;
    m_SubBlockCounter = 0;
    
    if(m_Buffer) delete [] m_Buffer;
    if(m_BlockSize>0) m_Buffer = new CORSIKA_WORD[m_BlockSize];
    else              m_Buffer = NULL;
    
    Clear();
}

void CorsikaSubBlock::Dump()
{
    cout << "-------------------------------------------------" << endl;
    cout << "SubBlock " << m_SubBlockCounter << ":" << endl;
    for(int i=0; i<m_BlockSize; i++)
    {
	for(int k=0; k<4; k++)
	{
	    cout << m_Buffer[i].CHAR[k];
	}
    }
    cout << endl;
    cout << "-------------------------------------------------" << endl;
}

int CorsikaSubBlock::findRUNH()
{
  static const char *RUNH = "RUNH";
  static const size_t nRUNH = strlen(RUNH);
  int posRUNH = -1;
  for (int i = 0; i < m_BlockSize-4; ++i) {
    if (strncmp(&(reinterpret_cast<const char*>(m_Buffer)[i]), RUNH, nRUNH) == 0) {
      posRUNH = i;
      break;
    }
  }

  return posRUNH;
}

CorsikaSubBlock::Block_Type CorsikaSubBlock::BlockType()
{
    if(!m_Buffer) return UNKNOWN; 
    if(m_Buffer[0].FLOAT == 0) return UNKNOWN;
    
    if(m_Buffer[0].CHAR[0] == 'R' && 
       m_Buffer[0].CHAR[1] == 'U' && 
       m_Buffer[0].CHAR[2] == 'N' && 
       m_Buffer[0].CHAR[3] == 'H')
	return RUNH; 
    
    if(m_Buffer[0].CHAR[0] == 'R' && 
       m_Buffer[0].CHAR[1] == 'U' && 
       m_Buffer[0].CHAR[2] == 'N' && 
       m_Buffer[0].CHAR[3] == 'E')
	return RUNE;

    if(m_Buffer[0].CHAR[0] == 'E' && 
       m_Buffer[0].CHAR[1] == 'V' && 
       m_Buffer[0].CHAR[2] == 'T' && 
       m_Buffer[0].CHAR[3] == 'H')
	return EVTH;

    if(m_Buffer[0].CHAR[0] == 'E' && 
       m_Buffer[0].CHAR[1] == 'V' && 
       m_Buffer[0].CHAR[2] == 'T' && 
       m_Buffer[0].CHAR[3] == 'E')
	return EVTE;
    
    if(m_Buffer[0].CHAR[0] == 'L' && 
       m_Buffer[0].CHAR[1] == 'O' && 
       m_Buffer[0].CHAR[2] == 'N' && 
       m_Buffer[0].CHAR[3] == 'G')
	return LONG;
    
    // If nothing else matches it must be a particle 
    return PART;
}

void CorsikaSubBlock::Clear()
{
    for(int i=0; i<m_BlockSize; i++) m_Buffer[i].FLOAT = 0;
}
