from math import *
from I3Tray import I3Units
from icecube import icetray, dataclasses
from .histogram import Histogram

class ExpressionHistogram(Histogram):
    '''
    This class makes is easy to generate histograms by taking a python
    expression that's used to calculate the value to be histogrammed.

    This expression is passed directly to the built-in function 'eval'
    where frame, I3Units, and everything in the python math library is 
    available.

    So 'log10(frame["SomeParticle"].energy/I3Units.GeV)' is a valid expression.

    Users typically should not use this class directly, but instead one
    of its derived classes like PhysicsHistogram or DAQHistogram.
    
    You have to specify several things on construction.
    - xmin : Minimum x value.    
    - xmax : Maximum x value.    
    - nbins : Number of bins.
    - name : Histogram name.
    
    - expression : This expression is passed directly to the python
        built-in fuction 'eval' which returns the value to be histogrammed.
        NB: 'frame' is available.
  
        So 'log10(frame["MyParticle"].energy/I3Units.GeV)' is perfectly valid.
    '''
    
    def __init__( self,
                  xmin,
                  xmax,
                  nbins,
                  name,
                  expression):
        Histogram.__init__(self, xmin, xmax, nbins, name)
        self.expression = expression
                
    def _Process(self, frame):
        '''
        Calculate the 'value' to be binned from the user defined expression.
        Catch all exceptions.
        '''            
        # Call eval and catch all errors
        value = 0.
        try:
            value = eval(self.expression)
        except Exception as e: 
            icetray.logging.log_debug("%s" % str(e))
            icetray.logging.log_debug("Histogram Name : %s" % self.name)
            icetray.logging.log_debug("Calling : %s" % self.expression)

        # Call fill and catch all errors
        try:
            self.fill(value)
        except Exception as e: 
            icetray.logging.log_debug("%s" % str(e))
            icetray.logging.log_debug("Histogram Name : %s" % self.name)
            icetray.logging.log_debug("Calling fill(%f, %f) " % value)


    def __getstate__(self):
        '''
        Allows Histograms to be pickled.
        '''
        state = Histogram.__getstate__(self)
        state['expression'] = self.expression
        return state
