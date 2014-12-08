#!/bin/ksh

MY_DIR=`dirname $0`
cd "$MY_DIR"
cat | ./lg.sh | ./floor.sh | awk '{print $1 + 1}' | ./pow2.sh | ./round.sh
