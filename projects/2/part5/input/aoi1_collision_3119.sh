#!/bin/ksh

. ./common.inc

function get_input_file_capacities {
    RET="3119"
}

function collision_testcase {
    #Collision counting trial overview
    #
    #for each key in test case's file
    #  compute the key's hash
    #  for each table capacity to be considered
    #    compute index from key for tables 
    #    increment count of keys hashing to that index for that table's capacity
    #for each table capacity to be considered
    #  output the table's capacity
    #  for X = 1 to table size
    #    if at least one index has exactly X collisions
    #      output: X and the number of indices that has exactly X collisions
    #
    typeset INPUT_FILE_NAME="$1"
    typeset FIELD_ID="$2" #field = 0 means use the whole line
    typeset INPUT_FILE_PATH="data/$INPUT_FILE_NAME"
    echo "Running collision testcase on $INPUT_FILE_PATH, field $FIELD_ID"
    count_keys "$INPUT_FILE_PATH"
    KEY_COUNT="$RET"
    echo "Number of keys=$RET"
    get_input_file_capacities "$INPUT_FILE_PATH"
    CAPACITIES="$RET"
    echo "Capacities=[$CAPACITIES]"
    HASH_FUNCTIONS="0 1 2 3"
    for HASH_FUNCTION in $HASH_FUNCTIONS; do
        for CAPACITY in $CAPACITIES; do
            typeset OUTPUT_FILENAME="$RESULTS_DIR/aoi1/collision_3119/$HASH_FUNCTION""+""$INPUT_FILE_NAME""+""$CAPACITY"
            ensure_file_dir_exists "$OUTPUT_FILENAME"
            echo "Testing capacity $CAPACITY and outputting to $OUTPUT_FILENAME"
            if [ "$FIELD_ID" -eq "0" ]; then 
                cat "$INPUT_FILE_PATH" \
                    | ./bin/aoi1/collision --hash-func $HASH_FUNCTION --map-capacity $CAPACITY \
                    > $OUTPUT_FILENAME
            else 
                cat "$INPUT_FILE_PATH" \
                    | sed 's/:/ /g' \
                    | sed 's/  / . /g' \
                    | ./bin/extract_field --field-to-extract $FIELD_ID \
                    | ./bin/aoi1/collision --hash-func $HASH_FUNCTION --map-capacity $CAPACITY \
                    > $OUTPUT_FILENAME
            fi
        done
    done
}

function testcase {
    typeset INPUT_FILE="$1"
    typeset FIELD_ID="$2" #field = 0 means use the whole line
    echo "Running testcases on $INPUT_FILE"
    collision_testcase "$INPUT_FILE" "$FIELD_ID"
}


rm -r "$RESULTS_DIR/aoi1/collision_3119/"
mkdir -p "$RESULTS_DIR/aoi1/collision_3119/"


testcase "aeneid_vergil-unique.txt" "0" "100000"
testcase "confucian-analects-unique.txt" "0" "100000"
testcase "divine-comedy_dante-unique.txt" "0" "100000"
testcase "origin-of-the-species_darwin-unique.txt" "0" "100000"
#the location files are much slower, so decrease repetitions by an order of magnitude
#the uid field (interpreted as a string) from the geographic data,
testcase "localities-uid.txt" "0" "10000"
#the location field (interpreted as a string) from the geographic data,
testcase "localities-location.txt" "0" "10000"
#the population field (interpreted as a string) from the geographic data,
testcase "localities-population.txt" "0" "10000"
#the coords field (interpreted as a string) from the geographic data, &
testcase "localities-coords.txt" "0" "10000"
#each entire line (interpreted as a string) from the geographic data.
testcase "localities-original.txt" "0" "10000"
