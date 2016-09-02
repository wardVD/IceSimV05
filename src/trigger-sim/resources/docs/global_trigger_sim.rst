I3GlobalTriggerSim
~~~~~~~~~~~~~~~~~~

.. image:: global_trigger_illustration.png

This module is responsible for building the trigger hierarchy for the
event. It's job is to collect the triggers from the various modules
(all expected to be in "I3TriggerName"), determine trigger overlaps and
merge accordingly, inserting either Throughput or Merged triggers to
mimic the hierarchy produced by the real Global Trigger running at the
pole. The output is a single trigger hierarchy (called
I3TriggerHierarchy in the frame).

* **I3TriggerName** - Name of the frame object which contains the
  triggers from the individual trigger modules.
* **GlobalTriggerName** - Name of the output trigger hierarchy.
* **FilterMode** - Determines whether to push frame objects when there's
  no trigger. By default only triggered events pass the filter. This
  must be set to "False" if you want to keep non-triggered events.
* **I3ReadoutWindowBefore** - Readout window before the trigger
* **I3ReadoutWindowAfter** - Readout window after the trigger
* **I3ReadoutWindowOffset** - Readout window offset w.r.t. trigger
* **I3DOMLaunchSeriesMapNames** - This holds the DOM launches used to
  determine event lengths for untriggered events.
* **TimeIncrement** - Time increment between frames.
* **FromTime** - Randomly distribute starting I3Times from this time.
* **ToTime** - Randomly distribute starting I3Times to this time.
