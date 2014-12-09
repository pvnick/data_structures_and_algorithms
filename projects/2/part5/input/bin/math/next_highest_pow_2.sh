#!/bin/ksh

MY_DIR=`dirname $0`
cd "$MY_DIR"
cat | ./lg.sh | ./ceil.sh | ./pow2.sh | ./round.sh
