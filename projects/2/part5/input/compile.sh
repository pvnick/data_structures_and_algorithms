#!/bin/sh

separator() {
    echo "-------------------------------------"
}

separator
echo "Compiling utilities"
separator

echo "Compiling randomize"
g++ --std=c++11 -Iinclude -o bin/randomize src/randomize.cpp 

echo "Compiling field extractor"
g++ --std=c++11 -Iinclude -o bin/extract_field src/extract_field.cpp 

separator
echo "Compiling area of interest binaries"
separator

echo "Compiling AOI1"
g++ --std=c++11 -Iinclude -o bin/aoi1 src/aoi1.cpp 
