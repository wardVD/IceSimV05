Introduction
================

This project is an IceTray interface for WimpSim (*v3.05* and above), the WIMP MonteCarlo by J. Edsjö et al. WimpSim (which is written in *FORTRAN77*) has to be run separetly, and the generated file is then given as input to WimpSimReader.

The file name can be a list of several WimpSim files separated by simcolon (';'). Wimpevent files are named ``we-m[mass in GeV]-ch[annihilation channel 1-18]-[sun or earth].dat``, e.g. ``we-m1000-ch5-sun.dat``.

If you just want to simulate a whole year of data its best practice right now to take the GCD-startday and add 365.25 days for the enddate.

**Remark**: All Units are in native IceCube Units (I3Units::nanoseconds, meter, radians) if not specified otherwise.
Coordinates are always given in the Cartesian system of IceCube (origin close to string 26 in 1948m depth; z-axis positivly pointing towards the sky).


Generated Volume
=================

WimpSim-Reader does compute the generated Volume depending on its configuration in two different ways:

Constant generation Volume
^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is the default Option: A constant volume is generated. This is a orthogonal Box in the limits specified in the Parameter ``PositionLimits``. If the parameter ``InjectionRadius`` is specified a vertical Cylinder is taken with the specified Radius and the z-Limits of ``PositionLimits`` (the x,y-limits will still be used to shift the centre of the cylinder, which is but not recommended).
A vertex/event will so get a isotropic random position inside this generated Volume.

Sensitive Volume
^^^^^^^^^^^^^^^^
This option must be indicated and specified by the Parameters ``SensitiveHeight`` and ``SensitiveRadius``. I will generate a cylindircal shaped sensitive Volume with the specified radius and height around the Zero-point of the detector coordinate system (if not shifted of by asymmetric ``PositionLimits``).
If this Option is specified the generated Volume of this particle will be computed by a generated legth: This is a characteristic of the secondary particle (lepton) type and its energy. It is a measure of the total length the particle has inside the detector plus a distance it takes for photons generated at the stop point of the lepton to dy out.
From this generated length and the requirement that the sensitive Volume must be traveresed by the particle track a (not nessessary rectangular) box is genrated. The vertex is now isotropic randomly positioned inside this box. The enclosed Volume is the generated Volume of this event.
This Option neccessarily requires the implementation of this lepton type specific function that returns the generated length. If such an implementation does not exist for this lepton type, a fall-back to the Constant Generation Volume does exist.

Which one to choose?
^^^^^^^^^^^^^^^^^^^^
The Sensitive Volume is superior to the constant generated Volume, as it gives more exact results on a lower statistics data samples, because it implements individual weights. However, in order to use it correctly a detailed study has to be done, that the computed generated length is chosen correctly and that the sensitive Volume limits are chosen big enough that all physical aspects resulting in a detector response are contained; also it should not be chosen too big so that light from particles dies out before it could reach the detector.

To choose the right settings and implications it is recommended to read: 
  Daan Hubert, "Aspects of neutralino weights and effective Volumes", IceCube Internal Report , Vrije Universiteit Brussel, 9. Oct 2007.

Parameters
===========
I3WimpSimReaderServiceFactory takes 15 parameters, whereof **the first 4 are mandatory**:
  * `FileNameList` [No Default] List of WimpSim(.dat)-files to read
  * `StartMJD` [No Default] MJD to start simulation
  * `EndMJD` [No Default] MJD to end simulation
  * `NEvents` [Default=0] Number of events to issue, 0 means all events in file
  * `PositionLimits` [Default=[-800,800,-800,800,-800,800].] Array of [xmin,xmax,ymin,ymax,zmin,zmax] in [m]. specifies an rectangular box in which particles should be injected.
  * `InjectionRadius` [Default=NAN] If >0, events will be injected in cylinder with specified radius and [zmin, zmax] height instead of rectangular box
  * `LowerZenCut` [Default=0deg] optional lower Zenith Cut in [rad] for Injection
  * `UpperZenCut` [Default=180deg] optional upper Zenith Cut in [rad] for Injection
  * `UseElectrons` [Default=false] Read and simulate electron vertices
  * `UseMuons` [Default=true] Read and simulate muon vertices
  * `UseTaus` [Default=false] Read and simulate tau vertices
  * `UseNCs` [Default=false] Read and simulate Neutral Current vertices
  * `Oversampling` [Default=0] N oversamples taken
  * `SensitiveHeight` [No Default] Muon box activated height in [m] (through center of PositionLimits)
  * `SensitiveRadius` [No Default] Muon box activated radius in [m] (around center of PositionLimits)
  * `InfoFileName_` [Default=''] if anything other than an empty string is specified, status information will be written to this file
  * `RandomServiceName` [Default=''] Name of the RandomService which will be used

Oversampling is configured so that a value of 1 means *do one oversample* equals to *deliver the original and one oversampled event from the same incidently read event*.

there are currently the following **HARDCODED** options, *modify them in the code only with great caution*:
  * `WriteInfoFrameOpt_` [Default=True] to store the delivered and total weight maps into a 'W'-frame
  * `WriteDrivingTimeOpt_` [Default=False] to also write "DrivingTime" which is needed for later simulation processing
  * `pFrameOpt_` [Default=False] do write P-Frames instead of Q-Frames; this is for backwards-compatibility
  * `flatMapOpt_` [Default=False] do write a flat I3StringDoubleMap for the WIMP_params
  * `fixGenVolume_` [Default=100m] fall-back if no generated Length can be computed


Output to the frame
=====================

Event by event
^^^^^^^^^^^^^^
If everything went fine the WimpSim-reader will put the following objects into the Q-frame:
  * I3EventHeader I3EventHeader:
  
    - StartTime (mjd of event or GCD time)
    - Event-Number (from file)
    - File Number (index of file in FileNameList)

  * I3Time DrivingTime: (needed for simulation purposes)

    - Time of the vertex (mjd of event or GCD time)

  * I3MCTree I3MCTree:

    - Primary: Nu (StoppingTrack, Null)
    - Child: Lepton (InIce, StartingTrack/Cascade)
    - Child: Hadron (InIce, Cascade)

  * I3Map_or_I3WimpParams WIMP_params

    - WIMP Mass
    - Annihilation Channel
    - Source
    - neutrino weight
    - leptonic weight
    - hadronic weight
    - Generated Volume
    - Time of the vertex

Info Frame 'W'
^^^^^^^^^^^^^^
At the beginning a WimpFrame(``W``-InfoFrame) will be written that contains all configuration parameters:
  * I3Map WimpSim_Params
  * I3Map WimpSimReader_Params
  * I3Map Nu_Osc_Params

Tail Info Frame 'W'
^^^^^^^^^^^^^^^^^^^^^^^^^^^
At the end another WimpFrame(``W``-InfoFrame) will be written containing all information of about the processed events and weight:
  * I3Map Delivered_Weight
  * I3Map Total_Weight

general for output
^^^^^^^^^^^^^^^^^^
The ``W`` frame will propagate information through the following ``Q``/``P`` stream (similar what GCD-frames do). This information is for example the oscillation parameters which are base-lining the WIMP event distributions; It's basically only a read-off from the wimpfile, but it is better to propagate it into the tray somewhere than to go back to a dusty place with spiders to unearth this information from the .dat or wimpsim-configuration files.

To calculate the a corresonding right rates you must multiply by the weights WIMP_params.vgen times:
  WIMP_params.lep_weight/sum(WIMP_params.vgen) 

In order to calculate the averaged effective volumes you must devide by the sum of all weights at generator level:
  WIMP_params.vgen*WIMP_params.lep_weight/sum(WIMP_params.lep_weight)

Timing in simulation
=====================
There has been made serious improvements to the timing of events and how timing in general is treated. This is also reflected in the *new* parameters ``StartMJDStart`` and ``EndMJD``. These have different function for earth and sun events, but also in the manner how they are configured.

The program will inform and warn if you are using these parameters in a possible wrong way.
Timing will always be saved to the ``I3EventHeader`` which will store timing information relevant for simulation processing but might be slightly modified (by a global trigger for example). ``WIMP_params`` will always store the *correct* timing information as read from the wimp event itself or show an empty field for earth wimps.

In the following section abreviations will be used : "S"tartMJD and "E"ndMJD

MJD Usage for Sun events
^^^^^^^^^^^^^^^^^^^^^^^^
Sun events are (angular) time-dependent, and the correct treatment of time-stamps is very important.
  * If both, S and E, are not configured or set to NAN: All events will be read from file, and no cuts applied
  * If one or both are configured: Only events which originated from a time window [S, E] will be read, all others will be discarded.

MJD Usage for Earth events
^^^^^^^^^^^^^^^^^^^^^^^^^^
While earth events intrinsically are not (angular) time-dependent, it is but still necessary to assign them a time-stamp for later simulation treatment. This can now be easily done.
  * If both, S and E, not configured or set to NAN: this operational mode is not permitted
  * If S == E: all events will get a fixed time-stamp; This option should be used if you want to specify for example the run start of the first run in the year.
  * If both are configured: Events will be randomly assigned a MJD in the time window [S, E]; this option is useful if you want to generate a whole year of simulation


Test and Example scripts
=========================
Have a look at ``resources/scripts/histogramming.py`` for a script that does collect and plot a lot of information on the overall event distribution
