#!/bin/ksh

MY_DIR=`dirname $0`
cd "$MY_DIR"

INPUT=$(cat)
cat pow2_mid_primes.txt | awk '{if ($1 >= '$INPUT') { print $1; exit 0}}'
