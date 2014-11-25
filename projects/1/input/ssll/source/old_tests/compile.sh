#!/bin/sh

g++ --std=c++11 -I"$(pwd)/../../../" fuzzer.cpp -o fuzzer
