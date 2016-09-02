from icecube import icetray
from icecube.production_histograms.histograms.histogram import Histogram
class HistogramModule(object):
    '''
    This is a base class for a collection of histograms.    
    Like an I3Module the user should override one of the
    'Stop' methods, like DAQ or Physics.
    '''
    def __init__(self):
        self.histograms = dict()
    
    def append(self, obj):
        '''
        Appends histograms.  If a class object is passed,
        and instance is created on the fly.
        '''
        try:
            if issubclass(obj, Histogram):
                # A class object is passed, so we need to create
                # an instance and add that to 'histograms'
                print("ok...it's a class object.")
                self.histograms[obj().name] = obj()
            else:
                icetray.logging.log_error("Passed an invalid class.")
                icetray.logging.log_fatal("%s is not a Histogram" % str(obj))
        except TypeError:
            # at this point we know it must be an instance
            if isinstance(obj, Histogram):
                # An instance was passed so it can be added directly
                # to the 'histograms' dictionary
                self.histograms[obj.name] = obj
            else:
                icetray.logging.log_error("Passed an invalid class.")
                icetray.logging.log_fatal("%s is not a Histogram" % str(obj))

    def Geometry(self, frame):
        pass

    def Calibration(self, frame):
        pass

    def DetectorStatus(self, frame):
        pass
        
    def Physics(self, frame):
        pass
        
    def DAQ(self, frame):
        pass

