import os
import sys

from icecube import icetray
from icecube.icetray import I3Module
from icecube import dataclasses as dc
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms.histogram_modules.histogram_module import HistogramModule

if sys.version_info[0] >= 3:
    import pickle
else:
    import cPickle as pickle

class ProductionHistogramModule(I3Module) :
    '''
    This I3Module passes DAQ or Physics frames to histograms
    loaded in the user-defined configuration.  The configuration
    object is a thin class that manages a dictionary of histograms
    where the key is the histogram name.
    '''
    def __init__( self, context ):
        super(ProductionHistogramModule, self).__init__(context)

        self.AddParameter("Histograms", "List of histograms or modules to run.", list() )
        self.AddParameter("OutputFilename", "Name of output pickle file.", "output.pickle" )
        self.AddParameter("Prescales", "Dictionary of frame type to prescale",  
                          {"Geometry" : 1,
                           "Calibration": 1,
                           "DetectorStatus": 1,
                           "DAQ": 1,
                           "Physics": 1,
                           })

        self.AddOutBox("OutBox")
        self._frame_counter = 0

    def Configure(self):
        self.outfilename = self.GetParameter("OutputFilename")
        self.prescales = self.GetParameter("Prescales")        
        histograms = self.GetParameter("Histograms")
        
        # This allows the user to pass either classes or instances
        # downstream code will expect an instance, so make one if it's
        # not an instance of Histogram or HistogramModule
        self.histograms = list()
        for h in histograms:
            try:
                if issubclass(h, (Histogram, HistogramModule)):
                    self.histograms.append(h())
            except TypeError:
                try:
                    if isinstance(h, (Histogram, HistogramModule)):
                        self.histograms.append(h)
                except TypeError:
                    icetray.logging.log_error("Passed an invalid object to Histograms")
                    icetray.logging.log_fatal("%s is neither a Histogram or HistogramModule" % str(h))
                                                                         
        # initialize the frame counters
        self._frame_counters = dict()
        for key in self.prescales.keys():
            self._frame_counters[key] = 0

    def _Process(self, frame):
        '''
        This method is called by all IceTray methods and only need one, 
        and I don't want to repeat a bunch of code.  DRY.
        '''
        stream_key = str(frame.Stop)
        self._frame_counters[stream_key] += 1
        if self._frame_counters[stream_key] % self.prescales[stream_key] != 0 :
            self.PushFrame(frame)
            return        

        for histogram in self.histograms :
            getattr(histogram, str(frame.Stop))(frame)

        self.PushFrame(frame)

    def Geometry(self, frame):
        self._Process(frame)

    def Calibration(self, frame):
        self._Process(frame)

    def DetectorStatus(self, frame):
        self._Process(frame)
        
    def Physics(self, frame):
        self._Process(frame)
        
    def DAQ(self, frame):
        self._Process(frame)
        
    def Finish(self):
        '''
        Write the histograms to an output pickle file.
        The output is a dictionary of histograms where
        the key is the name of the histogram.  This just
        makes access nicer.
        '''
        if self.outfilename :
            histogram_dict = dict()
            for histogram in self.histograms :
                if isinstance(histogram, Histogram):
                    histogram_dict[histogram.name] = histogram
                if isinstance(histogram, HistogramModule):
                    histogram_dict.update(histogram.histograms)
                
            pickle.dump(histogram_dict, 
                        open( self.outfilename, "w"), 
                        pickle.HIGHEST_PROTOCOL)
                          


