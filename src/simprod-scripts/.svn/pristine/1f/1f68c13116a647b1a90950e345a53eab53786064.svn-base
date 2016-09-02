#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import tempfile
import shutil
import logging

import icecube.simprod.modules.nugen

logging.basicConfig(level=logging.INFO)

from icecube import icetray
icetray.logging.set_level('ERROR')

if __name__ == "__main__":
    tmpdir = tempfile.mkdtemp(dir=os.getcwd())
    nugen = icecube.simprod.modules.nugen.NuGen()

    # Change some default values.
    parser = nugen._opt_parser

    gcdfile = os.path.join(os.environ["I3_TESTDATA"],
                           'sim',
                           'GeoCalibDetectorStatus_2013.56429_V1.i3.gz'
                           )

    parser.defaults["nevents"] = 1000
    parser.defaults['fromenergy']=1e3
    parser.defaults['toenergy']=1e6
    parser.defaults['gcdfile']= gcdfile
    parser.defaults["outputfile"] = os.path.join(tmpdir,"nugen_nue_fullchain.i3.gz")
    parser.defaults["summaryfile"] = os.path.join(tmpdir,"nugen_nue_fullchain.xml")
    parser.defaults['mjd'] = 56429
    parser.defaults['nuflavor']='NuE'
    parser.defaults['gamma']=1.0
    parser.defaults['histogramfilename'] = "nugen_nue.pkl"
    stats = {}
    nugen.ExecuteOpts(stats)

    stats = {}
    clsim = icecube.simprod.modules.ClSim()
    parser = clsim._opt_parser
    parser.defaults['inputfilelist'] = [os.path.join(tmpdir,"nugen_nue_fullchain.i3.gz")]
    parser.defaults['gcdfile'] = os.path.expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
    parser.defaults['usegpu'] = True
    parser.defaults['runmphitfilter'] = "No"
    parser.defaults["outputfile"] = os.path.join(tmpdir,"nugen_nue_photons_fullchain.i3.gz")
    parser.defaults["summaryfile"] = os.path.join(tmpdir,"nugen_nue_photons_fullchain.xml")
    parser.defaults['histogramfilename'] = "nugen_nue_pes.pkl"
    clsim.ExecuteOpts(stats)

    stats = {}
    detsim = icecube.simprod.modules.IC86()
    parser = detsim._opt_parser
    parser.defaults['inputfile'] = os.path.join(tmpdir,"nugen_nue_photons_fullchain.i3.gz")
    parser.defaults['gcdfile'] = os.path.expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
    parser.defaults['runid'] = 12345
    parser.defaults['histogramfilename'] = "nugen_nue_trigger.pkl"
    detsim.ExecuteOpts(stats)
   
    shutil.rmtree(tmpdir)
