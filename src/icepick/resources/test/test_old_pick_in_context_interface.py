#!/usr/bin/env python

#
#  Test that the icepick-in-the-context interface on I3ConditionalModule works
#

from icecube import icetray
from I3Tray import *
from icecube import icepick 

tray = I3Tray()

tray.AddModule('BottomlessSource', 'bs')

def int_putter(frame, where = 'someplace', value = -1):
    if int_putter.i % 2 == 0:
        frame[where] = icetray.I3Int(value)
    int_putter.i += 1

int_putter.i = 0

tray.AddModule(int_putter, 'putter',
               where = 'some_int',
               value = 777)

#
#  This doublet should cause the nulls to get added only when
#  the frame contains something at 'some_int'
#
tray.AddService('I3IcePickInstaller<I3FrameObjectFilter>', 'fofilter')(
    ("FrameObjectKey", 'some_int')
    )

tray.AddModule('AddNulls', 'adder')(
    ('IcePickServiceKey', 'fofilter'),
    ('where', ['x1', 'x2', 'x3'])
    )
    
tray.AddModule('Dump', 'dump')

def check(frame):
    if 'some_int' in frame:
        assert 'x1' in frame, "eh, x1 isn't in the frame!"
    else:
        assert 'x1' not in frame, "x1 shouldn't be here"
        
tray.AddModule(check, 'check')

tray.AddModule('TrashCan', 'tc')

tray.Execute(5)
