#!/bin/ksh

probe_files=$(find | grep "\/probes_required")
for probe_file in $probe_files; do 
    echo -n "$probe_file: " && cat $probe_file | ./../bin/math/avg.sh
    echo
done
