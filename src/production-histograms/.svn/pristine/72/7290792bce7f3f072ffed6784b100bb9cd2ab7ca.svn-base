#!/usr/bin/env python

import os
import unittest

from icecube import icetray
from icecube.production_histograms.histograms.histogram import Histogram

class DerivedHistogram(Histogram):
    '''Derived class to test the Histogram base class'''
    def DAQ(self, frame):
        self.fill(frame["integer"].value)    
        
class TestHistogram(unittest.TestCase):

    def test_fill(self):
        h = DerivedHistogram(0,10,10,"DerivedHistogram")

        for i in range(10):            
            frame = icetray.I3Frame()
            frame["integer"] = icetray.I3Int(i)
            h.DAQ(frame)

        frame = icetray.I3Frame()
        frame["integer"] = icetray.I3Int(-1)
        h.DAQ(frame)
        
        frame = icetray.I3Frame()
        frame["integer"] = icetray.I3Int(10)
        h.DAQ(frame)        

        self.assertEqual(len(h.bin_values), 10)
        self.assertEqual(h.name, "DerivedHistogram")
        
        for bin_value in h.bin_values :
            self.assertEqual(bin_value, 1)        

        self.assertEqual(h.overflow, 1)
        self.assertEqual(h.underflow, 1)
                            
    def test_pickle(self):
        import cPickle as pickle
        
        h = DerivedHistogram(0,10,10,"DerivedHistogram")

        f = open("tmp.pickle", "w")
        pickle.dump(h, f)
        f.close()

        f = open("tmp.pickle")
        h_test = pickle.load(f)
        
        os.remove("tmp.pickle")

        self.assertEqual(h.xmin, h_test.xmin) 
        self.assertEqual(h.xmax, h_test.xmax) 
        self.assertEqual(h.name, h_test.name) 
        self.assertEqual(h.overflow, h_test.overflow) 
        self.assertEqual(h.underflow, h_test.underflow) 
                
unittest.main()
