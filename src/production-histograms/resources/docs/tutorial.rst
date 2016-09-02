Tutorial
========

Creating a Histogram
....................

The easiest way to create a histogram is to use one of the provide convenience 
histograms, like DAQHistogram or PhysicsHistograms.  These inherit from 
ExpressionHistogram and run on DAQ and Physics frame, respectively.

::

 from icecube.production_histograms.histograms.frame_histograms import PhysicsHistogram
 histogram = PhysicsHistogram(0., 10., 100, "LogParticleEnergy", "log10(frame['ParticleKey'].energy/I3Units.GeV)")

To run this on an I3File try something like : 

::

 #!/usr/bin/env python
 from I3Tray import I3Tray
 from icecube import icetray, dataio
 from icecube.production_histograms import ProductionHistogramModule
 
 from icecube.production_histograms.histograms.frame_histograms import PhysicsHistogram
 histogram = PhysicsHistogram(0., 10., 100, "LogParticleEnergy", "log10(frame['ParticleKey'].energy/I3Units.GeV)")
 
 tray = I3Tray()
 
 tray.Add("I3Reader", FilenameList = filelist)
 tray.Add(ProductionHistogramModule, Histograms = [histogram])
 tray.Execute()
 tray.Finish()

This should generate a pickle file named 'output.pkl' that contains a dictionary
where the keys are histogram names and the entries are the histograms themselves.

::

 In [1]: import cPickle as pickle
 In [2]: histogram_dict = pickle.load(open('output.pkl'))
 In [3]: histogram_dict.keys()
 Out[3]: 
 ['LogParticleEnergy']
 In [4]: particle_energy = histogram_dict["LogParticleEnergy"]
 In [5]: particle_energy.nan_count
 Out[5]: 0
 In [6]: sum(particle_energy.bin_values)
 Out[6]: 7838

At this point you have full access to the histogram, so feel free to use whatever 
tool you want to render it.
