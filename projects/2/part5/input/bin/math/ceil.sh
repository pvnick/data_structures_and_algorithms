#!/bin/sh

cat | awk '{if ($1==int($1)) print $1; else print int($1 + 1)}'
