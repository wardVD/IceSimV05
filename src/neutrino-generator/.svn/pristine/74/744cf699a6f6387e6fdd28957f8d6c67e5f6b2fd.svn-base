#ifndef MemoryInfo_H_INCLUDED
#define MemoryInfo_H_INCLUDED
/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: MemoryInfo.h,v 1.0.0.1 2005/04/13 17:55:03 aya Exp $

    @version $Revision: 1.0.0.1 $
    @date $Date: 2005/04/13 17:55:03 $
    @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>

    Simple memory information class
    take a snapshot of memory information and print it out
*/
////////////////////////////////////////////////////////////////////////////

//#ifndef __APPLE_CC__
//#include <malloc.h>
//#endif
#include <iostream>

namespace nugen {

class MemoryInfo {
 public:
     static MemoryInfo* instance();
     void   snapshot();
     void   print(std::ostream& = std::cout);
     
 private:
     MemoryInfo();
     MemoryInfo(const MemoryInfo &);
     const MemoryInfo & operator=(const MemoryInfo &);
 
     void   printLine(std::ostream&, const char*, int, int, const char* = 0);
     static MemoryInfo* mMemoryInfo;
     //#ifndef __APPLE_CC__
     //     struct mallinfo      mInfo;
     //     struct mallinfo      mOldInfo;
     //#endif
     size_t               mCounter;
};

}

#endif
