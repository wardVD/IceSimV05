
TriggerSim Segment
~~~~~~~~~~~~~~~~~~

The TriggerSim segment includes the following modules :

 * SimpleMajorityTrigger
 * ClusterTrigger
 * CylinderTrigger
 * SlowMonopoleTrigger
 * I3GlobalTriggerSim
 * I3Pruner
 * I3TimeShifter

All of the above are added conditionally, with the exception of the I3GlobalTriggerSim, 
which is always included.  The trigger modules : SimpleMajorityTrigger, ClusterTrigger, 
CylinderTrigger, and SlowMonopoleTrigger will only be added if there's a configuration
in the GCD file.  The I3Pruner and I3TimeShifter can be disabled via segment parameters,
but it's not advised.  You should really know what you're doing and the purpose they 
serve before disabling them.

Parameters
^^^^^^^^^^

* **tray** - Standard for segments.
* **name** - Standard for segments.
* **gcd_file** - The GCD (I3File) that contains the trigger configuration.  Note the segment figures out from the GCD file which trigger modules need to be loaded and configures them accordingly. 
* **prune** (DEFAULT = True) - Whether to remove launches outside the readout windows. Nearly everyone will want to keep this set at True.  It makes simulation look more like data.
* **time_shift** (DEFAULT = True) - Whether to time shift time-like frame objects.  Nearly everyone will want to keep this set at True.  It makes simulation look more like data.
* **time_shift_args** (DEFAULT = dict()) - dict that's forwarded to the I3TimeShifter module.  See below for more details.
* **filter_mode** (DEFAULT = True) - Whether to filter frames that do not trigger.

I3TimeShifter
^^^^^^^^^^^^^

The I3TimeShifter module currently has two parameters :

* **SkipKeys** - Do not shift objects with these keys.
* **ShiftI3DoubleKeys** - Do shift I3Doubles with these keys.

Here's an example where you might want to shift some frame objects, but not others.::

  time_shift_args = dict()

  time_shift_args["SkipKeys"] = ["DontShiftMe"]
  time_shift_args["ShiftI3DoubleKeys"] = ["ShiftMe"]


  tray.AddSegment(trigger_sim.TriggerSim, "trigger", time_shift_args = time_shift_args)

