'''
Below are several convenience histograms that contain methods
that simply thunks to _Process.  This way a user just has to 
inherit from the correct class, configure the histograms and Bob's 
your uncle.
'''
from .expression_histogram import ExpressionHistogram
class DAQHistogram(ExpressionHistogram):
    '''
    Implements only the DAQ method.
    '''
    def DAQ(self, frame):
        self._Process(frame)

class PhysicsHistogram(ExpressionHistogram):
    '''
    Implements only the Physics method.
    '''
    def Physics(self, frame):
        self._Process(frame)

class GeometryHistogram(ExpressionHistogram):
    '''
    Implements only the Geometry method.
    '''
    def Geometry(self, frame):
        self._Process(frame)

class CalibrationHistogram(ExpressionHistogram):
    '''
    Implements only the Calibration method.
    '''
    def Calibration(self, frame):
        self._Process(frame)

class DetectorStatusHistogram(ExpressionHistogram):
    '''
    Implements only the DetectorStatus method.
    '''
    def DetectorStatus(self, frame):
        self._Process(frame)

