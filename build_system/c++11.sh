#!/bin/bash

DSAA_BASE_PATH="$1"
SOURCE_FILE_PATH="$2"
echo $DSAA_BASE_PATH

cd $SOURCE_FILE_PATH
echo "Building"
CRASH_TRACKING_HEADER_FILE_PATH="$DSAA_BASE_PATH"track_crashes.h
g++ --include "$CRASH_TRACKING_HEADER_FILE_PATH" -g -rdynamic -Wall *.cpp -std=c++11 -o main
COMPILER_RETURN_CODE=$?
if [ $COMPILER_RETURN_CODE -eq 0 ]; then
	echo "Running"
	echo "--------------------------"
	./main 2>&1 | $DSAA_BASE_PATH/build_system/debug_output.sh
	echo "--------------------------"
else
	echo "Compile error"
fi

read -n1 -p "Press any key to continue"
