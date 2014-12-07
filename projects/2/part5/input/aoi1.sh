#!/bin/sh

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

count_keys() {
    local INPUT_FILE="$1"
    RET=$(cat "$INPUT_FILE" | wc -l)
}

get_capacities_four_methods() {
    local NUM=$1
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

get_all_capacities() {
    #Table capacities to consider
    #
    #For each test-case, 13 table capacities will be considered; let X be the number of of keys in the test case (159,237 for the geographic data, 
    #the number of unique lexemes for the classic texts); the first table size will be X
    local KEY_COUNT="$1"
    local CAPACITIES="$KEY_COUNT"

    #the rest are described by this table:
    #X/23
    get_capacities_four_methods $(echo $KEY_COUNT | awk '{print $KEY_COUNT / 23}')
    CAPACITIES="$CAPACITIES $RET"
    #X/7
    get_capacities_four_methods $(echo $KEY_COUNT | awk '{print $KEY_COUNT / 7}')
    CAPACITIES="$CAPACITIES $RET"
    #X
    get_capacities_four_methods $KEY_COUNT
    CAPACITIES="$CAPACITIES $RET"

    RET=$CAPACITIES
}

get_input_file_capacities() {
    local INPUT_FILE="$1"
    count_keys "$INPUT_FILE"
    local KEY_COUNT=$RET
    get_all_capacities $KEY_COUNT
}

testcase() {
    local INPUT_FILE="$1"
    local FIELD_ID="$2"
    echo "Running testcase on $INPUT_FILE"

}

get_input_file_capacities data/localities-original.txt
echo $RET

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