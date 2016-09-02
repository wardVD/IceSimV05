#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import shutil
import tempfile
import logging

import icecube.simprod.modules

logging.basicConfig(level=logging.INFO)

if __name__ == "__main__":
    tmpdir = tempfile.mkdtemp(dir=os.getcwd())

    corsika = icecube.simprod.modules.Corsika5ComponentGenerator()

    # Change some default values.
    parser = corsika._opt_parser
    parser.defaults["nevents"] = 10000
    parser.defaults["outputfile"] = os.path.join(tmpdir,"corsika_5comp_fullchain.i3.gz")
    parser.defaults["summaryfile"] = os.path.join(tmpdir,"corsika_5comp_fullchain.xml")
    parser.defaults["compress"] = False
    parser.defaults["gcdfile"] = os.path.expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
    parser.defaults["histogramfilename"] = "corsika_5comp.pkl"

    stats = {}
    corsika.ExecuteOpts(stats)

    stats = {}
    clsim = icecube.simprod.modules.ClSim()
    parser = clsim._opt_parser
    parser.defaults['inputfilelist'] = [os.path.join(tmpdir,"corsika_5comp_fullchain.i3.gz")]
    parser.defaults['gcdfile'] = os.path.expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
    parser.defaults['usegpu'] = True
    parser.defaults["outputfile"] = os.path.join(tmpdir,"corsika_5comp_photons_fullchain.i3.gz")
    parser.defaults["summaryfile"] = os.path.join(tmpdir,"corsika_5comp_photons_fullchain.xml")
    parser.defaults['histogramfilename'] = "corsika_5comp_pes.pkl"
    clsim.ExecuteOpts(stats)

    stats = {}
    detsim = icecube.simprod.modules.IC86()
    parser = detsim._opt_parser
    parser.defaults['inputfile'] = os.path.join(tmpdir,"corsika_5comp_photons_fullchain.i3.gz")
    parser.defaults['gcdfile'] = os.path.expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
    parser.defaults['runid'] = 12345
    parser.defaults['histogramfilename'] = "corsika_5comp_trigger.pkl"
    detsim.ExecuteOpts(stats)

    shutil.rmtree(tmpdir)
