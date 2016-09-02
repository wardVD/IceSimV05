#!/usr/bin/env python

import unittest

from icecube import icetray
from icecube.production_histograms.histograms.expression_histogram import ExpressionHistogram

class TestHistogram(unittest.TestCase):

    def test_default(self):
        h = ExpressionHistogram(0,10,10,"DerivedHistogram", expression = "frame['integer'].value")

        for i in range(10):            
            frame = icetray.I3Frame()
            frame["integer"] = icetray.I3Int(i)
            h._Process(frame)

        for bin_value in h.bin_values :
            self.assertEqual(bin_value, 1)        
    
unittest.main()
