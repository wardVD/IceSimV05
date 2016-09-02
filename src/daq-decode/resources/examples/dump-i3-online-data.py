#!/usr/bin/env python

from I3Tray import *

from os.path import expandvars

from icecube import icetray 
from icecube import dataclasses 
from icecube import phys_services 
from icecube import payload_parsing 
from icecube import dataio 
from icecube import daq_decode 


input = sys.argv[1:]

if not input:
    testdata = expandvars("$I3_TESTDATA")
    input = [testdata + "/exp/IceCube/2015/PFFilt_PhysicsFiltering_Run00126598_Subrun00000000_00000028_slim.i3.gz"]

for i in input:
    print("will read in file ... %s" % i)

workspace = expandvars("$I3_SRC")

mb_id_file = workspace + "/phys-services/resources/mainboard_ids.xml.gz"


tray = I3Tray()


tray.AddService("I3XMLOMKey2MBIDFactory","omkey2mbid")(
   ("Infile",mb_id_file),
   )

tray.AddService("I3PayloadParsingEventDecoderFactory","decode")(
   ("MinBiasID", "MinBias"),
   ("FlasherDataID", "Flasher"),
   ("CPUDataID", "BeaconHits"),
   ("SpecialDataID", "SpecialHits"),
   ("SpecialDataOMs", [OMKey(0,1),   # 2016, IceAct
                       OMKey(12,65), # 2016, Scintillator
                       OMKey(12,66), # 2016, Scintillator
                       OMKey(62,65), # 2016, Scintillator
                       OMKey(62,66), # 2016, scintillator
                       OMKey(0,91),  # TWR... long time ago
                       OMKey(0,92)]),# TWR... long time ago
   )


tray.AddModule("I3Reader","reader")(
   ("FilenameList",input),
   )

tray.AddModule("QConverter", "qify", WritePFrame=False)

# Note: this may replace some frame objects such as
#       the event header (I3EventHeader)! however,
#       the original frame object is still included
#       but with a new name (cf. I3EventHeader_orig) 
tray.AddModule("I3FrameBufferDecode","decode")(
   ("BufferID","I3DAQDataTrimmed"),
   )

tray.AddModule("Dump","dump")

tray.AddModule("TrashCan","trash")


tray.Execute()
tray.Finish()

