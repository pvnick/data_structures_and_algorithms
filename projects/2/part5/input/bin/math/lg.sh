#!/bin/ksh

cat | awk '{printf("%.20f", log($1) / log(2))}'
