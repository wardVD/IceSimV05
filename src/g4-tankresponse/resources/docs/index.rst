.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id: index.rst 66546 2010-08-31 18:09:44Z kislat $
.. 
.. @version $Revision: 66546 $
.. @date $LastChangedDate: 2010-08-31 13:09:44 -0500 (Tue, 31 Aug 2010) $
.. @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy: kislat $

.. _g4-tankresponse-main:

g4-tankresponse
===============

This project is maintained by Javier Gonzalez <javierg@udel.edu>

.. toctree::
   :maxdepth: 3
   
   release_notes

About
-----

The g4-tankresponse project provides an I3TankResponse service to be used with
`topsimulator <../topsimulator/index.html>`_. This service calculates the response of an
IceTop tank to incoming particles using Geant4. In order to use this project you need have geant4 installed.
The physics in this project is documented in the original `internal note <http://internal.icecube.wisc.edu/reports/details.php?type=report&id=icecube%2F201207001>`_.

To use this tank response, one only needs to create the service and pass it to topsimulator.
The example in g4-tankresponse/resources/examples/simple_injection.py shows how to do this.

There is also `doxygen documentation <../doxygen/g4-tankresponse/index.html>`_.

ServiceFactories
----------------


I3IceTopResponseFactory<I3G4TankResponse>
"""""""""""""""""""""""""""""""""""""""""

The I3G4TankResponse is a specialization of an I3TankResponse. The geometry of the tank,
including the snow on top of it, is modelled accurately in Geant4. Incoming particles
are tracked by Geant4 including particle interactions in the material. Cherenkov photons
emitted in the ice are counted and distributed in time to reproduce the characteristic
waveform shape observed in data. They are not tracked through the ice to improve performance.

The number of Cherenkov photons is related to observed photo electrons using a rough
calibration with 4GeV muons. More precise calibrations can be obtained by simulating
small air showers to obtain a correction factor, which can be set in the "ChargeScale"
parameter.


Parameters
^^^^^^^^^^

**RandomServiceName**:

  Name of a random number generator service.

**CherenkovMethod**:

  Count emitted Cherenkov photons (no Cherenkov photon tracking), defaults to true.
  If set to false, the number of photo electrons will be determined based on energy
  deposit inside the tank.

**ChargeScale**:

  A charge correction factor to be used when a more precise calibration is available.
  Defaults to 1.

**TimeConstantZirco**:

  Time constant for distributing photo electrons in Zirconium lined tanks to 
  accurately model measured waveforms.

**TimeConstantTyvek**:

  Same as TimeConstantZirco, but for the Tyvek tanks.

**SafetyMargin**:

  All particles that get within this margin of a tank will be simulated. Particles 
  outside will be dropped. Tradeoff between speed and precision.

**CherenkovWeighting**:

  EXPERIMENTAL position based weighting of Cherenkov photons inside the tank. Can
  increase fluctuations (which seem to be too small in our simulations).

**VisMacro**:

  Geant4 visualization macro. You need a visualization enabled Geant4, which is not
  the case for the normal ports installation.
