..
.. copyright  (C) 2011
.. The Icecube Collaboration
..
.. $Id$
..
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Nathan Whitehorn <nwhitehorn@physics.wisc.edu> $LastChangedBy$

.. _wavedeform-main:

wavedeform
=================

WaveDeform is a linear algebra-based algorithm for deconvolving the DOM
response to photons. Using a non-negative linear least squares algorithm
(Lawson and Hanson, 1973), it deterministically deconvolves the SPE response
function from the waveforms recorded by the DOM's FADC and ATWD digitizers.
Unlike other algorithms, it deconvolves the response from both digitizers
simultaneously, improving the accuracy of the fit and preventing edge effects
at the boundary of the ATWD.

.. toctree::
   :titlesonly:
   
   release_notes

Arguments
^^^^^^^^^^^^^

1. Waveforms (default: CalibratedWaveforms)
	Name of the input waveform series map, containing waveforms from all
	digitizer channels (including SLC hits).

2. Output (default: WavedeformPulses)
	Name of output pulse series map.

3. SPEsPerBin (default: 4)
	This controls the number of basis functions to place in the range of
	each waveform. For example, if SPEsPerBin is set to 3, the ATWD will be
	unfolded with pulses spaced 1.1 ns apart, comparable to the PMT
	transit-time spread. The greater the number of SPEs per bin, the
	greater the time resolution, until the fundamental limit is reached
	around SPEsPerBin of 5. 

	The time required by the NNLS algorithm is (very roughly) proportional
	to SPEs^3. Thus, the speed of the algorithm can be increased
	substantially at the price of accuracy by reducing the number of
	unfolded SPEs, and vice versa.

4. Tolerance (default: 9)
	This controls the stopping tolerance of the algorithm, in the amplitude
	of the gradient of the least-squares residual. When the improvement per
	PE of allowing one more non-zero pulse drops below this number, the
	algorithm will terminate. Larger numbers will produce a faster, coarser
	fit using fewer pulses, and smaller numbers will provide a slower,
	better fit with more.

6. NoiseThreshold (default: 2)
	Consider waveform bins that are fewer than this many ADC counts from
	baseline as noise.  Set the bin values to 0 and reduce their weight
	by a factor of 4.

7. BasisThreshold (default: 3)
	When building the array of basis functions, require a waveform bin with
	amplitude at least this number of ADC counts above the baseline within
	the full-width-half-maximum of the basis function in order to include
	in in the unfolding matrix.
	
8. WaveformTimeRange (default: "CalibratedWaveformRange")
	Name of maximum time range of calibrated waveforms for this event

9. DeweightFADC (default: true)
	Reduce the weight of FADC data during the initial ATWD window if we have
	ATWD data.

10. ApplySPECorrections (default: false)
	Scale the unfolded charges using fits to the SPE charge spectrum
	provided by I3DOMCalibration

Code Review 
^^^^^^^^^^^ 
.. toctree:: 
   :titlesonly: 

   code_review
