#!/bin/sh

CUDA=/usr/local/cuda
CUDALIB=$CUDA/lib64
CUDABIN=$CUDA/bin

if test "$I3_SRC" = "" || test "$I3_BUILD" = ""; then
    echo "Error: I3_SRC or I3_BUILD is not set, exiting!"
else

    cd $I3_BUILD/ppc; make

    cd $I3_SRC/ppc/private/ppc/gpu
    if test "$1" = "cpu"; then
	make clib
    elif test "$1" = "gpu"; then
	PATH=$PATH:$CUDABIN make glib 2>&1 | grep -v assuming
    else
	echo "Do not forget to specify parameter: $0 [cpu/gpu]!"
    fi

    cd $I3_BUILD/lib
    ln -sf $I3_SRC/ppc/private/ppc/gpu/libxppc.so

    if test "$1" = "gpu"; then
	if test -d "$CUDALIB"; then
	    ln -sf $CUDALIB/libcudart.so
	else
	    echo "Error: directory $CUDALIB does not exit! Edit this file: $0."
	fi
    fi

fi
