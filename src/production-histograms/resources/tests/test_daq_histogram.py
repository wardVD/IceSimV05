#!/usr/bin/env python

import unittest

from icecube import icetray
from icecube.production_histograms.histograms.frame_histograms import DAQHistogram

class TestHistogram(unittest.TestCase):

    def test_default(self):
        h = DAQHistogram(0,10,10,"DerivedHistogram", expression = "frame['integer'].value")

        for i in range(10):            
            frame = icetray.I3Frame()
            frame["integer"] = icetray.I3Int(i)
            h.DAQ(frame)

        for bin_value in h.bin_values :
            self.assertEqual(bin_value, 1)        
    
    def test_physics(self):
        '''
        This tests that nothing happens when the Physics
        method is called.
        '''
        h = DAQHistogram(0,10,10,"DerivedHistogram", expression = "frame['integer'].value")

        for i in range(10):            
            frame = icetray.I3Frame()
            frame["integer"] = icetray.I3Int(i)
            h.Physics(frame)

        # all the bins should still be empty
        for bin_value in h.bin_values :
            self.assertEqual(bin_value, 0)
            
unittest.main()
