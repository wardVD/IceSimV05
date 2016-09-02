#ifndef TIMESHIFTERUTILS_H
#define TIMESHIFTERUTILS_H
/**
 * class: TimeShifterUtils
 *
 * Version $Id: $
 *
 * date: $Date: $
 *
 * @author olivas
 *
 * (c) 2006 IceCube Collaboration
 */
#include <icetray/I3PointerTypedefs.h>
I3_FORWARD_DECLARATION(I3Frame);

namespace TimeShifterUtils {			    
  void ShiftFrameObjects(I3FramePtr frame, 
                         double dt, 
                         const std::vector<std::string>& skip_keys,
                         const std::vector<std::string>& shift_i3double_keys);
};

#endif //TIMESHIFTERUTILS_H
