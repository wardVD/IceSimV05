from icecube import icetray
from icecube import dataclasses
from icecube import simclasses
from numpy import isnan

from icecube.trigger_sim import ShiftFrameObjects

class I3TimeShifter(icetray.I3Module) :
    '''
    This module shifts the time of everything time-like in the frame.
    The reference time is determined by the **earliest** time in the
    trigger hierarchy, in the case of triggered events, or the earliest
    I3MCPE(or IMCHit) time, for non-triggered events.

    I3Doubles are not shifted by default.  There is absolutely no way
    to know whether they're time-like objects or not.  Most of the time
    they won't be, so the user has to explicitly state which one's need
    to be shifted.
    '''
    def __init__(self,context):
        icetray.I3Module.__init__(self,context)

        self.AddOutBox("OutBox")

        self.AddParameter("SkipKeys",\
                          "Don't shift these frame objects.",\
                          list())

        self.AddParameter("ShiftI3DoubleKeys",\
                          "Do shift these I3Doubles.",\
                          list())

    def Configure(self):
        self.skip_keys = self.GetParameter("SkipKeys")
        self.shift_i3double_keys = self.GetParameter("ShiftI3DoubleKeys")

    def DAQ(self,frame) :
        '''
        Determine the time shift and apply to known frame objects with time members.
        Rules :

        1) The reference time is determined by either the earliest trigger time or,
           if there's no trigger, the earliest MC PEh/hit time.
        2) Every object is put back in the frame with the same name.

        '''

        # DELTA_T is the global time shift.
        # This will be stored in the frame as an I3Double with the key "TimeShift".
        DELTA_T = 0.

        # Fill the ttimes list with either I3MCHit times
        # or trigger times.  The earliest time will be DELTA_T.
        ttimes = list()
        for trigger_hierarchy in [f for key,f in frame.items() \
                                  if isinstance(f, dataclasses.I3TriggerHierarchy)] :
            ttimes = [t.time for t in trigger_hierarchy if t.fired]                      
        if len(ttimes) == 0 :
            # These are untriggered events, so look for the earliest I3MCHit or I3MCPE.
            for mcmap in [f for key,f in frame.items() \
                          if isinstance(f, dataclasses.I3MCHitSeriesMap) or \
                          isinstance(f, simclasses.I3MCPESeriesMap)]:
                for omkey, series in mcmap:
                    ttimes.extend([o.time for o in series])

            if len(ttimes) == 0 :
                # check for DOMLaunches.
                # this is for trigger hitspool data
                for launchmap in [f for key,f in frame.items() \
                            if isinstance(f, dataclasses.I3DOMLaunchSeriesMap)]:
                    for omkey, series in launchmap:
                        ttimes.extend([o.time for o in series])
                    
        # Calculate the earliest time.
        if ttimes :
            DELTA_T = min(ttimes)
            
        if isnan(DELTA_T) :
            raise ValueError("DELTA_T is 'NaN'")            

        # This is where the shifting is done.
        # The code is in trigger-sim/utilities/TimeShifterUtils.h(cxx)
        # This shifts everything by type unless the user explicitly
        # excludes it.  This is going to be most problematic for
        # retriggering with non-time-like I3Doubles in the frame.
        ShiftFrameObjects(frame,
                          DELTA_T,
                          self.skip_keys,
                          self.shift_i3double_keys)

        frame["TimeShift"] = dataclasses.I3Double(DELTA_T)
        
        self.PushFrame(frame)

