Release Notes V1
================

Release 1.9 Notes
-----------------

V01-09-06c
""""""""""

This is another incremental bug fix release.

* noise-generator - added sanity checks on 
  input parameters and lots of tests.
* sim-services - fix to I3TweakDOMStatus.
  It was setting the IceTop bin sizes
  to meaningless numbers.  This only 
  effects IceTop. 
  Added tests and I3DBHistogram puts 
  plots in plots.  Easier than before.
  Removed 'process-root-files' binary.
  It's now obsolete.
* examples-simulation - Cleaned up scripts.  
  Moved the DB scripts to examples.
* trigger-sim - Fix to IceTop trigger.  
  The trigger was being "appended" as
  a child, but is now inserted.  This 
  only effects IceTop.
* hit-constructor - Fixed IceCube effective 
  area when using AMANDA tables.
  Also added the feature of putting 
  the I3MCTree into the frame.
* mmc-icetray  - Fixed I3MMCTrack insertion.  
  Only adds them to the frame
  for particles that make sense.  There 
  are now tests too.
* neutrino-generator - Fixed the path to the 
  tables.  It looks in two locations
  now: I3_TOOLS and I3_TOOLS/share.
* ucr-icetray - checks that the ucr binary exists.  
  This would cause a hang otherwise.
* c2j-icetray - removed cerr statements.
* juliet - large (146M) jar file not check out
  from respository anymore.  Instead it's rsync'ed
  when the project is made.
* corsika++ - fixed setting of particle direction.
  Was missing an offset of pi.
* TWRSimulator - fixed scripts
* romeo-interface - cleaned up scripts

V01-09-06b
""""""""""

This is an incremental release.

The only change is to neutrino-generator.
To accomodate different locations of the
anis tables in I3_TOOLS a parameter
"TablesDir" was added.

V01-09-06a
""""""""""

This is an incremental bug fix release.

* noise generator no longer needs the TWR map
  if it's in IceCube only mode.
* sim-services - I3MCSourceServiceFactory now
  "fills on the gaps" between a previously
  installed trigger status map and the
  default configuration.  This is important
  for TWR triggers that don't yet exist in
  the database.

V01-09-06
"""""""""

This is the TWR release.  
  
* simclasses is a new project.
* TWRSimulator and noise-generator were added.  
* hit-constructor was modified to take both
  trees and lists and decoupled from romeo.
* Parameter MCListName was changed to MCTreeName
* EnhancementFactor was added and the default
  value is 1.  Previously it was retrieved from
  romeo and was nearly always set to 1.5.
* examples-simulation scripts were updated and
  new script was added which generates TWR samples.
* sim-services
* I3MCSourceServiceFactory now only installs default
  values.
* I3Tweak services were added for DOMStatus, Calibration,
  and Trigger.
* trigger-sim was modified to generate TWR triggers; there
  are several.  It also has modules to combine TWR and
  IceCube triggers.
* romeo-interface - SkipPhotoCathodeSimulator set to true by default.

V01-09-05
"""""""""

Mostly an EHE release.  Updates were in
romeo and brutus.

V01-09-04a
""""""""""

The only change in this release was in
neutrino-generator and *ONLY* effects
nu_tau generation.  The particle/anti-particle
assignments of the neutrinos were incorrect
in previous releases.

V01-09-04
"""""""""

This is based on offline software version
V01-09-09a (and V01-09-09a-dp) with the
exception of I3Db and dataclasses.  Here
are the differences...

* I3Db - Modified to handle next year's 23 string
  geometry.  This will get into offline software soon.

* dataclasses - ** I3Particle **  The version of
  I3Particle that's in offline software is specific
  to the use of I3MCTrees.  So it does not serialize
  the composite list, nor does it have methods to
  set the particle ID, parent ID, or primary ID.
  These methods still exist in the branched version
  of dataclasses used here.  It is also serializing
  boost version 0 particles (i.e. no composite lists).

New Features

* sim-services

  * All parameters are now configurable.
  * Can "add" default calibration and detector
    status objects for strings that are empty
    in the database.  Next year's strings, for example.
  * There are now scripts that dump the database
    parameters to an i3 file and creates histograms
    for the values with gaussian fits.  The fit values
    are used for future strings.

* examples-simulation

  * Cleaned up scripts.  More novice friendly now.
  * Added an InIce-IceTop Coincindent event script.
   
* romeo

  * Updated tables to recent measurements
  * PMT generation is faster now.  Removed
    unnecessary sorts.
  * Tolerance bug-fix for EHE.  Round-off errors would
    cause problems for bin size estimates in EHE events.
   
* romeo-interface

  * Removed AMANDA references.  romeo is now
    an IceCube only simulator.  TWR has its
    own waveform generator. 
  * example scripts cleaned up.
   
* hit-constructor

  * MaxPEs changed behavior.  Instead of skipping tracks
    when MaxPEs is reached, PEs are skipped.  So that
    most any DOM can have is MaxPEs.

Bug fixes

* mmc

  * fixed bug that caused mysterious throws "cannot propagate"
   
* mmc-icetray

  * fixed JNI memory leak
   
* DOMsimulator

  * fixed saturation bug
  * added I3PMTDummy to easily play with and 
    test DOMsimulator.
   
* PSInterface

  * Turned down PSI_Dummy to something more
    reasonable.  It still produces more light
    than photonics (which is good), but not
    much more.  The corsika trigger rate goes
    from 128Hz to 180Hz to give you an idea.
    Before it was on the order of 600Hz.
   
Test fixes

* juliet-interface
* c2j-icetray
* hit-constructor
* trigger-sim
* brutus
* simple-generator

V01-09-03
"""""""""

New features

* DOMsimulator

  * simulates IceTop DOMs including LC
  * produces a map with only LC launches
    (if CleanLC is set to true)
  * 40ns delay was added to discriminator
  * transformer droop now depends on temperature
  * Can simulate TestDAQ

* romeo

  * impedence changed to 43 ohms
  * saturation threshold moved from 4.6e-9 C to 4.6e-8 C
  * bug fix in RMOAngleAcceptance
  * SkipPhotoCathodeSimulator feature added - It's now
    possible to skip the photocathode simulation where
    the number of PEs in hit-constructor is no longer
    scaled up by a factor of 1.5 and there's a more clear
    correspondance between the number of I3MCHits and 
    the number of DOM launches.  This however causes a
    dramatic decrease in the trigger rate for corsika
    events.  The cause is still being investigated.

* sim-services

  * speTreshold changed to match the database
  * local coincidence time window changed to match the database
  * both icetop and inice triggers are loaded into detector status

* examples-simulation
  
  * I3DummyModule added (module stub)
  * I3RomeoCheck - module counts the number of I3MCHits and pmt pulses
  * added a standalone binary that compares two i3 files (sanity-check)

* trigger-sim

  * triggers both on icetop and inice
  * puts a "proper" trigger hierarchy in the frame w/ Global Trigger at top
  * getting the trigger information from the detector status works
    for both the database and the offline dummy mc source.

Projects added

* topsim is back

Running the UCR scripts in examples-simulation (*with* photonics)
I see trigger rates of 172Hz and 169Hz running with dummy status
and the database, respectively.
