..
.. copyright  (C) 2012
.. The Icecube Collaboration
..
.. $Id$
..
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Nathan Whitehorn <nwhitehorn@physics.wisc.edu> $LastChangedBy$

.. _corsika-reader-main:

corsika-reader
==============

.. toctree::
   :maxdepth: 3
   
   release_notes

I3CorsikaReader Module
----------------------

I3CorsikaReader provides a module to read output binary files from the CORSIKA
air-shower simulation package. Each simulated shower is emitted as a single
DAQ frame containing an I3MCTree with the shower particles, as well as an
object containing weight information and a header with the shower number.

Arguments
^^^^^^^^^

1. FilenameList (default: ``[]``)

	Array of paths to CORSIKA DAT files to read. Will be emitted in order.
	Each CORSIKA run (from the CORSIKA run configuration) will be emitted
	with a separate run ID in the frames.

2. Prefix (default: ``None``)

	Path to I3 file (e.g. GCD information) to emit before the CORSIKA
	events.

3. NEvents (default: ``0``)

	Number of CORSIKA showers per input file. This is required only on
	older CORSIKA versions that do not write this information into the
	run header. On such CORSIKA files, it will be checked at run end and
	an error emitted if incorrect. This parameter is ignored if set in
	the file.

4. CylinderHeight (default: ``1600`` m)

	Height of vertical cylinder centered at (0,0,0) in detector coordinates
	through which all simulated shower cores are made to pass.

5. CylinderRadius (default: ``800`` m)

	Radius of vertical cylinder centered at (0,0,0) in detector coordinates
	through which all simulated shower cores are made to pass.

6. ZenithBias (default: ``True``)

	Set to true if CORSIKA was compiled with the VOLUMECORR option and false
	if the VOLUMEDET option was used. The default zenith bias (proportional
	to sin(theta) for surface detectors) is not yet supported.

