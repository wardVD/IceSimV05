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

    mu_gun = icecube.simprod.modules.MuonGunGenerator()
    # Change some default values.
    parser = mu_gun._opt_parser
    parser.defaults["nevents"] = 1000
    parser.defaults["outputfile"] = os.path.join(tmpdir,"muongun_fullchain.i3.gz")
    parser.defaults["summaryfile"] = os.path.join(tmpdir,"muongun_fullchain.xml")
    parser.defaults['histogramfilename'] = "muon_gun.pkl"

    stats = {}
    mu_gun.ExecuteOpts(stats)

    stats = {}
    clsim = icecube.simprod.modules.ClSim()
    parser = clsim._opt_parser
    parser.defaults['inputfilelist'] = [os.path.join(tmpdir,"muongun_fullchain.i3.gz")]
    parser.defaults['gcdfile'] = os.path.expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
    parser.defaults['usegpu'] = True
    parser.defaults['runmphitfilter'] = "No"
    parser.defaults["outputfile"] = os.path.join(tmpdir,"muongun_photons_fullchain.i3.gz")
    parser.defaults["summaryfile"] = os.path.join(tmpdir,"muongun_photons_fullchain.xml")
    parser.defaults['histogramfilename'] = "muon_gun_pes.pkl"
    clsim.ExecuteOpts(stats)

    stats = {}
    detsim = icecube.simprod.modules.IC86()
    parser = detsim._opt_parser
    parser.defaults['inputfile'] = os.path.join(tmpdir,"muongun_photons_fullchain.i3.gz")
    parser.defaults['gcdfile'] = os.path.expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
    parser.defaults['runid'] = 12345
    parser.defaults['histogramfilename'] = "muon_gun_trigger.pkl"
    detsim.ExecuteOpts(stats)

    shutil.rmtree(tmpdir)
