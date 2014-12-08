#!/bin/ksh

. ./common.inc

rm -r "$RESULTS_DIR/aoi1/collision_chisquare/"
mkdir -p "$RESULTS_DIR/aoi1/collision_chisquare/"

CHISQUARE_DATA_OUTPUT_FILE="$RESULTS_DIR/aoi1/collision_chisquare/results"
RANDOMNESS_CHISQUARE_SUMS_OUTPUT_FILE="$RESULTS_DIR/aoi1/collision_chisquare/randomness_chisquare_sums"
RANDOMNESS_RANK_SUMS_OUTPUT_FILE="$RESULTS_DIR/aoi1/collision_chisquare/randomness_rank_sums"

FILEPATHS=$(find results/aoi1/collision/* -type f)
for FILEPATH in $FILEPATHS; do
    FILENAME=$(echo "$FILEPATH" | sed -e "s/^.*\///g")
    K=$(cat $FILEPATH | awk 'BEGIN{sum=0}{sum += $1 * $2}END{print sum}')
    HASH_FUNCTION=$(echo $FILENAME | cut -d+ -f1)
    INPUT_FILENAME=$(echo $FILENAME | cut -d+ -f2)
    C=$(echo $FILENAME | cut -d+ -f3)
    Ei=$(echo "$K $C" | awk '{printf("%.20f", $1/$2)}')
    echo 'BEGIN{Ei='$Ei'; chi_square=0; num_clusters=0; capacity='$C'}
        {
            cluster_size = $1
            num_instances = $2
            for (i = 1; i <= num_instances; i++) {
                Oi = cluster_size
                chi_square += (Oi - Ei) ^ 2 / Ei
                num_clusters++
            }
        }END{
            #the above only accounted for occupied slots, but we must also account for unoccupied slots (ie Oi=0)
            unoccupied_slots=capacity-num_clusters
            chi_square += num_clusters * Ei
            printf("%.20f", chi_square)
        }' >> foobar
    chi_square=$(cat $FILEPATH | awk 'BEGIN{Ei='$Ei'; chi_square=0; num_clusters=0; capacity='$C'}
        {
            cluster_size = $1
            num_instances = $2
            for (i = 1; i <= num_instances; i++) {
                Oi = cluster_size
                chi_square += (Oi - Ei) ^ 2 / Ei
                num_clusters++
            }
        }END{
            #the above only accounted for occupied slots, but we must also account for unoccupied slots (ie Oi=0)
            unoccupied_slots=capacity-num_clusters
            chi_square += num_clusters * Ei
            printf("%.20f", chi_square)
        }')
    echo "$C $K $chi_square $HASH_FUNCTION" >> $CHISQUARE_DATA_OUTPUT_FILE
done

#sum up the chi square values per hash function. the one with the lowest sum is the best
cat $CHISQUARE_DATA_OUTPUT_FILE \
    | sort -n -k1,1 -k2,2 -k3,3 \
    | awk '
        BEGIN{
            prev_cap = 0
            prev_key_count = 0
        }{
            curr_cap = $1
            curr_key_count = $2
            chi_square = $3
            hash_function=$4
            if (curr_cap != prev_cap || curr_key_count != prev_key_count) 
                rank=0; 
            print(hash_function, rank); 
            rank += chi_square; 
            prev_cap = curr_cap; 
            prev_key_count = curr_key_count
        }' \
    | sort -n \
    | awk '
        BEGIN{
            prev_hash_function = 0
            rank_sum = 0
        }{
            curr_hash_function = $1
            rank_sum += $2
            if (curr_hash_function != prev_hash_function) { 
                printf("%d %.5f\n", prev_hash_function, rank_sum)
                rank_sum = 0
            } 
            prev_hash_function = curr_hash_function
        } END{
            printf("%d %.5f", prev_hash_function, rank_sum)
        }' > $RANDOMNESS_CHISQUARE_SUMS_OUTPUT_FILE

#sum up the chi square ranks per hash function. the one with the rank sum is the best
cat $CHISQUARE_DATA_OUTPUT_FILE \
    | sort -n -k1,1 -k2,2 -k3,3 \
    | awk '
        BEGIN{
            prev_cap = 0
            prev_key_count = 0
        }{
            curr_cap = $1
            curr_key_count = $2
            chi_square = $3
            hash_function=$4
            if (curr_cap != prev_cap || curr_key_count != prev_key_count) 
                rank=0; 
            print(hash_function, rank); 
            rank += 1; 
            prev_cap = curr_cap; 
            prev_key_count = curr_key_count
        }' \
    | sort -n \
    | awk '
        BEGIN{
            prev_hash_function = 0
            rank_sum = 0
        }{
            curr_hash_function = $1
            rank_sum += $2
            if (curr_hash_function != prev_hash_function) { 
                printf("%d %.5f\n", prev_hash_function, rank_sum)
                rank_sum = 0
            } 
            prev_hash_function = curr_hash_function
        } END{
            printf("%d %.5f", prev_hash_function, rank_sum)
        }' > $RANDOMNESS_RANK_SUMS_OUTPUT_FILE