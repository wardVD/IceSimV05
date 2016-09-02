#!/usr/bin/env python

from icecube import icetray, dataio, dataclasses
from os.path import expandvars
f = dataio.I3File(expandvars("$I3_TESTDATA/nugen_numu_ic80_dc6.002488.000000.processed.i3.gz"))

while f.more():
    fr = f.pop_physics()

    launchmap = fr.Get("InIceRawData")
    
    # trying the old and new style of pybindings to the waveforms
    # whose serialization changed from I3Vector to std::vector<int>
    # this makes it easier to use this same script in old and new
    # releases and make sure all is well
    for k,v in launchmap :
        print(k)
        for l in v :
            try :
                # these are the new pybindings
                print(4*" " + "RawFADC")
                print(8*" " + str(l.raw_fadc))
                print(4*" " + "RawChargeStamp")
                print(8*" " + str(l.raw_charge_stamp))
                print(4*" " + "RawATWDs")
                for n,a in enumerate(l.raw_atwd):
                    print(8*" "  + "[%d]" % n + str(a))
            except AttributeError :
                # these are the old ones
                print(4*" " + "RawFADC")
                print(8*" " + str([ i for i in l.GetRawFADC() ]))
                print(4*" " + "RawChargeStamp")
                print(8*" " + str([ i for i in l.GetRawChargeStamp()]))
                print(4*" " + "RawATWDs")
                for i in range(4):
                    print(8*" " + str([ j for j in l.GetRawATWD(i)] ))
                        
