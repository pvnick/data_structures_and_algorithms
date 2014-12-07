#!/bin/sh

cat | awk '{print 2^$1}'
