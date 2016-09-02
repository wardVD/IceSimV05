from math import isnan
from icecube import dataclasses

def trigger_sim_test(trigger_status_map):
    for trigger_key, trigger_status in trigger_status_map :
        # expect No AMANDA triggers
        if trigger_key.source == dataclasses.I3Trigger.AMANDA_TWR_DAQ:
            return False
        if trigger_key.source == dataclasses.I3Trigger.AMANDA_MUON_DAQ:
            return False

        # make sure it's something we can simulate
        if trigger_key.type != dataclasses.I3Trigger.SIMPLE_MULTIPLICITY \
           and trigger_key.type != dataclasses.I3Trigger.STRING \
           and trigger_key.type != dataclasses.I3Trigger.VOLUME \
           and trigger_key.type != dataclasses.I3Trigger.SLOW_PARTICLE :
            return False
    return True

