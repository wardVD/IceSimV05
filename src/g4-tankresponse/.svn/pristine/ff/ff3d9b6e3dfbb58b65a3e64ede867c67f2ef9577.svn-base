#!/usr/bin/env python
import os
import unittest
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "g4-tankresponse"

    def test_muon_injection(self):
        for ch in "-+":
            args = "-p mu{0} -n 5 -k 2".format(ch).split()
            self.run_example("simple_injection.py", *args)

    def test_electron_injection(self):
        for ch in "-+":
            args = "-p e{0}".format(ch).split()
            self.run_example("simple_injection.py", *args)

    def test_photon_injection(self):
        args = "-p gamma".split()
        self.run_example("simple_injection.py", *args)

    def test_proton_injection(self):
        for ch in "-+":
            args = "-p p{0}".format(ch).split()
            self.run_example("simple_injection.py", *args)

    def tearDown(self):
        '''Clean up the mess.'''
        os.unlink("g4-tankresponse-test.i3")

unittest.main()
