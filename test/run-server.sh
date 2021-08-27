#!/bin/bash

NUM_PROCS=${1:-1}
PROTOCOL=${2:-"na+sm"}
BUILD_PATH=${3:-"`pwd`/build"}
SSG_FILE=group.ssg

BUILD_PATH=`cd "$BUILD_PATH"; pwd`

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BUILD_PATH

rm -f $SSG_FILE

mpirun -np $NUM_PROCS colza-dist-server \
        -c pipeline.json \
        -a $PROTOCOL \
        -s $SSG_FILE \
        -t 1 \
        -v trace
