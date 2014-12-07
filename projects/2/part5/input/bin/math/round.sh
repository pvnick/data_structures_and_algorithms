#!/bin/sh

cat | awk '{print int($1 + 0.5)}'
