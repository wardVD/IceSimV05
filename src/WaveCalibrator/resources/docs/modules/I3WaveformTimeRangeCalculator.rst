
.. _I3WaveformTimeRangeCalculator:

I3WaveformTimeRangeCalculator
-----------------------------

Because waveform times are corrected for PMT transit time, the first and last 
bins of calibrated waveforms are not necessarily within the trigger readout 
window. I3WaveCalibrator calculates the earliest and latest possible waveform 
bin edge times for an event so that downstream modules don't have to guess, and 
the I3WaveformTimeRangeCalculator module implements a stand-alone version of 
the calculation that can be used e.g. for SuperDST data.

Parameters
^^^^^^^^^^

**WaveformRange**:

  Name of the I3TimeWindow to put in the frame, e.g. "CalibratedWaveformRange".

OutBoxes
^^^^^^^^
One.

Example
^^^^^^^

::
    
    from icecube import icetray, dataio, WaveCalibrator
    import I3Tray
    
    tray = I3Tray.I3Tray()
    
    tray.AddModule("I3Reader","reader",
        FilenameList=["GCD.i3.gz", "data.i3.gz"]
        )
		
	tray.AddModule("I3WaveformTimeRangeCalculator", "calcrange",
	    WaveformRange="CalibratedWaveformRange")
        
    tray.AddModule("TrashCan", "YesWeCan")
    tray.Execute()
    tray.Finish()

Determining the baseline
^^^^^^^^^^^^^^^^^^^^^^^^

The baseline that WaveCalibrator subtracts from each digitized waveform is
measured from beacon-launch waveforms. These have to be harvested
(either from
raw data or from verification histograms) and inserted into the offline
database using the Python classes provided in the `BeaconHarvester
<http://code.icecube.wisc.edu/svn/sandbox/jvansanten/BeaconHarvester>`_
project.

Adding baselines to old GCD files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

GCD files produced prior to May 2011 (and all simulation GCD files to date) do not contain information about the beacon baseline. The script WaveCalibrator/examples/tweak_gcd.py can be used to add beacon baselines to such GCD files. Baselines used for processing real data are measured from beacon launches, while those for simulation are sythesized from the existing constants to match DOMsimulator's idea of the digitizer baseline.

