#!/bin/ksh

cat | awk '{if ($1==int($1)) print int($1); else print int($1 + 1)}'
