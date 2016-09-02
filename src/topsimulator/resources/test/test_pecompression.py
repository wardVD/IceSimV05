#!/usr/bin/env python
"""Test different PE compressions based on ParamTankResponse."""
import os
import sys
import time
import subprocess as subp

i3_testdata = os.path.expandvars("$I3_TESTDATA") 
gcd = i3_testdata + "/sim/GeoCalibDetectorStatus_IC79.55380_corrected.i3.gz"
inFile = i3_testdata + "/DAT010000"

base = os.path.basename(sys.argv[0])[:-3]

# generate test files
compressList = "012"
outFileList = []
for compress in compressList:
    outFile = "cur." + base + "-%s.i3.gz" % compress

    print "PE-compression: %(compress)s" % locals()
    cmd = ["topsimulator-simicetop", "--new", "--gcd", gcd, "--resp", "param", "--pecompress", compress,
           "-s", "1", "-r", "0", "--seed", "1", "-o", outFile, inFile]
    print " ".join(cmd)
    subp.check_call(cmd)
    outFileList.append(outFile)

# check that I3MCPulses do not depend on I3MCPE compression
from icecube.topsimulator.validationsuite import ValidationReport, Cmp, CmpI3File, CmpMap
from icecube.simclasses import I3MCPESeriesMap, I3MCPESeries

report = ValidationReport("Simulated event may not depend on PE-compression")
def CmpPESeries(rep, a, b):
    # compare npe counts per time, ignore pe source info
    from collections import defaultdict
    histA = defaultdict(lambda: 0)
    histB = defaultdict(lambda: 0)
    for x in a:
        histA[x.time] += x.npe
    for x in b:
        histB[x.time] += x.npe
    rep.result(histA == histB)
Cmp.register(I3MCPESeriesMap, CmpMap)  # don't use map.operator==
Cmp.register(I3MCPESeries, CmpPESeries)

for i in xrange(len(outFileList)):
    for j in xrange(i + 1, len(outFileList)):
        a = outFileList[i]
        b = outFileList[j]
        CmpI3File(report, a, b,
            restrictKeys=("MCTopPESeriesMap", "MCTopPESeriesMap_Pulses"))

print report

sys.exit(report == False)
