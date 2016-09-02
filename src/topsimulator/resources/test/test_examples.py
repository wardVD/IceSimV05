#!/usr/bin/env python
import os
import sys
import unittest
from icecube.icetray import I3Test
from util import have_g4tankresponse

# skipUnless doesn't exist in Python2.6, skip test
if sys.version_info < (2, 7):
    raise SystemExit


class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "topsimulator"

    def test_sim_icetop_param(self):
        self.sim_icetop("param")

    def test_sim_single_particles_param(self):
        self.sim_single_particles("param")

    @unittest.skipUnless(have_g4tankresponse, "needs g4-tankresponse")
    def test_sim_icetop_param(self):
        self.sim_icetop("g4")

    @unittest.skipUnless(have_g4tankresponse, "needs g4-tankresponse")
    def test_sim_single_particles_g4(self):
        self.sim_single_particles("g4")

    def sim_single_particles(self, resp):
        gcd = self.I3_TESTDATA + "/sim/GeoCalibDetectorStatus_IC79.55380_corrected.i3.gz"
        self.run_example(
            "sim-single-particles.py",
            "--gcd", gcd,
            "-o", "sim_single_particle_test.i3",
            "-n", "1",
            "--tanks", "36A",
            "--resp", resp
        )
        os.unlink("sim_single_particle_test.i3")

    def sim_icetop(self, resp):
        gcd = self.I3_TESTDATA + "/sim/GeoCalibDetectorStatus_IC79.55380_corrected.i3.gz"
        corsika = self.I3_TESTDATA + "/DAT010000"
        self.run_example(
            "sim-icetop.py",
            "--gcd", gcd,
            "-o", "sim_icetop_test.i3",
            "-n", "1",
            "--resp", resp,
            corsika
        )
        os.unlink("sim_icetop_test.i3")

unittest.main()
