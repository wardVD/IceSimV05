
.. _I3WaveformSplitter:

I3WaveformSplitter
------------------

The I3WaveformSplitter module splits a given I3WaveformSeriesMap into multiple maps, one for each waveform source.

Parameters
^^^^^^^^^^

**Input**:

  Name of the I3WaveformSeriesMap to get from the frame, e.g. "CalibratedWaveforms".
  
**HLC_ATWD**:

  Name of the I3WaveformSeriesMap containing the ATWD waveforms from HLC launches
  to put in the frame, e.g. "CalibratedATWD".
  
**HLC_FADC**:

  Name of the I3WaveformSeriesMap containing the FADC waveforms from HLC launches
  to put in the frame, e.g. "CalibratedFADC_HLC".
  
**SLC**:

  Name of the I3WaveformSeriesMap containing the FADC charge stamps from SLC launches
  to put in the frame, e.g. "CalibratedFADC_SLC". Note that InIce-style SLC stamps are
  indistinguishable from IceTop-style SLC stamps and will appear in the same map.
  
**Force**:

  Place output maps in the frame even if they're empty. This can be useful if downstream
  modules are lazy and assume their inputs are always present.

**PickUnsaturatedATWD**:
  
  Emit only the highest-gain unsaturated ATWD channel. This emulates WaveCalibrator's
  former CALIBRATE_UNSATURATED mode.
  
**SplitATWDChannels**:

  Split waveforms from the three ATWD channels into separate maps numbered 0, 1, and 2.
  
**SplitATWDChips**:

  Split waveforms from the two ATWD chips into separate maps, marked _Chip0 and _Chip1.

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
        
    tray.AddModule("I3WaveformSplitter", "splitter",
        Input="CalibratedWaveforms",
        ATWD_HLC="CalibratedATWD",
        FADC_HLC="CalibratedFADC_HLC",
        SLC="CalibratedFADC_SLC",
        PickUnsaturatedATWD=True,
        )
    
    tray.AddModule("TrashCan", "YesWeCan")
    tray.Execute()
    tray.Finish()
