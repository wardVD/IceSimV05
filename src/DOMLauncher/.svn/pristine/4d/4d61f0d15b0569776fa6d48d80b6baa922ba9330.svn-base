.. _DOMLauncher:

DOMLauncher Project
===================

This project implements low-level simulation of the IceCube detector, including the behavior of the photomutliplier tube and the DOM mainboard.

Modules:

* PMTResponseSimulator

  This module simulates the behaviour of a PMT. It reads I3MCPE, representing the number of photoelectrons 
  ejected from the photocathode, and produces I3MCPulses with real valued charges, still in units of 
  photoelectrons but representing the data after passage through the PMT.
  For a detailed description of the physics processes simulated, see :ref:`PMTResponseSimulator`.

* DOMLauncher

  This module simulates the behavior of the DOM mainbaord, including launching and digitization

Usage:

  Basic usage is easy: assuming you have imported the DOMLauncher bindings, to add dector simulation to your I3Tray just do::

        tray.AddSegment(DOMLauncher.DetectorResponse,"Detector_Simulation")

Table of Contents:

.. toctree::
   :maxdepth: 3

   release_notes
   PMTRes
   DOML
