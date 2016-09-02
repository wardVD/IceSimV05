#!/bin/bash

gpu=0
seed=0
in=$I3_TESTDATA/icesim-corsika/F2K010001.gz
out=example.i3.gz
./run.py corsika $gpu $seed $in $out
