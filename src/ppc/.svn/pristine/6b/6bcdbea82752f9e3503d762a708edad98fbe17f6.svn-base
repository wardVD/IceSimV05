#!/bin/sh
awk 'BEGIN {o=0.0359449; n=0;   s=1.1;   a=1.1} {print $1, s*$2, a*($3+o)-o, $4}' icemodel.dat > icemodel.s+.1a+.1
awk 'BEGIN {o=0.0359449; n=0; s=1/1.1; a=1/1.1} {print $1, s*$2, a*($3+o)-o, $4}' icemodel.dat > icemodel.s-.1a-.1
awk 'BEGIN {o=0.0359449; n=0;   s=1.1; a=1/1.1} {print $1, s*$2, a*($3+o)-o, $4}' icemodel.dat > icemodel.s+.1a-.1
awk 'BEGIN {o=0.0359449; n=0; s=1/1.1;   a=1.1} {print $1, s*$2, a*($3+o)-o, $4}' icemodel.dat > icemodel.s-.1a+.1
