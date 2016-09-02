
Expression Histograms
=====================

Creating an ExpressionHistogram is very similar to a regular histogram except 
you're passing a valid python expression to calculate the quantity to be binned.

NB: '''You should not use this class directly.''' Instead use one of the frame 
histograms, which run on the respective frames.
 
 * DAQHistogram
 * PhysicsHistogram
 * GeometryHistogram
 * CalibrationHistogram
 * DetectorStatusHistogram

What's available:

 * 'frame' - The frame is exposed.
 * I3Units - I3Units from I3Tray.
 * math - Everything from the python math library (e.g. log10).

Example:
:: 
  from icecube.production_histograms.histograms.frame_histograms import DAQHistogram
  expression = "log10(frame['SomeFrameKey'].member/I3Units.GeV)"
  h = DAQHistogram(0, 10, 10, "Log10SomeEnergy", expression)

