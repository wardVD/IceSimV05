/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: MemoryInfo.cxx,v 1.0.0.1 2005/04/13 17:55:03 aya Exp $

    @version $Revision: 1.0.0.1 $
    @date $Date: 2005/04/13 17:55:03 $
    @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>

        Simple memory information class     
        take a snapshot of memory information and print it out
*/
////////////////////////////////////////////////////////////////////////////
#include "neutrino-generator/legacy/MemoryInfo.h"
#include <iostream>

using namespace std;
using namespace nugen;

MemoryInfo* MemoryInfo::mMemoryInfo = 0;

MemoryInfo* MemoryInfo::instance()
{
     if (!mMemoryInfo)
         mMemoryInfo = new MemoryInfo();
     return mMemoryInfo;
 }
 
MemoryInfo::MemoryInfo()
 {
   //#ifndef __APPLE_CC__
   //   mInfo = mOldInfo = mallinfo();
   //#endif
     mCounter = 0;
 }
 
MemoryInfo::MemoryInfo(const MemoryInfo &) {/* private */}

const MemoryInfo&
MemoryInfo::operator=(const MemoryInfo &) {/* private */ return *this;}
 
 void MemoryInfo::snapshot()
{
  //#ifndef __APPLE_CC__
  //  mOldInfo = mInfo;
  //  mInfo = mallinfo();
  //#endif
    mCounter++;
 }

void MemoryInfo::printLine(std::ostream& os, const char* str, int cur, int old, const char* unit)
{
     os.width(40);
     //    os.fill('.');
     os.setf(ios::left);
     os << str << ' ' << cur << " (";
     os.setf(ios::showpos);
     os << cur-old << ") ";
     if (unit) os << unit;
     os << endl;
     os.unsetf(ios::showpos);
}

void MemoryInfo::print(std::ostream& os)
 {   
     os << "----------  Memory Status (snapshot #" << mCounter << ") ----------" << endl;
     //#ifndef __APPLE_CC__
     //printLine(os, "total space allocated from system", mInfo.arena, mOldInfo.arena);
     //printLine(os, "number of non-inuse chunks", mInfo.ordblks, mOldInfo.ordblks);
     //printLine(os, "number of mmapped regions", mInfo.hblks, mOldInfo.hblks);
     //printLine(os, "total space in mmapped regions", mInfo.hblkhd, mOldInfo.hblkhd);
     //printLine(os, "total allocated space", mInfo.uordblks, mOldInfo.uordblks);
     //printLine(os, "total non-inuse space", mInfo.fordblks, mOldInfo.fordblks);
     //printLine(os, "top-most, releasable space", mInfo.keepcost, mOldInfo.keepcost);
     //#else
     //os << "Sorry, MemoryInfo is not supported on Mac." << endl;    
     //#endif
     os << endl;
}
