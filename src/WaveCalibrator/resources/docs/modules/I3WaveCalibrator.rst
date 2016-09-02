
I3WaveCalibrator
----------------

Parameters
^^^^^^^^^^

**Launches**:

  Name of the I3DOMLaunchSeriesMap to get from the frame, e.g. "InIceRawData".

**Waveforms**:

  Name of the I3WaveformSeriesMap to put in the frame. This map contains all
  ATWD/FADC/SLC waveforms obtained from the input; if desired, they can be split
  out into separate maps using :ref:`I3WaveformSplitter`.
  
**Errata**:

  Name of the std::vector<OMKey> to put in the frame, containing the OMs for which
  the calibration failed in one or more waveforms, and should be considered
  unreliable.

**ATWDSaturationMargin**:

  Saturation threshold of the ATWD, in ADC counts below the maximum value; e.g. a
  saturation margin of 123 means that ATWDs with any bins above 1023-123=900
  counts will be considered saturated.
  
**FADCSaturationMargin**:

  Saturation threshold of the FADC, in ADC counts below the maximum value.

.. note::
	The ATWDMode parameter is no longer supported. I3WaveCalibrator will always
	calibrate all channels present in each DOMLaunch. If you need to have only
	the highest-gain unsaturated channel it can be split out using
	:ref:`I3WaveformSplitter`.

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
        Filename="data.i3.gz"
        )
    
    tray.AddModule("I3WaveCalibrator", "sedan",
        Launches="InIceRawData",
        Waveforms="CalibratedWaveforms",
        Errata="BorkedOMs",
        ATWDSaturationMargin=123,
        FADCSaturationMargin=0,
        )
    
    tray.AddModule("TrashCan", "YesWeCan")
    tray.Execute()
    tray.Finish()

