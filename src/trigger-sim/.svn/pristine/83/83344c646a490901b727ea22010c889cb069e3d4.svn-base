#!/usr/bin/env python

'''
This example shows how to add a new trigger status to a GCD file.
This is useful if you want to change the trigger setting for one
of the trigger modules.

This is important because the trigger modules aren't capable of
changing the threshold, for example, via IceTray parameters.  They
get their configuration from the GCD file.

This script takes a GCD file and adds the TriggerStatus to the D frame.
'''

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio
from os.path import expandvars

# make a trigger key
tkey = dataclasses.TriggerKey()
tkey.source = dataclasses.I3Trigger.IN_ICE
tkey.type = dataclasses.I3Trigger.SIMPLE_MULTIPLICITY
tkey.config_id = 999

# make a trigger status
tstat = dataclasses.I3TriggerStatus()

# set the triggername
tstat.trigger_name = "MyNewTrigger"
tstat.trigger_settings["threshold"] = '3'
tstat.trigger_settings["timeWindow"] = '2500'
tstat.trigger_settings["domSet"] = '5'

# make a readout config
roc = dataclasses.I3TriggerStatus.I3TriggerReadoutConfig()
roc.readout_time_minus = 10000
roc.readout_time_plus = 10000
roc.readout_time_offset = 0

# add the readout config to the trigger status
tstat.readout_settings[dataclasses.I3TriggerStatus.ALL] = roc

f = dataio.I3File(expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC59.55000_candidate.i3.gz"))

from icecube.sim_services.sim_utils import gcd_utils

ts = gcd_utils.get_triggerstatus(f)
ts[tkey] = tstat

# it's important that you reopen the file
# because it seems the I3File.rewind doesn't
# do what I thought it should do
f = dataio.I3File(expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC59.55000_candidate.i3.gz"))
newgcd = gcd_utils.put_triggerstatus(ts,f,"./newGCD.i3.gz")
newgcd.close()
