#!/usr/bin/env python

import os
try:
    import unittest2 as unittest
except ImportError:
    import unittest

from icecube import icetray
from icecube.production_histograms.histograms.histogram import Histogram
from icecube.production_histograms.histogram_modules.histogram_module import HistogramModule

class DilbertRNGHistogram(Histogram):
    def __init__(self):
        Histogram.__init__(self, 0, 10, 10, "DilbertRNG")


class DerivedHistogramModule(HistogramModule):
    '''Derived class to test the Histogram base class'''
    def __init__(self):
        HistogramModule.__init__(self)
        # append a Histogram instance
        self.append(Histogram(0,10,10,"TestAppendInstance"))
        # append a Histogram class
        self.append(DilbertRNGHistogram)

    def DAQ(self, frame):
        self.histograms["DilbertRNG"].fill(9)
        self.histograms["TestAppendInstance"].fill(1)
        
class TestHistogram(unittest.TestCase):

    def test_fill(self):
        hm = DerivedHistogramModule()

        for i in range(10):            
            hm.DAQ(icetray.I3Frame())

        h = hm.histograms["DilbertRNG"]
        self.assertEqual(h.bin_values[9], 10)
        self.assertEqual(len(h.bin_values), 10)        
                            
    def test_throw(self):
        # appending a object that's not an instance
        # or a class object of Histogram throws
        class foo(object):
            pass

        dh = DerivedHistogramModule()
        self.assertRaises(RuntimeError, dh.append, foo)

unittest.main()
