#!/bin/ksh

. ./common.inc

#Area of investigation #1
#Hash function performance
#
#Goal: identify the “best” performing hash function for keys of type K.
#Questions to be answered
#
#    How quickly does a particular function compute a hash for keys of type K?
#    How well does a particular hash function for keys of type K, perform at randomly distributing those keys for a table capacity of n for each of the data sets?
#    For data sets with keys of type K, which hash function does the best over-all job?
#    For data sets with keys of type K, which hash function does the best over-all job for a table capacity of 3119?
#
#Experimental protocol
#
#To answer these questions, two kinds of experiment, each consisting of multiple trials will need to be performed.
#Input
#
#There shall be ten data sets used in these tests:
#
#    aeneid_vergil-unique.txt,
#    confucian-analects-unique.txt,
#    divine-comedy_dante-unique.txt,
#    origin-of-the-species_darwin-unique.txt,
#    war-and-peace_tolstoi-unique.txt,
#    the uid field (interpreted as a string) from the geographic data,
#    the location field (interpreted as a string) from the geographic data,
#    the population field (interpreted as a string) from the geographic data,
#    the coords field (interpreted as a string) from the geographic data, &
#    each entire line (interpreted as a string) from the geographic data. 
#

function count_keys {
    typeset INPUT_FILE="$1"
    RET=$(cat "$INPUT_FILE" | wc -l)
}

function get_capacities_four_methods {
    typeset NUM=$1
    #nearest larger prime 
    METHOD1=$(echo $NUM | ./bin/math/next_highest_prime.sh)
    #nearest larger power of two
    METHOD2=$(echo $NUM | ./bin/math/next_highest_pow_2.sh)
    #prime nearest larger power of two
    METHOD3=$(echo $NUM | ./bin/math/next_highest_pg591_prime.sh)
    #prime in middle of nearest powers of two
    METHOD4=$(echo $NUM | ./bin/math/next_highest_pow2_mid_prime.sh)
    RET="$METHOD1 $METHOD2 $METHOD3 $METHOD4"
}

function get_all_capacities {
    #Table capacities to consider
    #
    #For each test-case, 13 table capacities will be considered; let X be the number of of keys in the test case (159,237 for the geographic data, 
    #the number of unique lexemes for the classic texts); the first table size will be X
    typeset KEY_COUNT="$1"
    typeset CAPACITIES="$KEY_COUNT"

    #the rest are described by this table:
    #X/23
    get_capacities_four_methods "$(echo $KEY_COUNT | awk '{print '$KEY_COUNT' / 23}')"
    CAPACITIES="$CAPACITIES $RET"
    #X/7
    get_capacities_four_methods "$(echo $KEY_COUNT | awk '{print '$KEY_COUNT' / 7}')"
    CAPACITIES="$CAPACITIES $RET"
    #X
    get_capacities_four_methods "$KEY_COUNT"
    CAPACITIES="$CAPACITIES $RET"

    RET=$CAPACITIES
}

function get_input_file_capacities {
    typeset INPUT_FILE="$1"
    count_keys "$INPUT_FILE"
    typeset KEY_COUNT=$RET
    get_all_capacities $KEY_COUNT
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
            typeset OUTPUT_FILENAME="$RESULTS_DIR/aoi1/collision/$HASH_FUNCTION""+""$INPUT_FILE_NAME""+""$CAPACITY"
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


rm -r "$RESULTS_DIR/aoi1/collision/"
mkdir -p "$RESULTS_DIR/aoi1/collision/"


testcase "aeneid_vergil-unique.txt" "0"
testcase "confucian-analects-unique.txt" "0"
testcase "divine-comedy_dante-unique.txt" "0"
testcase "origin-of-the-species_darwin-unique.txt" "0"
#the uid field (interpreted as a string) from the geographic data,
testcase "localities-original.txt" "1"
#the location field (interpreted as a string) from the geographic data,
testcase "localities-original.txt" "2"
#the population field (interpreted as a string) from the geographic data,
testcase "localities-original.txt" "4"
#the coords field (interpreted as a string) from the geographic data, &
testcase "localities-original.txt" "5"
#each entire line (interpreted as a string) from the geographic data. 
testcase "localities-original.txt" "0"