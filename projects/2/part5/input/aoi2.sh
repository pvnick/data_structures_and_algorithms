#!/bin/ksh

. ./common.inc

#Fixed size table experiment overview
#
#for each load factor from 0.03 to 0.99 by 0.03
#  for each set of data sets  [e.g., The Aeneid set of data sets]
#    for each data set in that set of data sets  [e.g., permutation 1]
#      run an individual trial



#Variable size table experiment overview
#
#for each a from 0.0 to 1.0 by 0.05
#  run a fixed size table experiment

function run_testcase {
    typeset INPUT_FILE="$1"
    typeset INPUT_FILE_UNIQ="$2"
    typeset a="$3"
    typeset CAPACITY="$4"
    typeset CAPACITY_FUNC="$5"
    typeset LOAD_FACTOR="$6"
    OUTPUT_DIR="$RESULTS_DIR/aoi2/$a/$LOAD_FACTOR/$CAPACITY_FUNC/$INPUT_FILE/$INPUT_FILE_UNIQ"
    echo "Running testcase:"
    echo "    input file: $INPUT_FILE"
    echo "    corresponding unique input file: $INPUT_FILE_UNIQ"
    echo "    a: $a"
    echo "    load factor: $LOAD_FACTOR"
    echo "    capacity planning function: $CAPACITY_FUNC"
    echo "    capacity: $CAPACITY"
    echo "    output directory: $OUTPUT_DIR"
    echo -n "..."

    mkdir -p "$OUTPUT_DIR"
    ./bin/aoi2/individual_trial --capacity $CAPACITY --target-load $LOAD_FACTOR --input-file "$INPUT_FILE" --input-file-unique "$INPUT_FILE_UNIQ" --output-dir "$OUTPUT_DIR"
    echo "done"
}

function fixed_size_testcase {
    typeset INPUT_FILE="$1"
    typeset INPUT_FILE_UNIQ="$2"
    typeset a="$3"
    count_keys "$INPUT_FILE"
    KEY_COUNT=$RET
    M=$(echo $KEY_COUNT | awk '{printf("%.5f", $1 * 0.95)}' | ./bin/math/ceil.sh)
    for LOAD_FACTOR_ID in {1..33}; do
        LOAD_FACTOR=$(echo $LOAD_FACTOR_ID | awk '{printf("%.2f", $1 * 0.03)}')
        #Usage: ./bin/aoi2/individual_trial --capacity ULONG --target-load DOUBLE --input-file FILE --input-file-unique FILE --output-dir DIR
        #Alternatively you may print this message with the following command:
        #./bin/aoi2/individual_trial help
        #    
        #Parameter descriptions:
        #  --capacity ULONG: map capacity
        #  --target-load DOUBLE: target load factor
        #  --input-file FILE: file containing the data set
        #  --input-file-unique FILE: corresponding file containing the unique data set
        #  --output-dir DIR: where to save the results

        #Let's assume that the user supplies an upper-bound, M on the number of items she will insert into the table. We want to choose a capacity C, such that C is one of:
        #1 - M + aM, where 0 ≤ a ≤ 1.0
        #2 - N, where N, is the “closest” (≥) prime to (M + aM), where 0 ≤ a ≤ 1.0
        #3 - N, where N is the closest, power of two ≥ M
        #4 - N, where N is the prime that is “closest” to the closest power of two ≥ than M
        #5 - N, where N is a prime that is ≥ M and “closest” to being half-way between the nearest powers of two bounding M above and below.

        #1
        CAPACITY=$(echo "$M $a" | awk '{printf("%.5", $1 + $2 * $1)}' | ./bin/math/ceil.sh)
        #2
        CAPACITY=$(echo "$M $a" | awk '{printf("%.5", $1 + $2 * $1)}' | ./bin/math/ceil.sh | ./bin/math/next_highest_prime.sh)
        #3
        CAPACITY=$(echo "$M" | ./bin/math/next_highest_pow_2.sh)
        #4
        CAPACITY=$(echo "$M" | ./bin/math/next_highest_pg591_prime.sh)
        #5
        CAPACITY=$(echo "$M" | ./bin/math/next_highest_pow2_mid_prime.sh)
    done
}

function variable_size_testcase {
    typeset INPUT_FILE="$1"
    typeset INPUT_FILE_UNIQ="$2"
    A_IDs={0..20}
    for A_ID in $A_IDs; do
        a=$(cat $A_ID | awk '{printf("%.2f", 0.05 * $1)}')
    done
}


rm -r "$RESULTS_DIR/aoi2"
mkdir -p "$RESULTS_DIR/aoi2"

#    The Aeneid set of data sets
#        this set of data sets consists of:
#
#            aeneid_vergil-unique.txt
#            five (5) permutations of the file aeneid_vergil-unique.txt (see “Creating permutations from a data set” for instructions
#            aeneid_vergil.txt
./bin/create_permutations.sh data/aeneid_vergil-unique.txt 5
variable_size_testcase "data/aeneid_vergil.txt" "data/aeneid_vergil-unique.txt"
variable_size_testcase "data/aeneid_vergil.txt" "data/aeneid_vergil-unique.txt-1"
variable_size_testcase "data/aeneid_vergil.txt" "data/aeneid_vergil-unique.txt-2"
variable_size_testcase "data/aeneid_vergil.txt" "data/aeneid_vergil-unique.txt-3"
variable_size_testcase "data/aeneid_vergil.txt" "data/aeneid_vergil-unique.txt-4"
variable_size_testcase "data/aeneid_vergil.txt" "data/aeneid_vergil-unique.txt-5"

#    War and Peace set of data sets
#        as per the The Aeneid, but using the War and Peace files. 
./bin/create_permutations.sh data/war-and-peace_tolstoi-unique.txt 5
variable_size_testcase "data/war-and-peace_tolstoi.txt" "data/war-and-peace_tolstoi-unique.txt"
variable_size_testcase "data/war-and-peace_tolstoi.txt" "data/permutations/war-and-peace_tolstoi-unique.txt-1"
variable_size_testcase "data/war-and-peace_tolstoi.txt" "data/permutations/war-and-peace_tolstoi-unique.txt-1"
variable_size_testcase "data/war-and-peace_tolstoi.txt" "data/permutations/war-and-peace_tolstoi-unique.txt-1"
variable_size_testcase "data/war-and-peace_tolstoi.txt" "data/permutations/war-and-peace_tolstoi-unique.txt-1"
variable_size_testcase "data/war-and-peace_tolstoi.txt" "data/permutations/war-and-peace_tolstoi-unique.txt-1"

#    Geographical UID set of data sets
#        this set of data sets consists of:
#
#            the UID's extracted from localities-uid.txt and saved, in original order, to a file named uid-unique.txt.
#            five (5) permutations of the file uid-unique.txt
#
#        Note: the keys in this set will be treated as std::strings 
./bin/create_permutations.sh data/uid-unique.txt 5
variable_size_testcase "data/uid-unique.txt" "data/permutations/uid-unique.txt-1"
variable_size_testcase "data/uid-unique.txt" "data/permutations/uid-unique.txt-2"
variable_size_testcase "data/uid-unique.txt" "data/permutations/uid-unique.txt-3"
variable_size_testcase "data/uid-unique.txt" "data/permutations/uid-unique.txt-4"
variable_size_testcase "data/uid-unique.txt" "data/permutations/uid-unique.txt-5"

#    Geographical coords set of data sets
#        this set of data sets consists of:
#
#            the coordinates extracted from localities-coords.txt and converted to a string of the standard form (e.g., -12345.03421 → “123 45 S, 34 21 E” and 4567.23365 → “45 67 N, 126 35 W”) saved, in original order, to a file named coords-unique.txt [Note: this file will actually contain duplicates; if you are on a UNIX(-like) system, from a command line you can use the command
#
#            sort < input.txt | sort -u > output.txt
#
#            to remove the duplicates; doing this is highly recommended!]
#            five (5) permutations of the file coords-unique.txt
./bin/create_permutations.sh data/coords-unique.txt 5
variable_size_testcase "data/coords-unique.txt" "data/permutations/coords-unique.txt-1"
variable_size_testcase "data/coords-unique.txt" "data/permutations/coords-unique.txt-2"
variable_size_testcase "data/coords-unique.txt" "data/permutations/coords-unique.txt-3"
variable_size_testcase "data/coords-unique.txt" "data/permutations/coords-unique.txt-4"
variable_size_testcase "data/coords-unique.txt" "data/permutations/coords-unique.txt-5"

#    Geographical entries set of data sets
#        this set of data sets consists of:
#
#            localities-original.txt
#            localities-uid.txt
#            localities-location.txt
#            localities-country.txt
#            localities-population.txt
#            localities-coords.txt
#            two (2) permutations of the file localities-original.txt
#
#        Where the protocol calls for the “corresponding unique data set”, use localities-original.txt 
./bin/create_permutations.sh data/localities-original.txt 2
variable_size_testcase "data/localities-original.txt" "data/localities-original.txt"
variable_size_testcase "data/localities-original-sorted-by-uid.txt" "data/localities-original.txt"
variable_size_testcase "data/localities-original-sorted-by-location.txt" "data/localities-original.txt"
variable_size_testcase "data/localities-original-sorted-by-country.txt" "data/localities-original.txt"
variable_size_testcase "data/localities-original-sorted-by-population.txt" "data/localities-original.txt"
variable_size_testcase "data/localities-original-sorted-by-coords.txt" "data/localities-original.txt"
variable_size_testcase "data/permutations/localities-original.txt-1" "data/localities-original.txt"
variable_size_testcase "data/permutations/localities-original.txt-2" "data/localities-original.txt"