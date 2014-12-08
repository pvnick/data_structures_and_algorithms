#!/bin/ksh

cat | awk '
    BEGIN {
        ex = 0
        ex2 = 0
    } {
        ex += $1
        ex2 += $1 ^ 2
    } END {
        mean = ex / NR
        stddev = ex2 / NR
        stddev = sqrt(stddev - mean ^ 2)
        printf("%.20f", stddev)
    }'