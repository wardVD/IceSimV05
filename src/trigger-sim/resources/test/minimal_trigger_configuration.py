#!/usr/bin/env python

import os
from icecube import icetray, dataclasses, dataio
from icecube import trigger_sim, phys_services
from I3Tray import I3Tray
from os.path import expandvars

tray = I3Tray()

if os.environ.get("I3_TESTDATA") :
    i3_testdata = expandvars("$I3_TESTDATA") 
else :
    i3_testdata = expandvars("$I3_TESTDATA")
  
gcd_file = i3_testdata + "/sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz"

tray.AddModule("I3InfiniteSource", "source", 
               prefix = gcd_file)

tray.AddModule("SimpleMajorityTrigger","IISMT8")(
    ("TriggerConfigID",1006)
    )

tray.AddModule("SimpleMajorityTrigger","DCSMT3")(
    ("TriggerConfigID",1011)
    )

tray.AddModule("ClusterTrigger","string")    

tray.AddModule("CylinderTrigger","cyl")

tray.AddModule("SlowMonopoleTrigger","slop")

tray.AddModule("SimpleMajorityTrigger","ITSMT6")(    
    ("TriggerConfigID",102)
    )

tray.AddModule("TrashCan","trash")

tray.Execute(10)
