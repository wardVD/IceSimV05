/**
 *  $Id: I3PhysicsTimer.h 1432 2012-11-04 16:17:04Z nwhitehorn $
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#ifndef ICETRAY_I3PHYSICSTIMER_H_INCLUDED
#define ICETRAY_I3PHYSICSTIMER_H_INCLUDED

#include <sys/time.h>
#include <sys/resource.h>

#include <string>
#include <icetray/I3Frame.h>
#include <icetray/I3PointerTypedefs.h>

struct I3RUsage : public I3FrameObject 
{
  // in I3Units of time.
  double systemtime, usertime, wallclocktime;

  template <typename Archive>
  void 
  serialize(Archive &ar, unsigned version);
};
  
I3_POINTER_TYPEDEFS(I3RUsage);

class I3PhysicsTimer
{
  struct rusage start_rusage_, end_rusage_;
  struct timeval start_wallclock_, end_wallclock_;

  I3FramePtr frame_;
  std::string module_name_;

  double 
  delta(const struct timeval &starttime, const struct timeval& stoptime);

public:

  I3PhysicsTimer(I3FramePtr frame, const std::string& module_name);
  ~I3PhysicsTimer();

};
  


#endif
