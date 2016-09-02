#!/usr/bin/env python
"""Ensure that MuonGunGenerator is working"""

import os
import tempfile
import shutil

from icecube.simprod.modules import MuonGunGenerator 

from icecube import icetray, dataclasses, dataio
from I3Tray import I3Tray

try:
    tmpdir = tempfile.mkdtemp(dir=os.getcwd())
    tmpfile = os.path.join(tmpdir,'test.i3')
    summaryfile = os.path.join(tmpdir,'summary.xml')
    gcdfile = os.path.expandvars('$I3_TESTDATA/sim/GCD.i3.gz')
    
    # make a very small muon gun file
    n = MuonGunGenerator()
    n.SetParameter('outputfile',tmpfile)
    n.SetParameter('summaryfile',summaryfile)
    n.SetParameter('gcdfile',gcdfile)
    n.SetParameter('gamma',2)
    n.SetParameter('length',1600)
    n.SetParameter('radius',800)
    n.SetParameter('model','Hoerandel5_atmod12_SIBYLL')
    if n.Execute({}) != 0:
        raise Exception('MuonGunGenerator did not return OK')

    # now check generated file
    tray = I3Tray()
    tray.Add('I3Reader', filename=tmpfile)
    def checky(frame):
        assert('I3MCTree' in frame)
        #print len(frame['I3MCTree'])
    tray.Add(checky, Streams=[icetray.I3Frame.DAQ])
    tray.Execute()
    tray.Finish()
finally:
    shutil.rmtree(tmpdir)
