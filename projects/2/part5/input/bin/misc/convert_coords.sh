#!/bin/ksh 
#this does not work on bsd unfortunately

cat \
    | sed -r 's/^([0-9])/+\1/g' \
    | sed -r 's/^\-/S/g' \
    | sed -r 's/^\+/N/g' \
    | sed -r 's/N0\.09999/-/g' \
    | sed -r 's/^([NS])([0-9]*?)([0-9]{2})\./\2 \3 \1,/g' \
    | awk 'BEGIN{FS=",";OFS=","}{
        lat=$1
        XXXXX=$2
        if (XXXXX < 18000)
            print lat, XXXXX
        else
            print lat, (XXXXX - 36000)
    }' \
    | sed -r 's/,([0-9])/,+\1/g' \
    | sed -r 's/,\-/,W/g' \
    | sed -r 's/,\+/,E/g' \
    | sed -r 's/,([EW])([0-9]*?)([0-9]{2})$/, \2 \3 \1/g' \
    | sed -r 's/,  /, 0 /g' \
    | sed -r 's/^ /0 /g' \
    | sed -r 's/([^0-9])0+([0-9]+)/\1\2/g' \
    | sort | uniq