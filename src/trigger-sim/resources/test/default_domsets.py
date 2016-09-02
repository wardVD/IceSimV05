#!/usr/bin/env python
from icecube.icetray.I3Test import ENSURE
from icecube import dataclasses,trigger_sim

sets = trigger_sim.GetDefaultDOMSets()

max_len_set = list()
for k,v in sets:
    if len(v) > len(max_len_set):
        max_len_set = v

ENSURE( len(max_len_set) == 4 ,"There should be 4 DOM sets in this list not %d." \
        % len(max_len_set))

for domset in [2,4,5,6]:
    ENSURE( domset in max_len_set, "DOM set %d not found in default lists." % domset)

