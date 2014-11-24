#!/bin/bash

g++ -g --std=c++11 -I"$(pwd)/../../../" fuzzer.cpp -o fuzzer
