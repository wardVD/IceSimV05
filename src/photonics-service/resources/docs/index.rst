.. _photonics-service:

=================
Photonics Service
=================

This project contains services to call the photonics i/o functions
for reading photonics tables. The tables contain information about the
number of photoelectrons expected in an OM, and the distribution of
their creation times, relative a nominal "geometric time" which is the
time taken for a direct photon to travel from the source to the OM.

.. toctree::
   :maxdepth: 3
   
   release_notes

Usage
^^^^^ 

I3PhotonicsServiceFactory
-------------------------

The service (I3PhotonicsService) is instantiated by the
I3PhotonicsServiceFactory class which also takes a number of
configuration arguments and passes these to I3PhotonicsService.

Parameters:

- DriverFileDirectory - Directory where the driver files are located 
- PhotonicsAngularSelectionHigh - Maximum zenith angle for photonics tables loading; Default: 180°
- PhotonicsAngularSelectionLow - Minimum zenith angle for photonics tables loading; Default: 0
- PhotonicsInterpolationMode - Photonics interpolation bitmask; Default: 255
- PhotonicsLevel1DriverFile - Name of level 1 driver file; Default: "level1_table.list"
- PhotonicsLevel2DriverFile - Name of level 2 driver file; Default: "level2_table.list"
- PhotonicsTableSelection - Photonics table level, or zero for both; Default: 0
- PhotonicsTopLevelDirectory - Path to the directory containing the subdirectory with tables
- PhotonicsZSelectionHigh - Maximum z for photonics tables loading; Default : DBL_MAX
- PhotonicsZSelectionLow - Description : Minimimum z for photonics tables loading; Default: -DBL_MAX
- ServiceName - Name of Photonics Service; Default: 'I3PhotonicsService'
- UseDummyService - Use dummy photonics service without photon tables; Default: False

In order to use this service you will first need to determine which photon tables you want to use and obtain them.
Determining which photon tables to use is beyond the scope of this document.
The photonics tables used in simulation production can be found at http://convey.icecube.wisc.edu/data/sim/scratch/test_pt/photonics-prod/tables/.
There is no convention for naming photonics tables, and some commonly used photonics tables confusingly have more than one ice model in their name so double check that you are using the correct tables.
If you are not sure which photonics tables to use or can't find the tables you are looking for, ask around.

Photonics tables are often spread across many different files. 
A driver file specifies which files contain which information, level1 and level2 files use different driver files.
You will need to have the correct driver files for the photonics files you wish to use.
Once you have located the driver files, set DriverFileDirectory to the directory of your driver files and set PhotonicsLevel1DriverFile and PhotonicsLevel2DriverFile to the relative path of DriverFileDirectory. 
The driver files contain relative paths, look at the path names in the the driver files to determine the correct value of PhotonicsTopLevelDirectory, you may have to use symlinks to make it work right.

If you set UseDummyService to true it will not load photonics tables and instead use a dummy analytic function to represent light. 
This is useful for testing your script, photonics tables take a long time to load and you can set this to true while you debug the rest of your script.
Then when you are sure everything else works switch UseDummyService to false.

PhotonicsTableSelection specifies what level of photonics tables to use: 0 for both, 1 for level1 and 2 for level2.
PhotonicsInterpolationMode is a bitmask, generally speaking you want to turn all interpolation, so stick with the default.
PhotonicsZSelectionHigh and PhotonicsZSelectionLow allow you to pick the z range of tables to load, you will probably not use this.
ServiceName specifies the name icetray will refer to the service you will not need to change this unless you use more than one photonics service in the same tray.

Muon files suitable for simulating events are typically binned in 10° increments of the muon track.
The full muon tables are often too large for the entire table to be loaded into main memory at the same time, so the work-around is to load the tables for one 10° zenith range at a time. 
Simulation production uses 18 different trays to simulate a the entire zenith range, starting with a 0°-10° zenith band and continuing until it reaches the  170°-180° zenith band. 

The following is an example for simulating muons in the 0°-10° zenith band:
::

   load("libphotonics-service")
   load("libhit-maker")

   tray.AddService("I3PhotonicsServiceFactory","PSService",
                   PhotonicsTopLevelDirectory    = "/path/to/tables",
		   DriverFileDirectory           = "/path/to/driverfile",
		   PhotonicsLevel1DriverFile     = "L1_showers.list",
		   PhotonicsLevel2DriverFile     = "L2_muons.list",
		   PhotonicsAngularSelectionLow  =  0 * I3Units.degree,
		   PhotonicsAngularSelectionHigh = 10 * I3Units.degree,
		   )

   tray.AddModule("I3HitMakerModule","hit-constructor")

The same service is used for reconstruction photon tables as well as for simulation. 
Photorec tables are smaller than simulation tables so that the entire table can be loaded into main memory at once.
Photorec tables only use muon tables so make sure to use the option  ( PhotonicsTableSelection    = 2 ).

The following is an example of using a photon tables based energy reconstruction to estimate the energy of a reconstructed muon:
::

   load("photonics-service")
   load("photorec-llh")

   tray.AddService('I3PhotonicsServiceFactory', ServiceName ,
                   PhotonicsTopLevelDirectory = "/path/to/tables",
                   DriverFileDirectory        = "/path/to/driverfile",
                   PhotonicsLevel2DriverFile  = "level2_table_DEGEN.list",
                   PhotonicsTableSelection    = 2 ,
		   )                    

   tray.AddModule("I3PhotorecEnergyEstimator", "MPEFitPhotorecEnergy",
                  RecoPulsesName         = "OfflinePulses" ,
                  RecoParticleName       = "MPEFit",
                  ResultParticleName     = "MPEFitPhotorecEnergy",
                  MaxDistance            = 450.0 * I3Units.m ,
                  GeoMode                = "AllOMs" ,
                  HypothesisType         = "Lightsaber" ,
                  ConstEventLength       = 1000.0 * I3Units.ns,
                  NoiseRate              = 0.0 * I3Units.hertz ,
		  )


I3PhotoSplineServiceFactory
---------------------------

Parameters:

- AmplitudeTable - Path to the amplitude spline table.
- TimingTable - Path to the timing spline table.
- TimingSigma - Smear the arrival time distribution with a B-spline of order 1 and this standard deviation.
- ServiceName - I3PhotonicsService

I3PhotonicsHitMaker
-------------------

Parameters:

- BinWidth - Join photons from a single particle within BinWidth ns into a single MC hit
- CascadeService - Photonics service to use for cascades
- HCList - Name for list of light-producing particles. If set to the empty string (default), no list will be written.
- Input - Name of input particles in frame
- Output - Name of output in frame
- RandomService - Description : A random number generating service (derived from I3RandomService). The default is to get it from the context.
- TrackService - Photonics service to use for tracks
- UnshadowedFraction - Description : Fraction of photocathode available to receive light (e.g. unshadowed by the cable)


Internals
^^^^^^^^^

PSInterface, written by Thomas Burgess, was the original icetray service that interfaced with both photonics and PTD.
It was replaced by photonics-service which allowed it to be faster and more efficient since it only needed to support photonics.
Due to the development of improved light tracking and binning software :ref:`ppc<PPC>` and :ref:`clsim-main` as well as the improvements from storing photon data in splines instead of tables, photonics is being phased out.

Photonics-service currently contains all the code it needs to read photonics tables on its own, it does not depend on the photonics in the $I3_PORTS distribution.
The following files contain code copied form photonics-1.73: `I3PhotonicsL1Reader`_ contains function needed for reading level1 files, `I3PhotonicsL2Reader`_ contains functions need for reading level2 files, and `I3PhotonicsReader`_ contains functions needed for reading both level1 and level2 files.
The global variables used by the level1 reading code was moved into member variables of the `I3PhotonicsL1Reader Header`_ allowing the use of more than one instance of I3PhotonicsServiceFactory.

.. _I3PhotonicsL1Reader Header: ../../doxygen/photonics-service/classI3PhotonicsL1Reader.html
.. _I3PhotonicsL1Reader: ../../doxygen/photonics-service/I3PhotonicsL1Reader_8cxx.html
.. _I3PhotonicsL2Reader: ../../doxygen/photonics-service/I3PhotonicsL2Reader_8cxx.html
.. _I3PhotonicsReader: ../../doxygen/photonics-service/I3PhotonicsReader_8cxx.html

Additional Reading
^^^^^^^^^^^^^^^^^^
The original paper describing photonics:
J. Lundberg, P. Miočinović, K. Woschnagg, T. Burgess, J. Adams, S. Hundertmark, P. Desiati, P. Niessen, "Light tracking through ice and water -- Scattering and absorption in heterogeneous media with PHOTONICS", Nucl. Instr. Meth. A **581**, 619 (2007). `doi:10.1016/j.nima.2007.07.143 <http://dx.doi.org/10.1016/j.nima.2007.07.143>`_. `astro-ph/0702108 <http://arxiv.org/astro-ph/0702108>`_

The photonics package is currently hosted at http://photonics.sourceforge.net/index.php

IceCube photonics wiki: https://wiki.icecube.wisc.edu/index.php/Photonics

IceCube photonics-service wiki: https://wiki.icecube.wisc.edu/index.php/Photonics-service

Automatically generated documentation:  `doxygen <../../doxygen/photonics-service/index.html>`_ docs.
