.. _trigger-sim:

trigger-sim
===========

**Maintainer:** Alex Olivas

.. toctree::
   :maxdepth: 3

   release_notes
   trigger_sim_segment
   trigger_config_ids
   slow_trigger_monopole
   global_trigger_sim
   pruner
   time_shifter

Overview
~~~~~~~~
This project contains several trigger modules and corresponding utilities.

The Modules
~~~~~~~~~~~

* `SimpleMajorityTrigger <../../doxygen/trigger-sim/classSimpleMajorityTrigger.html>`_ - Simple majority trigger for InIce and IceTop. The simple majority trigger only passes events that contains a certain number of DOMLaunches ("threshold") within a given time window ("timeWindow").  This module works both on InIce and IceTop DOM launches.
* `ClusterTrigger  <../../doxygen/trigger-sim/classClusterTrigger.html>`_ - This trigger emulates the string trigger in IceCube.
* `MultiplicityStringTrigger  <../../doxygen/trigger-sim/classMultiplicityStringTrigger.html>`_ - The trigger formerly known as ULEE.
* `CylinderTrigger  <../../doxygen/trigger-sim/classCylinderTrigger.html>`_ - This trigger emulates the cylinder trigger in IceCube.
* `SlowMonopoleTrigger  <../../doxygen/trigger-sim/classSlowMonopoleTrigger.html>`_ - Slow monopole trigger.
* `I3GlobalTriggerSim  <../../doxygen/trigger-sim/classI3GlobalTriggerSim.html>`_ - Collects the various trigger hierarchies and builds a global trigger.
* `I3Pruner  <../../doxygen/trigger-sim/classI3Pruner.html>`_ - Cleans IceCube DOMs outside of the readout window.
* `I3TimeShifter  <../../doxygen/trigger-sim/classpython_1_1modules_1_1time__shifter_1_1I3TimeShifter.html>`_ - Shifts the times of all known elements in the frame with respect to the the event time.

Utilities Classes and Namespaces
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* `ReadoutWindowUtil <../../doxygen/trigger-sim/classReadoutWindowUtil.html>`_ 
* `GTSUtils  <../../doxygen/trigger-sim/namespaceGTSUtils.html>`_ 
* `DOMSetFunctions <../../doxygen/trigger-sim/namespaceDOMSetFunctions.html>`_ 

Algorithms
~~~~~~~~~~

* `TriggerHit <../../doxygen/trigger-sim/classTriggerHit.html>`_ 
* `TimeWindow <../../doxygen/trigger-sim/classTimeWindow.html>`_ 
* `MultiplicityStringTriggerAlgorithm <../../doxygen/trigger-sim/classMultiplicityStringTriggerAlgorithm.html>`_ 
* `ClusterTriggerAlgorithm <../../doxygen/trigger-sim/classClusterTriggerAlgorithm.html>`_ 
* `CylinderTriggerAlgorithm <../../doxygen/trigger-sim/classCylinderTriggerAlgorithm.html>`_ 


