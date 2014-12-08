#!/bin/ksh

cat | awk 'BEGIN{numerator=0;denominator=0;}{numerator += $1; denominator++}END{printf("%.20f", numerator/denominator)}'
