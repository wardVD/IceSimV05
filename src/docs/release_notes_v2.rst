Release Notes V2
================

Release 2.6 Notes ("Mischief Managed")
--------------------------------------

`IceCuber's Guide to IceSim 2.6 <http://wiki.icecube.wisc.edu/index.php/IceCuber's_Guide_to_IceSim_2.6>`_

V02-06-10
"""""""""

* ppc - Using latest ppc (same as IceSim 3.3) which supports the SPICE-Lea ice model.  V01-01-00 was simply converted to support P-frames and is aptly named V01-01-00-p-frame. 

V02-06-09
"""""""""

* diplopia - Corrected the multiplicity counter to include the primary or signal track in the count. For NuGen events this means that neutrinos with no CR background events will have a multiplicity value equal to 1.
* mmc-icetray - Fixed I3PropagatorService bug that retrieved secondary particle's lifetimes instead of lengths.

V02-06-08
"""""""""

* neutrino-generator - Uses only one propagator service and propagates muons and taus regardless of the neutrino flavor.
* sim-services - Fixes to propagator service so the MCTree is properly updated and uses AtmFlux for tau generation.
* mmc-icetray - Fixes to propagator service for tau generation.
* diplopia - Coincident NuGen/CORSIKA bug fixes.

V20-06-07
"""""""""

* trigger-sim - This is a P-frame version of the latest trigger-sim release (6/19/12) V07-02-00, which is the same release used in IceSim 3.2

  .. note:: This was released after IceSim 3.1 and won't be backported to 3.1 or 3.0.  trigger-sim V07-02-00 first appears in IceSim 3.2.5.

Other changes were necessary to get it to build and run on recent Ubuntu versions ( 11 and 12 ):

* cmake - Need to add '--no-as-needed' to the link line so module registration works correctly.
* jebclasses - I3JSON.cxx doesn't build in gcc 4.5 or later and is disabled.

V02-06-06
"""""""""

* topsimulator - Fixed a bug in the member function GetOptDistance. Several simulation tests showed that the calculation performed by this function was wrong.

  .. note:: This was released after IceSim 3.1 and won't be backported to 3.1.0 or 3.0.  This fix first appears in 3.1.1.

V02-06-05
"""""""""

* diplopia - Fixed two timing bugs that affected coincident nu/mu simulation only. Hit times never satisfied the condition for expanding the dynamic window because the initial size was too large compared to the sampling window. The other bug assumed possitive time increments which is not the case for this type of simulation since muons are thrown within +/-  .5 * timewindow of the neutrino.

V02-06-04
"""""""""

* neutrino-generator - A very minor fix to the writing of MMCTrackLists in the frame.  In the last version this would always be written to the frame even when the intention was to run in the old-style ( MMC as a module ) instead of passing propagator services. In this release when no propagators are passed to neutrino generator no MMCTrackList is written to the frame.  This way old scripts should work as intended.

V02-06-03
"""""""""

* ucr-icetray - This fixes a bug in the flux normalization for 5-component weights in I3Corsika5CompWeightModule. 
  There was also an extra global factor of nevents in the calculation of fluxsum that has been corrected.
* ppc - Simple fix in CMakeLists.txt file

V02-06-02
"""""""""

* neutrino-generator - Now shifts the particles before propagation.
* trigger-sim - I3TimeShifter now shifts I3MMCTracks.
* simclasses - Made I3MMCTrack members public.

V02-06-01
"""""""""

* I3Db - Fix to get latest geometry for IC79
* pmt-simulator - Fixed typo in gain dependent saturation constants.
* ppc - Fixed DOM efficiency - 0.95 -> 1.0

V02-06-00
"""""""""

* Found Zero Zenith Angle Bug (a.k.a. Point-Source Bug)

  * Reported Point-Source Bug fix (6002, 6008, and 6004 have no zenith angle bug any more) 
    Solution was to properly initialize all member variables in I3NeutrinoGenerator ( specifically `simulatePointSource` in this case).

* trigger-sim

  * The Simple Multiplicity Trigger was updated to correctly handle the sliding time window and the subsequent extension 
    of the event.  I3SMTrigger has been replaced with "SimpleMajorityTrigger"

* DOMsimulator 

  * Changes to the LC logic has been revised to correctly treat Local Coincidence (LC) and ATWD dead time. 
  * The default settings now reflect current detector operations. The parameter "FullICRunsInSLC" is set to *True* 
    by default.  This causes I3DOMsimulator to include SLC launches in the InIce/IceTopRawData maps just as they would appear in data.

* neutrino-generator - there was inconsistency of ice geometry (thickness) and Earth's radius between nugen and mmc. 

  * Now the default setting uses NEW ICE/CRUST GEOMETRY (mmc compatible) which is NOT same as old releases.
  * To simulate with old setting, you must specify "crustmodel" parameter as "Legacy".
  * Has the option to take muon and tau propagators as parameters

* ucr-icetray 

  * Has the option to take a muon propagator as a parameter and propagate muons w/o the MMC module.

* Hadron Energy Correction in hit-maker and cmc - There were two major changes to the scaling of the hadron light output made in both hit-maker and cmc.

  * Change of log(energy) -> log10(energy) in the calculation of the RMS of the gaussian.  The net effect is fluctuations will decrease.
  * The treatment of unphysical (>1 or <0) hadron energy fraction samples are no longer set to 0 or 1.  Instead the distribution is resampled.

* hit-maker

  * Properly initialize `ClusJoinDist` and `ClusONMinDist`.  These must be initialized in the constructor as opposed to the Configure 
    method before the GetParameter call.  This is a general IceTray feature and not specific to hit-maker. 
  * The late pulse probability is no longer included in the scaling of meanPEs.  Only the pre-pulse probability is.  The idea being that only 
    pre-pulses are correlated to the photon flux at the photocathode, whereas none of the late/after-pulses are.  They're correlated with photo-electrons.  
    An SPE can become a late-pulse if it scatters off the first dynode, but the mean number of PEs immediately after the photocathode does not change.  
    The net effects is the decrease of the number of PEs by -3.5%.
  * UseRDE default in hit-maker - this is needed for correct simulation of DeepCore. It is now set to true by default.
  * Ignores "Dark" particles - I3Particles whose shape is set to I3Particle::Dark. 

* photonics-service

  *  Treatment of cascade light scaling.  Change to Kowalski scaling for both EM and hadronic cascades.  The net result is an increase of +19.2% more photons.

* number of bare cherenkov photons - Using 32582.0 ( instead of 32440 from photonics).  It's a minimal change (+0.44%), but at least we know 
  where the number comes from. It's calculated with the script photonics-service/resources/examples/bare_cherenkov.py and is the integral of the Cherenkov 
  spectrum including the wavelength dependence of the index of refraction.
* mmc and mmc-icetray 

  * Depth shift of 8m in mediadef.
  * No longer has the option to shift particles.  This should be done in generator with the service version of the propagator.
  * mmc-ictray has a service version of the propagator

* Extensive clean-up in simulation and offline removing all differentiations between InIce and IceTop DOMs that check only a 
  hard-coded DOM number of 60.  This was causing major problems for people doing extension studies for InIce strings that had more than 60 DOMs.  
  There are still scripts and tests that check the DOM number, but the extension guys are unstuck now.
* cmc 

  * Now issues unique IDs to sub-particles.
  * Includes an I3Context-less service version (so no corresponding service factory)

* c2j-icetray

  * Added python bindings to create I3Context-less services

* New Projects

  * AfterPulseSimulator
  * topsimulator
  * ppc
  * photospline
  * cuda 
  * g4-tankresponse

* simclasses

  * Improved pybindings for I3MMCTrack - Added GetI3Particle and GetELost

* sim-services

  * Added tools to aid generator and the new propagator services

* juliet (V00-06-00) Many feature updates.

  * nuetrino oscillation becomes more precisely by including nu_e <-> nu_mu.
  * several neutrino flux is included such as the latest GZK neutrino flux that doesn't touch the Fermi's diffuse gamma-ray flux limit.
  * including a tool to calculate Feldman-Cousins limit.
  * inclusion of interaction matrix based on ZEUS neutirno-nucleon cross section.
  * inclusion of calculation of interaction and neutrino propagation by evaporation of micro black hole.
  * extension of CR flux to 105 GeV according to Tibet measurement.

* juliet-interface (V01-04-01)

  * A small fix to make this run with standard hit-maker. (The location type of in-ice has to be set for generated particles.)

* brutus (V01-02-00)

  * A feature added to enable zenith angle binning. (Imported from the standard hit-maker)
  * A bug fix for the hadron energy correction.

Release 2.3 Notes
-----------------

V02-03-02RC
"""""""""""

This is a release candidate.

* hit-maker
 
  * making a bug-fix release of hit-maker. it needs to put HitBinWidth? in the frame when hit binning is enabled. pmt-simulator needs this to generate the correct template.
  * V00-01-06 (Bug-fix)


* pmt-simulator

  * bug fix for missing HitBinWidth?. This fix was to fill the MPE
    template in the I3PMT constructor. Using an arbitrarily small hit bin width gives a single gaussian as a template. This reproduces the old behavior where pmt-simulator did not know about the width of the binned hits and will work with old versions of hit-constructor/maker that did not put HitBinWidth? in the frame.
  * V01-01-06 (Bug-fix)


* TWRSimulator:

  * update of the stretch factor file. In addition, the code has been updated so that the default value for the parameters point to this new file. The history file also is updated accordingly.
  * V01-04-03 (Bug-fix)

* sim-services:

  * There are additional input files coming from Ty's work. In addition, the code has been updated so that the default values of the parameters point the new files instead of the old ones.
  * V00-06-07 (Bug-fix)

EHE Projects

* romeo

  * Feature update: V00-08-00
  * The parameterization for the saturation has been updated to the newvalue which Suruj derived.

* juliet

  * Feature update: V00-05-00 
  * Added a method to calculate the CR primary energy distribution
  * The detail correction of the earth curvature effect on atmospheric density.


* juliet-interface

  * Feature update: V01-04-00
  * calculate the CR primary energy distribution 
  * Added the interface to the InteractionMatrix object in JULIeT


V02-03-01RC
"""""""""""

This is a release candidate.
Dedicated to TWR simulation.

* TWRSimulator:

  * changed default path for stretch file
  * V01-04-02

* sim-services:

  * added new threshold file and made it the default one
  * V00-06-06


V02-03-00
"""""""""

This is a release.

This release is basically identical to previous release candidate V02-02-15 except following two projects 
of which change is irrelevant to any physics so that they were included here.

.. note::

   our previous full release is V02-02-09. To know the difference between the two full releases (V02-02-09 and V02-03-00), 
   one can read release notes of candidate releases from V02-02-10 to V02-02-15.

* I3DOMsimulator:

  * Removed I3DOMsimulator.cxx-olivas which is not needed anymore.
  * V02-03-06

* wimpsim-reader:

  * Moved accessing context into configure to prevent icetray-inspect failure.
  * V00-02-02

Release 2.2 Notes
-----------------

V02-02-15
"""""""""

This is a release candidate.

To do list for future release:

* Single I3DetectorSimulation module to save memory
* PMT saturation improvement
* Individual DOM sensitivities (perhpas in offline-software)
* Individual noise rates
* One-way LC connections
* Unified threshold treatment for IceTop and InIce
* Save random number generator status for hit-making

Projects:

* neutrino-generator:

  * Removed tau leptons from list of possible inice parents in order to fix the Not-a-Neutrino fatal error.
  * V00-03-05

* wimpsim-reader:

  * the I3WimpSimReader used to skip the first event in the .dat file that was read. This was fixed in this release. All events are read now.
  * V00-02-01

* photonics-service

  * Added parameters PhotonicsZSelectionLow and PhotonicsZSelectionHigh to restrict the z range of the photonics tables to load. (Analogous to PhotonicsAngularSelectionLow/High.)
  * V00-01-05

* hit-maker

  * Added handling of restricted z range for tables in complete analogy to the zenith angle case.
  * V00-01-05

V02-02-14
"""""""""

This is a release candidate dedicated for TWRSimulation.

To do list for future release:

* ability to set up DOM efficiency individually.

Projects:

* TWRSimulator:

  * Incomplete path in default name for stretch file. Added safety checks to see that file opens OK.
  * Switched on hit binning for TWR data.
  * Bug-fix release V01-04-01.

* hit-maker:

  * Added zenith angle check for each particle in I3MCTree before making a call to photonics.
  * Bug-fix V00-01-04


V02-02-13
"""""""""

This is a release candidate dedicated for TWRSimulation.

To do list for future release:

* ability to set up DOM efficiency individually.
* bug fix in nutau events due to mmc-icetray (duplication of MCTree)

Projects:

* TWRSimulator:

  * In TWRWaveformer new "stretching" functionality of the template pulses is added. A text file with stretch factors is read in. This feature can be turned off by setting a parameter.
  * Feature V01-04-00

* sim-services:

  * In the I3TWRMCParamService module, the default dmadd threshold is changed into the newest values with mV units. The new dmadd table is added to a resources/table directory with the name "DMADD_thresh2008_best_mV.dat"
  * Bug-fix V00-06-05

* hit-maker:

  * Set location type in test code (I3Particle default is no longer InIce). Added clearer error message if tests run with undefined I3_BUILD.
  * Bug-fix V00-01-03

* romeo-interface

  * Added new test which checks existence of romeo data dir.
  * Bug-fix V01-03-03

* romeo

  * Small bug fix (removed double free problem) in some modules (they are not used in icetray)
  * Changed return value of RMOParameter::SetDir function (void -> bool)
  * Updated Makefiles for stand-alone mode
  * Bug-fix V00-07-06


V02-02-12
"""""""""

This is a release candidate.

TWR simulation still need improvements for future release.

* mmc:

  * Dima has implemented cross-section re-weighting that allows the user to essentially "force" the muon to have a large stochastic energy loss. There is a new option parameter "rw". When rw#0 (default) mmc behaves normally. When rw!#0 (rw#10 is a typical value) the first muon in the list is taken and a random point along the track inside the detector is chosen. The muon is propagated normally to that point. After that, the cross-section reweighting is done, resulting in a much higher chance of hard energy loss for whichever interaction is next. A weight is also calculated. The muon is then normally propagated again after the re-weighted interaction.
  * Feature V00-04-00

* mmc-icetray:

  * These changes allow the use of the mmc cross-section re-weighting. A weight class MMCWeight is written to the frame when option parameter rw!#0.
  * Feature V03-02-00

* simclasses:

  * The weight class MMCWeight for cross section re-weighting was added.
  * Feature V00-03-00

* sim-services:

  * In the TWRMCParamService, the default DMADD threshold option is changed from 2005 file to 2008 file.
  * Bug Fix V00-06-04

* wimpsim-reader:

  * Parameters to run taus, muons, and electrons separately.
  * Feature V00-02-00


V02-02-11
"""""""""

This is a bug-fix release candidate especially for IceTop.

IceTop and TWR simulation still need improvements for future release.

* topsim:

  * Bugfix: When the "UseXMLData" option was disabled (default setting) the ArrayShowerTracer did not use the correct VEM calibration constants from the GCD-file which caused the number of generated PMT hits to be wrong.
  * Changed some default settings (Disabled "UseXMLData", Enabled "ExtendedPar")
  * V00-03-06

* neutrino-generator:

  * Bug fixing of null pointer when random prob is 0.
  * V00-03-04


V02-02-10
"""""""""

This is a bug-fix release candidate.

IceTop and TWR simulation still need improvements for future release.

The main purpose of this release is to test TWR simulation based on current changes.

* TWRSimulator:

  * New SPE distribution fitted from data.
  * DMADD algorithm can handle multiple hits at same time.
  * Typo error fixed in DMADD_String method.
  * Bug-fix V01-03-03

* photonics-service:

  * Corrected dummy photonics-service photorec calling sequence and added a little test script using this call.
  * Fixed inheritance bug for dummy photonics service (photorec).
  * Made PhotonicsSource constant in GetPhotorecInfo.
  * Bug-fix V00-01-02

V02-02-09
"""""""""

This is a bug-fix release.

IceTop and TWR simulation still need improvements for future release.

* pmt-simulator:

  * bugfix for the possibility of unphysical charges
  * V01-01-05

V02-02-08
"""""""""

This is a release w/ many features and bug-fixes since last release V02-00-15.

IceTop and TWR simulation still need improvements for future release.

This is our first release since V02-00-15.
This release is basically the same as candidate release V02-02-07.
(some ehe projects were updated to fix failing tests.)
There are many features and bug-fixes in current V02-02-08 release compared to previous release V02-00-15.
You can find all details of the changes in previous candidate release notes (from V02-02-00 to V02-02-07).
The notable differences between previous and current releases are:

* Simulation Speed up: ~2 to 3 times faster (CPU time) in generating events
  when hit-maker and photonics-service are used instead of using hit-constructor and PSInterface.

* Better DOMsimulator:

  * FADC bug fix: FADC waveforms are now correctly generated.
  * The relative timing between FADC and ATWD has been fixed.

* pmt-simulator:

  * Applies the time delay from DOMcal to the PMT so that MC Hit time is the time when a photon reaches PMT.

* TWRSimulator:

  * Reads database values for new trigger and acts accordingly.
  * Produces waveforms scaled to calibration pe_area
  * Allan's (hardware) trigger for low energy events was added.

* trigger-sim:

  * Now uses TWR-I3 transit time and global T0 offset from the calibration.
  * Added I3StringTrigger.

* noise-generator:

  * Produces after pulses for TWR according to MC hit weight. Added re-scaling factor for noise rate in deep core DOMs.

* cmc:

  * Default output tree name is changed to "CMCTree".

* Every project's CMakeList.txt has been adapted to work with offline V2.

* tarball_hook.sh.in file

  * Modified to include $ROOTSYS/etc directory in tarball. ROOT 5.18 needs etc/plugins.

* New Projects: hit-maker, photonics-service, simple-injector

There are still important remaining issues:

* InIce Simulation:

  * need to overcome mismatch of spe threshold between data and MC. (Actually, the threshold in data is higher than what it supposed to be.)

* IceTop simulation:

  * Proper treatment of thresholds

* TWR simulation:

  * need to solve spe pulse shape difference.
  * need to understand how pulse threshold was set/interpreted in data
  * need to understand DMADD trigger window
  * need better agreement of trigger rates.


ehe simulation fix for this release

* romeo-interface

  * Fixed failing test.
  * V01-03-02

* juliet-interface

  * Fixed failing test.
  * V01-03-03

* brutus

  * Fixed failing test.
  * V01-00-01

V02-02-07
"""""""""

This is a bug-fix release candidate.

IceTop and TWR simulation still need improvements for future release.

* DOMsimulator

  * Addition of a "if" statement for temporary separate treatment for InIce and IceTop signals.
  * IceTop : old discri calibration + shaping of the signal in the discri chain
  * InIce : new discri calibration, no shaping (as it's included in the description of the threshold
    but looks like improver for IceTop signals) This is done consistently for both SPE & MPE thresholds.
  * Temporary fix to allow first productions with IC40 + IceTop.
  * Proper calibration to get a consistent treatment has to be made.
  * V02-03-05

* TWRSimulator

  * Some algorithm changes in finding M8 trigger.
  * DMADD algorithm changes.
  * Code cleanups and small bugfix.
  * V01-03-02

* sim-services

  * support for the new SPE discriminator threshold.
  * Fixed failing tests.
  * V00-06-03

* diplopia:

  * a couple of configurable paramters were added: option for setting twr flag.
  * bug-fix release V00-01-02

* hit-maker:

  * HIT_MAKER_I3PARTICLES now switched on automatically for Debug builds
  * bug-fix release V00-01-02

* corsikaXX

  * Fixed the test: #includeing a .cxx file lead to a double free when exiting the test binary. Replaced that by including the appropriate header file.
  * bug-fix release V03-00-03

* hit-constructor

  * Fixed failing tests.
  * bug-fix release V01-04-02

* noise-generator

  * Fixed failing tests.
  * bug-fix V01-05-01

* neutrino-generator

  * Fixed failing tests. cmake wasn't including needed projects.
  * bug-fix V00-03-03

* pmt-simulator

  * Fixed single_pulse_weighted test
  * bug-fix V01-01-04

* cmc

  * Included icetray-inspect test
  * Moved all constant definitions into implementation file.
  * bug-fix V01-01-01

* mmc-icetray

  * Fix failing tests.
  * V03-01-03

V02-02-06
"""""""""

This is a bug-fix release candidate.

IceTop DOM threshold issue is still needs to be solved soon.
TWR simulation mismatch w/ data needs to be understood and solved near future.

* tarball_hook.sh.in file

  * Modified to include $ROOTSYS/etc directory in tarball. ROOT 5.18 needs etc/plugins.

* ucr-icetray

  * minor change in I3CorsikaWeightModule - Timescale added to SummaryService so that
    we can visualize it on the production web portal
  * added CORSIKA oversampling factor to the calculation of TimeScale
  * added two new parameters (ThetaMin and ThetaMax), the minimum and maximum CR zenith angles.
    Implemented general expression of AreaSum, based on these two angles. This is to generalize
    the TimeScale calculation
  * an additional bug correction
  * new release - V03-01-04

* mmc-icetray

  * PrimaryTreeName can be set as parameter but it was writing only the default name (#"I3MCTree")
    to the frame. Fixed.
  * new release - V03-01-02

* examples-simulation

  * change in I3BasicHistos : for Monte Carlo retrieve the livetime directly from the weight info
    which is in the frame. Previously livetime used to be set by hand with a special parameter.
    For Nugen normalize events with OneWeight and assume livetime # 1 sec.
  * new release - V02-02-06

* trigger-sim

  * Now uses TWR-I3 transit time and global T0 offset from the calibration.
  * Bug fix to setting parameters of the string trigger
  * new release - V04-02-03

* domsimulator

  * Improvement for the description of the LC simulation for IceTop :
    The LC scheme is not hardcoded relying on a basic LG/HG check anymore but now uses the gain status
    of the DOMs. In the future this method should be replaced by a more general method which uses
    the actual LC-settings provided by the I3DOMStatus. Currently this is not possible
    of the DOMs. In the future this method should be replaced by a more general method which uses
    the actual LC-settings provided by the I3DOMStatus. Currently this is not possible
    since the lcTXMode is missing in the I3DOMStatus.
  * This release should be used for InIce production only. An unified description of the discriminator
    threshold for IceTop and InIce couldn't be achieved here and InIce was chosen to start production.
    It is under progress for a global fix and will be part of next release.
  * new bug-fix release - V02-03-04

* topsim

  * Updated default waveform decay times according to the studies done at DESY
    (see Decay times of IceTop waveforms)
  * Added special treatment of 2005 tanks with respect to waveform decay times
  * new bug-fix release - V00-03-05

V02-02-05
"""""""""

This is a bug-fix release candidate

V02-02-04
"""""""""

This is a bug-fix release candidate

* DOMsimulator: The mode SoftLC was missing for the test on LC mode of isolated DOMs which was causing the skipping of the test and a following crash of the code.

V02-02-03
"""""""""

This is a bug-fix release candidate

* juliet: Syncronized with the JULIeT version 3.3, including modification of bug.
* juliet-interface: Neutrino interaction weight was changed for the Glashow resonance.

V02-02-02
"""""""""

This is a feature + bug-fix release candidate

* photonics-service: Return zero for average number of PEs and log info message if needed photonics tables (level1 or level2) not loaded. Feature V00-01-00
* sim-services: Updates to TWR part to match new TWR MC. Feature V00-06-00
  
  * Bug fix to I3MCCalibrationService - Now adds default droop double tau parameters and PMT transit time linefit values.
  * Bug fix for NANs for not-yet-deployed modules to I3MCSource.

* I3Db: Reads two new string trigger settings into frame. Feature V02-01-00 (tagged by Erik)
* I3DOMsimulator: Skips over DOMs in "headers-only" LCMode. Bug-fix V02-03-01
* pmt-simulator: Check that DOMFunction::TransitTime returns a sane value for the time. Bug-fix V01-01-01
* TWRSimulator: Scaling from charge to amplitude. Safety checks when getting calibration. Bug-fix V01-03-01
* TWRCalibrator: *Not part of icesim*. Modifications to fill MC t0's to GCD file. Feature V01-04-00
* romeo : Bug-Fix V00-07-02

  * Bug fix of PRE-pulse & POST-pulse padding
  * Refined GetSubpeaks function (used only when I3RomeoInterfaceModule::StoreFakeRecoPulseMap option is ON)

* romeo-interface  : Feature V01-03-00

  * Added PMT transit time (referring domStatus and domCalib). It does NOT die if PMT transit time is NAN (use 0ns for transit time instead), be careful...
  * Removed StorePMTPulses option
  * Removed OutputIceCubePMTPulseMap option
  * Added StoreFakeRecoPulseMap option. Default is False. It stores "fake" RecoPulseSeriesMap directly from Romeo.
  * Added OutputFakeRecoPulseMap option (name of fake recopulsemap)

* examples-simulation: a few line of bug fix. BUG-FIX V02-02-01
    
V02-02-01
"""""""""

This is a feature + bug-fix release candidate

* hit-maker: Fixed HadronEnergyCorrection bug and photonics energy scaling. Improved doxygen docs. Added parameter for DOM shadowing fraction (with default#0). Set default shadowing fraction to 10%. Added re-scaling factor for deep core DOMs. FEATURE V00-01-00
* photonics-service: Improved doxygen docs. Incremental V00-00-01
* TWRSimulator: Reads database values for new trigger and acts accordingly. Produces waveforms scaled to calibration pe_area. Feature release V01-03-00
* trigger-sim: In TWRSoftTrigger: adapted to new trigger algorithms. Feature release V04-02-00
* wimpsim-reader: New parameter to set time of simulation (needed for sim-prod). Bug-fix release V00-01-02
* hit-constructor: Fixed HadronEnergyCorrection bug and photonics energy scaling. Added parameter for DOM shadowing fraction (with default#0). Set default shadowing fraction to 10%. Added re-scaling factor for deep core DOMs. FEATURE V01-04-00
* diplopia: Bug in multiple hit-map handling fixed (see IceSim_Release_Candidate_V02_02_00) BUG-FIX V00-00-09
* juliet: Bug fix. The tables for Glashow resonance could not be read correctly. Bug-fix release V00-04-01
* neutrino-generator: Fixed logic that assigns LocationType so that neutrinos are properly handled. BUG-FIX V00-03-01
* DOMsimulator: see IceSim_Release_Candidate_V02_02_00 for modifications around the shaping of the signal and the timing of the traces. A pedestal subtraction option has been added : "PedestalSuppressedData". Default is false. If this option is enabled the ATWDs will be produced with pedestal subtraction. The droop simulation is now enabled by default. FEATURE V02-03-00
* cmc: Default output tree name is changed to "CMCTree". With this change, the existing I3MCTree in the frame is not modified, instead a new tree is put into the frame additionaly. Muon production in hadronic cascades is added and enabled per default. FEATURE V01-01-00

V02-02-00
"""""""""

This is a feature + bug-fix release candidate 

4 new projects added

Every project's CMakeList.txt has been adapted to work with offline V2 by Alex.

* simclasses: Added I3SumGenerator which generates sum of random numbers drawn 
  from the same probability density (specified as a function) by means of lookup tables 
  for different number of terms. FEATURE V00-02-02 
* sim-services: Updated to include new (or previously neglected) calibration and 
  detector status parameters. Also added I3GeoShifter, which provides tools for generators 
  and propagators for translating I3MCTree particles to the reference frame of 
  the detector center. FEATURE V00-05-00. 
* DOMsimulator: Includes new simulation for the timing of waveforms. The delays implied 
  for acquisition have been checked and reimplemented (as a consequence, the relative timing 
  between FADC and ATWD has been fixed). FADC waveforms are now correctly generated : 
  simulation of the preamplification shaping added. Modification of the implementation of 
  the coarse charge stamp simulation (SLC). Modification of the implementation of the noise 
  generation on the baseline (speeds up execution)BUG-FIX V02-02-06 
* PSInterface: No significant changes. Only those to make it work with offline V2. 
  Contains tools for angular selection. FEATURE V02-03-00
* PSInterfaceFactory: No significant changes. Only those to make it work with offline V2. 
  BUG-FIX V01-01-02 (incremental really). 
* c2j-icetray: No significant changes. Only those to make it work with offline V2. 
  BUG-FIX V03-01-02 (incremental really). 
* corsikaXX V03-00-01: 
* hit-constructor: Works with new offline V2. FEATURE V01-03-00
  
  * "Adds" to a pre-existing I3MCHitSeriesMap (really it copies and replaces.)
  * hit times now correspond to photon arrival times.
  * The binned methods of LatePulseGenerator and AfterPulseGenerator now check 
    for a minimum number of hits before being applied. 
* mmc V00-03-07: 
* mmc-icetray: Added the option to shift particles to the progagation cylinder center. 
  FEATURE V03-01-00 
* ucr-icetray: Shifts the particles to the center of the given detector, rather than 
  relying on a shifted detector. FEATURE V03-01-00 
* noise-generator: Can turn on and off the subdetectors individually. FEATURE V01-04-00 
* romeo: Bug fix to read correct PMT data (This bug only applies for only photo-cathode mode). 
  The latest ROOT is supported. FEATURE V00-07-01 
* romeo-interface: No significant changes. Only those to make it work with offline V2. 
  BUG-FIX V01-02-02. 
* juliet: Glashow resonance is included. A function to move the detector center is added. FEATURE V00-04-00 
* juliet-interface: Many features are introduced such as treatment of muon bundles, 
  calculating multiple neutrino fluxes at once, handling Glashow resonance, making an option 
  to allow to move the detector center, and so on. FEATURE V01-03-00 
* simple-generator V00-02-01: 
* neutrino-generator: Works with new offline and shifts the particles to the detector frame V00-03-00. 
* TWRSimulator: Changed TWR Launch map names to resemble data. Better pulseheight spectrum (from AMASIM). 
  Better handling of weighted hits (with sum-generator) FEATURE V01-02-07 
* examples-simulation: Updated scripts to use 40 string geometry. Merged NuGen scripts into 
  one that can handle all three flavors. FEATURE V02-02-00 
* topsim: Switched from $I3_WORK to $I3_SRC in the I3ArrayShowerTracer. BUG-FIX V00-03-04 (Incremental) 
* trigger-sim: Added I3StringTrigger. Renamed I3Conglomerator -> I3GlobalTriggerSim.
  In TWRSoftTrigger: Clears TWRLaunch map if no trigger. This is the experimental case. FEATURE V04-01-01
* pmt-simulator: Applies the time delay from DOMcal to the PMT features. Uses I3SumGenerator 
  for pulse heights of weighted hits (for speed). FEATURE V01-01-00. 
* cmc V01-00-01: 
* wimpsim-reader V00-01-01: 
* diplopia V00-00-08: 
* hit-maker: New project. Faster version of hit-maker, relying on photonics-service. 
  Please see the hit-maker docs. MAJOR V00-00-01
* photonics-service: New project. Photonics-specific photon-table interface which is 
  more efficient than PSInterface. Please see the Photonics-service docs. MAJOR V00-00-00 
* simple-injector: This is a new project. This does the same job as simple-generator 
  but is a bit more versatile in some respects.MAJOR V01-00-00 
* brutus: Again in simulation. Adapted to simV2 (MCTree etc). MAJOR V01-00-00 

Release 2.1 Notes
-----------------

V02-01-02
"""""""""

This is a bug fix candidate

* DOMsimulator - fixed FADC time shift that was
  causing random crashes

V02-01-01
"""""""""

This is a feature release

* mmc

  * bug fixes to loading of MMCTrack parameters
  * CMakeList.txt fixed for multiple Java installations

* mmc-icetray

  * bug fixes to filling of MMCTrack parameters

* noise-generator

  * can switch off noise for each sub-detector

* DOMsimulator

  * bug fixes for SLC

* examples-simulation

  * more plots for BasicHisto

* simclasses

  * fixed I3Units in I3MMCTrack
  * added access methods for I3MMCTrack position

* PSInterface
  
  * bug fixes for muon energy correction

* hit-constructor

  * fills pre-existing hit maps

* diplopia

  * correction to weight calculation

* sim-services

  * added support for RunID in time generator

* trigger-sim

  * I3Conglomerator knows when to increment the EventID

Release 2.0 Notes
-----------------

V02-00-14
"""""""""


This is a bug fix candidate

* TWRSimulator - fixed calculation of time bin

V02-00-13
"""""""""

This is a bug fix candidate

* diplopia - fixed storing of intermediate weighting information in MPHitFilter.

V02-00-12
"""""""""

This is a bug fix candidate

* TWRSimulator - uses info from AOMStatus
* examples-simulation - cleaned up logging

V02-00-11
"""""""""

This is a bug fix candidate

* offline-software - Using V01-11-04
* neutrino-generator - no longer generates events with weight # 1.
* ucr-icetray - change type of spectral index from int to double.
* diplopia - changed the way weights are added to the map.
* TWRSimulator - handles binned hits

V02-00-10
"""""""""

**THIS IS THE RELEASE**

small tweaks to modules and scripts in examples-simulation 

V02-00-09
"""""""""

* topsim - tweaks to VEMCal handling
* diplopia - bug fix to weight calculation 

V02-00-08
"""""""""

Using offline V01-11-04 (w/ latest I3DB which supports VEMCal and TWRCal)

* ucr-icetray - changed the names in the weight dict from TimeScale to LiveTime
* diplopia - correctly calculates weight for combined weighted events

V02-00-07
"""""""""

* hit-constructor - SPETime skips NAN voltages.
* topsim - script works now

V02-00-06
"""""""""

* romeo-interface - deals with late/after/pre pulses

V02-00-05
"""""""""

* hit-constructor - For good measure all cascades are type 1.
* romeo - bug fix for stringstream platform dependence.
* examples-simulation
* trigger-sim

  * Added more debugging info if a TriggerKey is not found.
  * Fixed the frame delete of event header for the THIRD time!?

V02-00-04
"""""""""

* phys-services

  * Bug in IceTop station shift corrected.

* sim-services

  * handles different LCSpans for IceTop and InIce

* trigger-sim 
  
  * I3Conglomerator - Bug corrected in calculation of EndTime.
    Event header bug crept back.  This was also fixed.

* examples-simulation

  * Scripts and histo module updated.

V02-00-03
"""""""""

* hit-constructor 
  
  * Skip DOMs that are outside min and max woltage.
  * More verbose error if a NAN time is generated.
  * Skip DOMs with no Detector Status record.

* PSInterface

  * PSI_Photonics type is always 1 if level is 1 cascades.
    Monopoles are the exception.

V02-00-02
"""""""""

* DOMsimulator - simulate FADC if status is Off but deltaCompress is On.
* Added primary energy to weight map. 

V02-00-01
"""""""""

* trigger-sim - replace event header in frame (I3Conglomerator) 

V02-00-00
"""""""""

This is a major release with quite a few changes.

* sim-services

  * New feature in I3MCSourceServiceFactory.  It
    can skip strings and stations with "DoNotModifyStrings"
    and "DoNotModifyStations."
  * Defaults were updated to reflect latest DOMs and likely future DOMS.
  * I3DBHistogram - PMTGain plot was added.  Other plots updated.
  * TWR AOMStatus information is filled by default.
  * I3MCTimeGeneratorServiceFactory - Added MJD, MJDSeconds, 
    and MJDNanoSeconds to specify the time as an alternative to DAQTime.
  * Added RunNumber parameter.  An I3EventHeader is now inserted into
    the frame with the run number.  I3Db needs this to retrieve the 
    detector status..

* DOMsimulator

  * LC takes into account broken links.
  * Added SLC (Soft Local Coincidence).
  * Simulates droop using double tau instead of single tau.
  * Takes into account differences between old and new DOMs (toroid-wise)
  * Update of ping-pong.  Timing is more realistic now.
  * Electronic noise added to FADC    

* PSInterface

  * Reads monopole tables now.
  * Turned down logging level for PSI_Photonics.
  * Added I3FATInjector which is simple a square wave.

* PSInterfaceFactory

  * Modified to handle I3FATInjector.
  * Reorganized code a bit to make it a bit slimmer and cleaner.

* c2j-icetray

  * Big overhaul.  Now the parameters have to be passed via the
    "Options" parameter.  This is more flexible, there's no need
    for ICETRAY_CLASSPATH anymore.  NOTE: You must pass the jar
    file to I3JavaVMFactory.

* corsikaXX

  * Switched from MCList to MCTree
  * Adapted mmc policy of secondaries
  * Added reading of NKG parameters from shower
  * Speedup by factor of > 10 by discarding particles early. 

* hit-constructor

  * Hit Binning is included
  * After/Pre/Late pulses are now generated.
  * It can handle monopoles and monopole tables.

* mmc

  * Added Antares water (thanks to Claudine Colnard). 
  * MMC user block info is now filled correctly as part of the output 
    Particles for use by external applications. 
  * More memory is made available to java at run time (using an -Xmx512M option).
  * Propagation of particles from outside the Earth is fixed.

* mmc-icetray

  * Handles trees now instead of lists.

* ucr-icetray

  * Puts trees in the frame instead of lists.
  * Adds intermediate pions/Kaons if the corsika information is there.
  * I3CorsikaWeightModule calculates the weight and adds it to the frame.
  * Reads NKG information if it exists.

* romeo

  * Updated PMT constants
  * Moved tables to seperate server.  Build system rsyncs them.
  * Handles pre/after/late pulses

* romeo-interface

  * Handles pre/after/late pulses
  * Updated defaults

* juliet

  * Version 3.0 of JULIeT is now here.

* juliet-interface

  * Uses trees now instead of lists
  * No longer sets the memory options in code.  This should be done through
    c2j-icetray (I3JavaVMFactory) at the script-level.

* simple-generator

  * Puts trees in the frame
  * Bug fix to gammaIndex # 1 and energy setting.

* neutrino-generator

  * Puts trees in the frame

* examples-simulation

  * example scripts updated (moved from scripts to examples).
  * Added Cascade effective volume module/script.
  * Added histogram module which includes "basic" histograms.

* topsim

  * Switched to MCTree from MCList
  * Fixed bug that primary is not necessarily particle number 0
  * VEM calibration built in
  * Added NKGTracer which converts NKG densities into tank singnals 
  * Added Extended Particle Response Parametrisation (also containing
    hadrons etc.) 
* trigger-sim

  * Renamed I3IcecubeTriggerSim to I3SMTrigger.  It no longer does
    both IceTop and InIce at the same time.  It does them seperately,
    so for coincident runs you'll need two instances.

* simclasses

  * Added I3MCNKGInfo.h
  * Moved I3MCPMTResponse.h and I3GaussianPMTPulse.h from romeo.

NEW PROJECTS:

* pmt-simulator - Lightweight IceCube PMT simulator.  Does the
  same as romeo when skipping photo-cathode simulation.  If you
  want to simulate the photocathode response you'll need to use
  ROMEO.
* cmc - Cascade splitter to simulate the LPM effect.
* wimpsim-reader - Reads files generated with wimpsim.
* diplopia - Injects two CORSIKA primaries.
