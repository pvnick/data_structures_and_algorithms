#!/bin/bash

DSAA_DIRECTORY="$1"
SOURCE_FILE_PATH="$2"
CURR_DIR="$(dirname $0)"

cd "$DSAA_DIRECTORY"
git add --all 
git commit -m "Compiling"
#git push &
cd "$CURR_DIR"
nohup xterm -geometry 300x70 -e ./c++11.sh "$DSAA_DIRECTORY" "$SOURCE_FILE_PATH"
