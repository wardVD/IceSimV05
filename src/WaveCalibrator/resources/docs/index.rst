.. _WaveCalibrator-main:

WaveCalibrator
============================

Calibrate DOMLaunches into waveforms, correcting for known issues
in the electronics.  This project is maintained by Jim Braun
<jbraun@icecube.wisc.edu>.

.. toctree::
   :maxdepth: 1
   
   release_notes
   
About
^^^^^

I3WaveCalibrator is a module that applies calibration constants to transform the contents
of raw DOMLaunches (ADC counts) into calibrated waveforms (mV), while correcting
for known effects of the electronics. It is a reimplementation of an earlier
module called I3DOMcalibrator.

The following transformations are applied to the digitized waveform to obtain the final calibrated output:

1. A *baseline* measured from beacon-launch data is subtracted from the ADC
   counts.
2. The baseline-subtracted ADC counts are multiplied by the bin-specific
   gain to obtain a voltage.
3. The PMT high voltage setting is used to correct the
   start time of the waveform for the transit time, the average time it takes a pulse
   to propagate through the entire PMT. Waveforms from the second ATWD chip and the
   FADC are corrected for their delay with respect to the first ATWD chip.
4. Finally, all waveforms are corrected for the effects of droop in the transformer
   that couples the mainboard to the PMT output. This is done by calculating the
   expected reaction voltage from the toroid at each time, and adding the reaction
   voltage to the calibrated waveform to compensate. The reaction voltages are made
   to decay exponentially according to a temperature-dependent model of the
   transformer's behavior. When a readout contains consecutive launches from the same
   DOM, the reaction voltages at the end of the last launch are used to correct for
   the residual droop in the follow-on launch.

There are a few key differences between I3WaveCalibrator and its predecessor, I3DOMcalibrator:

1. I3WaveCalibrator uses the baseline measured in data-taking mode from beacon launches
   for voltage calibration. This is more reliable than the value measured by DOMCal
   and used by DOMcalibrator for voltage calibration.

2. I3WaveCalibrator keeps better records of what it's done. Every waveform is tagged with
   the chip and channel it came from, so that the transformation can be inverted or
   checked for sanity by downstream modules.

3. I3WaveCalibrator outputs a single I3WaveformSeriesMap containing all the calibrated waveforms
   from its input. If separate maps for each digitizer/channel are needed, they can
   be extracted from the output with :ref:`I3WaveformSplitter`.

4. I3WaveCalibrator applies droop correction to all waveforms from a single readout in
   one pass, so that the residual droop in follow-on launches can be corrected as
   well. As such, no launch cleaning should be applied to the DAQ payload before
   calibration. If waveforms need to be separated by digitizer/chip or LC condition
   for later processing, this can be done using :ref:`I3WaveformSplitter`.

5. I3WaveCalibrator takes a different approach to droop correction for undershot or
   saturated waveforms. Instead of assuming a downward-ramping input voltage when the
   digitizer is floored at 0 counts, it assumes zero input voltage and allows the
   reaction voltages to decay until the end of the undershot region, where it resumes
   normal droop correction. When the FADC saturates within the ATWD window, the
   measured ATWD waveform is used to reconstruct what the FADC would have measured
   and calculate the appropriate droop correction. The same is done for high-gain ATWD
   channels (0 and 1) when they saturate. In the extremely rare cases where the
   FADC saturates outside the ATWD window or ATWD channel 2 saturates, all bets
   are off, and WaveCalibrator marks the waveform series for that DOM as unrecoverable.

Table of Contents:

.. toctree::
   :maxdepth: 3
   :glob:

   modules/*

