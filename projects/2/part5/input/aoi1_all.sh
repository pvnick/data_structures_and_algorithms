#!/bin/ksh

. ./common.inc

separator
echo "Timing test"
separator
./aoi1_timing.sh
echo "Done"

separator
echo "Timing statistics"
separator
./aoi1_timing_statistics.sh
echo "Done"

separator
echo "Collision test"
separator
./aoi1_collision.sh
echo "Done"

separator
echo "Collision test chi square statistics"
separator
./aoi1_chisquare.sh
echo "Done"

separator
echo "Collision test, capacity 3119"
separator
./aoi1_collision_3119.sh
echo "Done"

separator
echo "Collision test chi square statistics, capacity 3119"
separator
./aoi1_chisquare_3119.sh
echo "Done"