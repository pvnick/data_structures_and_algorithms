#!/bin/sh

cat | awk '{print (log($1) / log(2))}'
