
I3TimeShifter
~~~~~~~~~~~~~
This module shifts the time of everything time-like in the frame. The 
reference time is determined by the **earliest** time in the trigger 
hierarchy, in the case of triggered events, or the earliest I3MCPE
(or IMCHit) time, for non-triggered events.  This module handles multiple 
trigger hierarchies just fine and will find the earliest trigger time of all 
hierarchies in the frame, although you should really have only one hierarchy.  
Every time-like frame object (including I3Double) is shifted by the reference 
time.  For debugging purposes the shift is added to the frame with the 
name "TimeShift" as an I3Double.

`Doxygen generated docs.  <../../doxygen/trigger-sim/classpython_1_1modules_1_1time__shifter_1_1I3TimeShifter.html>`_ 

Rationale
^^^^^^^^^
Types of time
%%%%%%%%%%%%%
There are two types of time in an event and I distinguish them by their C++ type: 
I3Time and double.

I3Time
$$$$$$
This is a class used to denote the absolute time to tenth of ns.
It's instructive to look at the constructor.  To create an object of 
this type you need to specify the year and the number of tenths of
nanoseconds since the beginning of the year, otherwise known as DAQTime.::

  /**
   * @brief creates the object with the given times as the DAQ time
   */
  I3Time(int32_t year, int64_t daqTime);

This is used in only one place in simulation: I3EventHeader.

I3EventHeader is set at the **end** of the simulation chain by I3GlobalTriggerSim.  
The event header consists mainly of a StartTime and an EndTime (both of type I3Time).

 - StartTime = I3DetetorStatus::StartTime + time_of_earliest_DOMLaunch
 - EndTime = I3DetectorStatus::StartTime + time_of_latest_DOMLaunch

double
$$$$$$

Every simulation module other than trigger sim **only** deals with times of type 
double and carry I3Units of time. The origin (t_0 = 0), determined by the 
earliest primary, is set by the generator.  Each subsequent time is taken with 
respect to the earliest primary.

The Problem
$$$$$$$$$$$
The statement of the goal is very simple: **make simulation look like data**.  Before
triggering, DOM launch times are relative to t_0 (the time the primary was injected).
At generation level it's impossible to guess at what time this primary will trigger
the detector, so choosing the earliest primary time to be 0, is just as good as any
other.
  
In data the DOM launch times are relative to earliest trigger time.  It should be
obvious, but we'll state it anyway...there's no way to know this **before** running
the trigger simulation.

Below are options that were considered before creating the I3TimeShifter module.  
They're worth thinking about.

* Option 1 : Do nothing.

  - Pros
    
    ~ Doing nothing takes no work.
  
  - Cons

    ~ The DOM launches will now appear to launch long after the trigger time, which is inconsistent with data.

* Option 2 : Correct just the DOM launch times.

  - Pros

    ~ The DOM launch times are consistent in interpretation and measurement as compared to data.
    ~ There are only two maps to worry about.  InIce and IceTop.

  - Cons

    ~ The DOM launch times are shifted with respect to the low level objects that created them, i.e. I3Particle, I3MCPulse, I3MCPE, etc...

* Option 3 : Correct **every** time in the event.

  - Pros

    ~ Everyone is happy

  - Cons

    ~ This could be very difficult to do in practice, and if not done robustly could lead to time skews between frame objects.

The Solution
$$$$$$$$$$$$

We obviously went with option #3, and it's the job of I3TimeShifter to attempt to shift every 
frame object with a float-type, relative, time-like members. 

Rules
^^^^^

1) The reference time is now determined by either the earliest trigger time or, if there's no trigger, the earliest MC PEh/hit time.
2) The module used to be more strict, but this was problematic for running on retriggered events, where things like the PMT maps weren't kept after processing.  The user still has to explicitly tell the module to skip the frame objects.
3) Every object is put back in the frame with the same name.

Time-like Objects in the Frame
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This module will shift the follow frame objects to align with the trigger time.  All of the following parameters take a list as multiple containers will generally need to be shifted.

**NB** : The current module will shift everything it finds by type, which makes it a bit more aggressive than previous I3TimeShifters.  With the exception of I3Double (which is special), I3TimeShifter will shift every frame object of the types listed below:

* **I3Particle** stored in I3MCTree, I3VectorI3Particle, or the I3Frame
* **I3MCHit** stored in I3MCHitSeriesMap
* **I3MCPMTResponse** stored in I3MCPMTResponseMap (NB : No longer supported.)
* **I3MMCTrack** stored in I3MMCTrackList
* **I3DOMLaunch** stored in I3DOMLaunchSeriesMap
* **I3Trigger** stored in I3TriggerHierarchy and I3VectorI3Trigger
* **I3Double** stored in I3Frame (Only if specified. See below.)
* **I3FlasherInfo** stored in I3FlasherInfoVect
* **I3MCPulse** stored in I3MCPulseSeriesMap

Parameters
^^^^^^^^^^
There are two parameters :

* **SkipKeys** (Default = list()) - List of names of objects not to shift in the frame.
* **ShiftI3DoubleKeys** (Default = list()) - List of I3Doubles in the frame to shift.  By default, I3Doubles are not shifted.

Frequently Asked Questions
^^^^^^^^^^^^^^^^^^^^^^^^^^
1) Q: Why doesn't I3TimeShifter shift I3EventHeader?

A: I3TimeShifter was designed to shift float type times relative to the event's start time (i.e. the earliest trigger time).  I3EventHeader members are I3Time, which specify an absolute time.

2) Q: Why are I3Doubles special?

A: Because there's no way in principle to determine whether they're time like or not.  In general, they won't be, so we assume they're not unless explicitly stated by the user.

3) Q: Why is the absolute event time taken from the I3DetectorStatus start time?

A: Because for the most part it doesn't matter at all in simulation what the absolute time is.  The lepton propagation, photon propagation, and detector simulation works the same in January as it does in July.
