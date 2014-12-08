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


function timing_testcase {
    typeset INPUT_FILE="$1"
    typeset FIELD_ID="$2" #field = 0 means use the whole line
    typeset HASH_REPITITIONS="$3"
    echo "Running timing testcase on $INPUT_FILE, field $FIELD_ID"
    count_keys "$INPUT_FILE"
    KEY_COUNT="$RET"
    typeset HASH_FUNCTIONS="0 1 2 3"
    echo "Calculating non-hashing overhead"
    if [ "$FIELD_ID" -eq "0" ]; then 
        typeset OVERHEAD_BASELINE_OUTPUT="$(cat $INPUT_FILE | ./bin/aoi1/timing --hash-func -1 --hash-repititions $HASH_REPITITIONS)"
    else 
        typeset OVERHEAD_BASELINE_OUTPUT="$(cat $INPUT_FILE \
            | sed 's/:/ /g' \
            | sed 's/  / . /g' \
            | ./bin/extract_field --field-to-extract $FIELD_ID \
            | ./bin/aoi1/timing --hash-func -1 --hash-repititions $HASH_REPITITIONS)"
    fi
    OVERHEAD_BASELINE="$(echo $OVERHEAD_BASELINE_OUTPUT | awk '{printf("%.20f", $1 + $2)}')"
    echo $OVERHEAD_BASELINE
    for HASH_FUNCTION in $HASH_FUNCTIONS; do
        typeset OUTPUT_FILENAME="$RESULTS_DIR/aoi1/timing/$HASH_FUNCTION.out"
        echo $OUTPUT_FILENAME
        echo "Testing hash function $HASH_FUNCTION"
        if [ "$FIELD_ID" -eq "0" ]; then 
            typeset HASH_TIME_OUTPUT="$(cat $INPUT_FILE | ./bin/aoi1/timing --hash-func $HASH_FUNCTION --hash-repititions $HASH_REPITITIONS)"
        else 
            typeset HASH_TIME_OUTPUT="$(cat $INPUT_FILE \
                | sed 's/:/ /g' \
                | sed 's/  / . /g' \
                | ./bin/extract_field --field-to-extract $FIELD_ID \
                | ./bin/aoi1/timing --hash-func $HASH_FUNCTION --hash-repititions $HASH_REPITITIONS)"
        fi
        HASH_TIME="$(echo $HASH_TIME_OUTPUT | awk '{printf("%.20f", $1 + $2)}')"
        CORRECTED_HASH_TIME="$(echo $HASH_TIME $OVERHEAD_BASELINE | awk '{printf("%.20f", $1 - $2)}')"
        TIME_PER_HASH="$(echo $CORRECTED_HASH_TIME $KEY_COUNT | awk '{printf("%.20f", $1 / $2)}')"
        echo "$TIME_PER_HASH" >> $OUTPUT_FILENAME
    done
}

function testcase {
    typeset INPUT_FILE="$1"
    typeset FIELD_ID="$2" #field = 0 means use the whole line
    typeset HASH_REPITITIONS="$3"
    echo "Running testcases on $INPUT_FILE"
    timing_testcase "$INPUT_FILE" "$FIELD_ID" "$HASH_REPITITIONS"
}


rm -r "$RESULTS_DIR/aoi1/timing/"
mkdir -p "$RESULTS_DIR/aoi1/timing/"

testcase "data/aeneid_vergil-unique.txt" "0" "100000"
testcase "data/confucian-analects-unique.txt" "0" "100000"
testcase "data/divine-comedy_dante-unique.txt" "0" "100000"
testcase "data/origin-of-the-species_darwin-unique.txt" "0" "100000"
#the location files are much slower, so decrease repetitions by an order of magnitude
#the uid field (interpreted as a string) from the geographic data,
testcase "data/localities-uid.txt" "0" "10000"
#the location field (interpreted as a string) from the geographic data,
testcase "data/localities-location.txt" "0" "10000"
#the population field (interpreted as a string) from the geographic data,
testcase "data/localities-population.txt" "0" "10000"
#the coords field (interpreted as a string) from the geographic data, &
testcase "data/localities-coords.txt" "0" "10000"
#each entire line (interpreted as a string) from the geographic data.
testcase "data/localities-original.txt" "0" "10000"

#Timing trial overview
#
#for each key in test case's file
#  compute the key's hash
#report utime and stime 
#
#Timing experiment overview
#
#for each key data set 
#  run a timing trial
#
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
#Timing experiment overview
#
#for each table capacity
#  for each key data set 
#    run a collision counting trial
#
#Note
#
#This set of experiments is testing hash functions; not your hash map implementations!
#Analyzing the data and reporting the results
#
#For each question, you be expected to provide a textual answer and justification, supported with appropriate graphs, charts, and or tables. Let's consider the questions posed in order...
#How quickly does a particular function compute a hash for keys of type K?
#
#The first thing to do is to compute the mean time to hash a key in data set i using function f [hash time for all keys in data set i / number of keys in data set i] for each {data set + hash function} pairing. The second thing to do is to compute the mean time to hash a key for each hash function across all the data sets [sum( hashing time for all keys in data set i ) / sum( number of keys in data set i)]. Then for each data set, we can visually present those means using a histogram:
#
#    for each function
#        y-axis: time per key
#        x-axis: data set 1, ... , data set n, across all data sets
#
#We should also include a box-and-whisker with median bar and whiskerbars graph with summarizing each hash functions performance across all the data sets.
#
#        y-axis: time per key 
#        x-axis: hash function 1, ... , hash function 4
#
#Followed by a small table giving the means and standard deviations.
#How well does a particular hash function for keys of type K, perform at randomly distributing those keys for a table capacity of n for each of the data sets?
#
#Here we consider is how evenly keys are distributed through the table. Assume we have a table with a capacity of X and X different keys. A perfect hash function would map each keys to a unique slot—in other words, it would evenly distribute the keys throughout the table. If there were Y keys, such that X < Y ≤ 2X, then no slot would have more than two keys mapped to it, &c.
#
#Given table capacity C and K keys, then the (ideal) expected number of keys per slot is
#
#    E_i = K/C
#
#Then we use the formula for Pearson's cumulative test statistic (which approximates a Chi-squared distribution). Let O_i be the observed number of keys mapped to slot i
#
#     2   n              2
#    X = SUM[ (O_i - E_i)  / E_i ]
#        i=1
#
#Now we'll do something that would make a statistician cringe: we'll assume the smaller X^2, the more uniform the distribution, and thus the better. [Notice that if there are no differences between the observed and expected values, that X^2 is zero, and increases the more and greater the differences.]
#
#I leave it to you to determine an appropriate way to present this information.
#For data sets with keys of type K, which hash function does the best over-all job?
#
#Devise and justify a technique for choosing the best (simply “eye-balling it” won't do!).
#For data sets with keys of type K, which hash function does the best over-all job for a table capacity of 3119?
#
#Devise and justify a technique for choosing the best (simply “eye-balling it” won't do!). I'll want to see a histogram:
#
#    for each function
#        y-axis: X^2 (Pearson's cumulative test statistic)
#        x-axis: data set 1, ... , data set n
#
