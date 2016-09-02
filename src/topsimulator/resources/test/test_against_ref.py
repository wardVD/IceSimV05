#!/usr/bin/env python
"""
Tests output of simulation segment against a reference file.

Call with argument "makeref" to generate new reference files.
"""
import os
import sys
import subprocess as subp
from util import have_g4tankresponse

i3_testdata = os.path.expandvars("$I3_TESTDATA")

gcd = i3_testdata + "/sim/GeoCalibDetectorStatus_IC79.55380_corrected.i3.gz"
inFile = i3_testdata + "/DAT010000"

# generate test files for both tank responses
filesToCompareList = []
for resp in ("param", "g4"):
    if resp == "g4" and not have_g4tankresponse:
        continue
    print("Response: " + resp)

    # if reference does not exist, create it instead of comparing
    base = os.path.basename(sys.argv[0])[:-3]
    common = base + "-%s.i3.gz" % resp
    refFile = i3_testdata + "/topsimulator/ref." + common
    outFile = "cur." + common

    if not os.path.exists(outFile):
        cmd = ["topsimulator-simicetop", "--new", "--gcd", gcd, "--resp", resp, "--pecompress",  "1",
               "-s", "1", "-r", "0", "--seed", "1", "-o", outFile, inFile]
        print(" ".join(cmd))
        subp.check_call(cmd)

    filesToCompareList.append((outFile, refFile))

if len(sys.argv) == 2 and sys.argv[1] == "makeref":
    # copy newly-created files to the refence location
    import shutil
    for pair in filesToCompareList:
        outFile, refFile = pair
        print("Writing " + refFile)
        shutil.copy(outFile, refFile)
    raise SystemExit
else:
    # prepare comparison
    from icecube.topsimulator.validationsuite import ValidationReport, Cmp, \
      CmpMap, CmpI3Tree, CmpI3File, CmpFields, CmpSeries


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

    def CmpMCHit(rep, a, b):
        # ignore ParticleID
        return CmpFields(rep, a, b,
                         "charge", "cherenkov_distance", "hit_id", "hit_source",
                         "npe", "time")

    from icecube import simclasses, dataclasses
    Cmp.register(simclasses.I3MCPESeries, CmpPESeries)
    Cmp.register(dataclasses.I3MCTree, CmpI3Tree)
    Cmp.register(dataclasses.I3MCHit, CmpMCHit)

    # check that output is identical to reference
    report = ValidationReport("Comparing current simulation with reference")

    for cur, ref in filesToCompareList:
        if ref is None:  # we ran in reference generation mode
            continue

        CmpI3File(report, cur, ref,
                  ignoreKeys=(
                    "MCTopPESeriesMap_PulsesParticleIDMap", # ParticleIDs change all the time
                  ),
                  strictOnLayout=False)

    if len(report) == 0:
        print("Reference files are missing.")
        sys.exit(1)
    else:
        print(report)
        sys.exit(report == False)
