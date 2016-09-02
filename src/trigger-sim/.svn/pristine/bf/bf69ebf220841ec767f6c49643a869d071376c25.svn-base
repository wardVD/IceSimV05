#!/usr/bin/env python

from icecube.trigger_sim.trigger_hierarchy_recipes import *
from icecube import dataio, dataclasses
from os.path import expandvars

from I3Tray import I3Units
from icecube.icetray.I3Test import *

tkey_l = list()
sid1 = dataclasses.I3Trigger.IN_ICE
tid1 = dataclasses.I3Trigger.SIMPLE_MULTIPLICITY
cid1 = 1008
tkey1 = dataclasses.TriggerKey(sid1, tid1, cid1)
tkey_l.append( tkey1 )

tr1 = dataclasses.I3Trigger()
tr1.key = tkey1

sid2 = dataclasses.I3Trigger.IN_ICE
tid2 = dataclasses.I3Trigger.SIMPLE_MULTIPLICITY
cid2 = 1010
tkey2 = dataclasses.TriggerKey(sid2, tid2, cid2)
tkey_l.append( tkey2 )

tr2 = dataclasses.I3Trigger()
tr2.key = tkey2

th = dataclasses.I3TriggerHierarchy()
th.insert( tr1 )
th.insert( tr2 )

for tk in tkey_l :
    ENSURE( len(find_triggers(th,tk)) == 1, "there should be 1 trigger found" )

ENSURE( n_triggers(th, sourceID = sid1) == 2, "expected 2 source %s, found %d" \
            % (str(sid1), n_triggers(th,sourceID = sid1) ) )

ENSURE( n_triggers(th, typeID = tid1) == 2, "expected 2 type %s found %d" \
            % (str(tid1), n_triggers(th,typeID = tid1) ) )

ENSURE( n_triggers(th, sourceID = sid1, configID = cid1) == 1, "expected 1 config %s found %d" \
            % (str(cid1), n_triggers(th,sourceID = sid1, configID = cid1) ) )

ENSURE( n_triggers(th, typeID = tid1, configID = cid1) == 1, "expected 1 config %s found %d" \
            % (str(cid1), n_triggers(th,sourceID = sid1, configID = cid1) ) )

ENSURE( n_triggers(th, sourceID = sid1, typeID = tid1, configID = cid1) == 1, "expected 1 config %s found %d" \
            % (str(cid1), n_triggers(th,sourceID = sid1, typeID = tid1, configID = cid1) ) )

ENSURE( n_triggers(th, sourceID = sid2, configID = cid2) == 1, "expected 1 config %s found %d" \
            % (str(cid2), n_triggers(th,sourceID = sid2, configID = cid2) ) )

ENSURE( n_triggers(th, typeID = tid2, configID = cid2) == 1, "expected 1 config %s found %d" \
            % (str(cid2), n_triggers(th,typeID = tid2, configID = cid2) ) )

ENSURE( n_triggers(th, sourceID = sid2, typeID = tid2, configID = cid2) == 1, "expected 1 config %s found %d" \
            % (str(cid2), n_triggers(th,sourceID = sid2, typeID = tid2, configID = cid2) ) )

ENSURE( len(trigger_lengths(th, sourceID = sid1)) == 2, "expected 2 source %s, found %d" \
            % (str(sid1), len(trigger_lengths(th,sourceID = sid1) ) ) )

ENSURE( len(trigger_lengths(th, sourceID = sid1, typeID = tid1)) == 2, "expected 2 source %s, found %d" \
            % (str(sid1), len(trigger_lengths(th,sourceID = sid1, typeID = tid1) ) ) )

ENSURE( len(trigger_lengths(th, sourceID = sid1, typeID = tid1, configID = cid1 )) == 1, "expected 1 source %s, found %d" \
            % (str(sid1), len(trigger_lengths(th,sourceID = sid1, typeID = tid1, configID = cid1 ) ) ) )

ENSURE( len(trigger_lengths(th, sourceID = sid2)) == 2, "expected 2 source %s, found %d" \
            % (str(sid2), len(trigger_lengths(th,sourceID = sid2) ) ) )

ENSURE( len(trigger_lengths(th, sourceID = sid2, typeID = tid2)) == 2, "expected 2 source %s, found %d" \
            % (str(sid2), len(trigger_lengths(th,sourceID = sid2, typeID = tid2) ) ) )

ENSURE( len(trigger_lengths(th, sourceID = sid2, typeID = tid2, configID = cid2 )) == 1, "expected 1 source %s, found %d" \
            % (str(sid2), len(trigger_lengths(th,sourceID = sid2, typeID = tid2, configID = cid2 ) ) ) )

ENSURE( len(trigger_lengths(th, typeID = tid1)) == 2, "expected 2 type %s found %d" \
            % (str(tid1), len(trigger_lengths(th,typeID = tid1) ) ) )

ENSURE( len(trigger_lengths(th, configID = cid1)) == 1, "expected 2 config %s found %d" \
            % (str(cid1), len(trigger_lengths(th,configID = cid1) ) ) )



