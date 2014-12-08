#!/bin/ksh

. ./common.inc

rm -r "$RESULTS_DIR/aoi1/timing_statistics/"
mkdir -p "$RESULTS_DIR/aoi1/timing_statistics/"

HASH_FUNCTIONS="0 1 2 3"
for HASH_FUNCTION in $HASH_FUNCTIONS; do
    cat "$RESULTS_DIR/aoi1/timing/$HASH_FUNCTION.out" | ./bin/math/avg.sh > $RESULTS_DIR/aoi1/timing_statistics/$HASH_FUNCTION.mean
    cat "$RESULTS_DIR/aoi1/timing/$HASH_FUNCTION.out" | ./bin/math/stddev.sh > $RESULTS_DIR/aoi1/timing_statistics/$HASH_FUNCTION.stddev
    echo "Hash function $HASH_FUNCTION, mean=$(cat $RESULTS_DIR/aoi1/timing_statistics/$HASH_FUNCTION.mean), stddev=$(cat $RESULTS_DIR/aoi1/timing_statistics/$HASH_FUNCTION.stddev)" 
done