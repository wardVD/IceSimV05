Release Notes V3
================

Release 3.3 Notes
-----------------

This supports the 2012 season.

`IceCuber's Guide to IceSim 3.3 <https://wiki.icecube.wisc.edu/index.php/IceCuber's_Guide_to_IceSim_3.3>`_

V03-03-01
"""""""""

* mmc-icetray - Fixed I3PropagatorService bug that retrieved secondary particle's lifetimes instead of lengths.

V03-03-00
"""""""""

* Uses photonics 1.73 (Includes `time transformation bug fix <http://icecube.wisc.edu/~jfeintzeig/Slides/PhotonicsTimeBug_SimWorkshop8-29.pdf>`_) 
* Update GCD file (MJD 56062 - May 15th 2012) to include the bad DOM list in the D frame.
* Uses the latest offline that contains fixes to DB to support non-int parameters in I3TriggerStatus.
* Update trigger-sim modules to use the tools to extract the configuration from the trigger status.
* using cmake project release V12-01-01 which includes better GEANT4 detection.
* g4-tankresponse builds with GEANT 4.9.5.
* wimpsim-reader - Adapted I3EventService to module ( now it actually works with Q-frames ).
* diplopia - Corrected the multiplicity counter to include the primary or signal track in the count. For NuGen events this means that neutrinos with no CR background events will have a multiplicity value equal to 1.

* .. container:: strikethrough

     mmc-icetray - Fixed I3PropagatorService bug that retrieved secondary particle's lifetimes instead of lengths.

Release 3.2 Notes ("Death Star")
--------------------------------

This release is focused on including cleanups necessary for IC86 2011 production, including two major
bug fixes : NuGen/CORSIKA coincident events and NuGen (Tau and GR) service propagator fixes.  Also
included in this release is clsim and SPICE Lea.

* diplopia - http://wiki.icecube.wisc.edu/index.php/Coincident_Simulation_Fixes 
* neutrino-generator - Fixes for the latest propagator service.  Handles propagation correctly for all channels.
* sim-services - Latest propagator service fixes.
* mmc-icetray 

  * The propagator service switched between AtmFlux and Amanda depending on whether '-prop' is added to the parameters.  
    This is consistent with the module version and the difference bewteen "mode 1" and "mode 3." 
  * The I3MCTree structure is the same regardless of which class is used.  The event structure is not the same on the Java side.
  * The peak of the muon length histogram shifted slightly.  This needs to be investigated further.

* ucr-icetray 

  * InIce location is now set for secondaries.  Important for clsim light generation.

* ppc - New Ice Model ( SPICE Lea ). 
* clsim - Welcome to IceSim! `CLSIM_Code_Review_v01.pdf <https://wiki.icecube.wisc.edu/index.php/File:CLSIM_Code_Review_v01.pdf>`_
* pmt-simulator 

  * Adapt to new I3SumGenerator location.
  * Suppressed compiler warnings about un/signed int comparisons.

* simclasses 

  * I3SumGenerator was moved to sim-services, which is a much more appropriate location for it.
  * Building with ROOT is optional.
  * No need for OLD_MMCTRACK anymore.  This was needed for `very` old IceSim releases ( <# 1.9.6 ).
  * More pybindings for I3MMCTrack.
  * LinkDef.h was removed and was only needed for MMCWeight ( which isn't really used ).

* DOMsimulator 

  * Small fix to I3SumGenerator move.
  * Peakfinder ignores the first bin of the sample when creating SLC waveforms.

* topsimulator 

  * Small fix to I3SumGenerator move.
  * Bug fix from 2.6.6

* cmake - Need a patch so module registrations works on recent Ubuntu platforms


Release 3.1 Notes ("Deth Starr")
--------------------------------

This release is focused on simulation IC86. The date we're using for simulation is May 16th, 2011 ( `MJD # 55697` ). Below are the projects with new features and/or bug fixes.

* trigger-sim - Added SlowMonopoleTrigger and CylinderTrigger ( both new in IC86 ).  There was also a bug fix to the global trigger simulation.  Every now and then ( ~0.5% of events ) very late triggers would be dropped when re-triggering data.  The problem was in GlobalTriggerSim::Merge. That method was a little convoluted and was re-written.  It should be easier to understand and the performance is much better. 

  * Shifting I3MMCTracks again.

* noise-generator - The default scale factor was changed to `0.8949` to scale down the pDAQ rates to match the old testDAQ rates. Logging level increased to ERROR for NaN noise rates.  This is a real error and should be taken seriously.
* BadDomList - A bad DOM list for IC86 was added.
* sim-services - A set of scripts were added to help with GCD validation.  They can be found in sim-services/resources/gcd_validation. Check the `IceCuber’s Guide to IceSim 3.1 <http://wiki.icecube.wisc.edu/index.php/IceCuber%27s_Guide_to_IceSim_3.1>`_ for the recipe.
* hit-maker - Logging level increased to ERROR for NaN relative DOM efficiencies.  This is a real error and should be taken seriously.
* examples-simulation - Updated example scripts to support IC86 simulation.
* neutrino-generator - Three small fixes.  1) Shift particles before propagation 2) A MMCTrackList is only written out when propagators are passed and 3) request suspension when we've generated the specified number of events.
* simclasses - Made members public for I3MMCTrack.  No reason not too.  This should be a simple struct anyway.
* Project clean up - Removed romeo/romeo-interface, juliet/juliet-interface, brutus, and AfterPulseSimulator from the meta-project.  These are only used by the EHE group and are part of the EHE meta-project, so this change should be harmless.

Release 3.0 Notes ("Q*bert")
----------------------------

This mainly includes adaptation to Q-frames, but also the infamous "std::" fixes.  No new features beyond what's in the 2.6 series should exist in this release.

* Nearly all of the projects needed to have their Physics methods changd to DAQ methods.
* In lots of projects "using namespace std" was removed from header files.
* sim-services - I3MCTimeGenerator was replaced with I3MCEventHeaderGenerator
