#ifndef _OPEN_ADDRESSING_MAP_H_
#define _OPEN_ADDRESSING_MAP_H_

#include <iostream>
#include "../../common/common.h"
#include "../../part4/source/rbst.h"

namespace cop3530 {
    class HashMapOpenAddressing {
    private:
        typedef int key_type;
        typedef char value_type;
        typedef hash_utils::ClusterInventory ClusterInventory;
        struct Slot {
            key_type key;
            value_type value;
            bool is_occupied = false;
        };
        Slot* slots;
        size_t curr_capacity = 0;
        size_t num_occupied_slots = 0;
        size_t probe(size_t i) {
            return i;
        }
        size_t hash(key_type const& key) {
            size_t M = capacity();
            hash_utils::functors::primary_hashes::hash_basic hasher;
            size_t big_hash_number = hasher(key);
            size_t hash_val = big_hash_number % M;
            return hash_val;
        }
        /*
            searches the map for an item matching key. returns the number of probe attempts needed
            to reach either the item or an empty slot
        */
        int search_internal(key_type const& key) {
            size_t M = capacity();
            size_t hash_val = hash(key);
            size_t probes_required;
            for (probes_required = 0; probes_required != M; ++probes_required) {
                size_t slot_index = (hash_val + probe(probes_required)) % M;
                if (slots[slot_index].is_occupied) {
                    if (slots[slot_index].key == key) {
                        break;
                    }
                } else
                    //found unoccupied slot
                    break;
            }
            return probes_required;
        }
        //all backing array manipulations should go through the following two methods
        void insert_at_index(key_type const& key, value_type const& value, size_t index) {
            Slot& s = slots[index];
            s.key = key;
            s.value = value;
            if ( ! s.is_occupied) {
                s.is_occupied = true;
                ++num_occupied_slots;
            }
        }
        value_type remove_at_index(size_t index) {
            Slot& s = slots[index];
            if (s.is_occupied) {
                s.is_occupied = false;
                --num_occupied_slots;
            }
            return s.value;
        }
    public:
        HashMapOpenAddressing(size_t const min_capacity)
        {
            if (min_capacity == 0) {
                throw std::domain_error("min_capacity must be at least 1");
            }
            curr_capacity = 1 << static_cast<size_t>(std::ceil(lg(min_capacity))); //make capacity a power of 2, greater than the minimum capacity
            slots = new Slot[curr_capacity];
        }
        ~HashMapOpenAddressing() {
            delete slots;
        }
        /*
            if there is space available, adds the specified key/value-pair to the hash map and returns the
            number of probes required, P; otherwise returns -1 * P. If an item already exists in the map
            with the same key, replace its value.
        */
        int insert(key_type const& key, value_type const& value) {
            size_t M = capacity();
            if (capacity() == size())
                return -1 * size();
            size_t probes_required = search_internal(key);
            size_t index = (hash(key) + probe(probes_required)) % M;
            insert_at_index(key, value, index);
            return probes_required;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the map, stores it's value in
            value, and returns the number of probes required, P; otherwise returns -1 * P.
        */
        int remove(key_type const& key, value_type& value) {
            size_t M = capacity();
            size_t probes_required = search_internal(key);
            size_t index = (hash(key) + probe(probes_required)) % M;
            if (slots[index].key != key)
                //key not found
                return -1 * probes_required;
            value = remove_at_index(index);
            size_t start_index = index;
            //remove and reinsert items until find unoccupied slot
            for (int i = 1; ; ++i) {
                index = (start_index + probe(i)) % M;
                Slot const& s = slots[index];
                if (s.is_occupied) {
                    remove_at_index(index);
                    insert(s.key, s.value);
                } else {
                    break;
                }
            }
            return true;
        }
        /*
            if there is an item matching key, stores it's value in value, and returns the
            number of probes required, P; otherwise returns -1 * P. Regardless, the item
            remains in the map.
        */
        int search(key_type const& key, value_type& value) {
            size_t M = capacity();
            size_t probes_required = search_internal(key);
            size_t index = (hash(key) + probe(probes_required)) % M;
            if (slots[index].key != key)
                //key not found
                return -1 * probes_required;
            value = slots[index].value;
            return true;
        }
        /*
            removes all items from the map.
        */
        void clear() {
            size_t cap = capacity();
            for (size_t i = 0; i != cap; ++i)
                slots[i].is_occupied = false;
            num_occupied_slots = 0;
        }
        /*
            returns true IFF the map contains no elements.
        */
        bool is_empty() {
            return size() == 0;
        }
        /*
            returns the number of slots in the map.
        */
        size_t capacity() {
            return curr_capacity;
        }
        /*
            returns the number of items actually stored in the map.
        */
        size_t size() {
            return num_occupied_slots;
        }
        /*
            returns the map's load factor (size = load * capacity).
        */
        double load() {
            return static_cast<double>(size()) / capacity();
        }
        /*
            inserts into the ostream, the backing array's contents in sequential order.
            Empty slots shall be denoted by a hyphen, non-empty slots by that item's
            key. [This function will be used for debugging/monitoring].
        */
        std::ostream& print(std::ostream& out) {
            size_t cap = capacity();
            out << '[';
            for (size_t i = 0; i != cap; ++i) {
                if (slots[i].is_occupied) {
                    out << slots[i].key;
                } else {
                    out << "-";
                }
                if (i + 1 < cap)
                    out << '|';
            }
            out << ']';
            return out;
        }

        priority_queue<ClusterInventory> cluster_distribution() {
            //use an array to count cluster instances, then feed those to a priority queue and return it.
            size_t M = capacity();
            size_t cluster_counter[M + 1];
            for (size_t i = 0; i <= M; ++i)
                cluster_counter[i] = 0;
            if (size() == M) {
                //handle the special case when the map is full
                cluster_counter[size()]++;
            } else {
                //have at least one unoccupied slot
                bool first_cluster_skipped = false;
                size_t curr_cluster_size = 0;
                //treat the backing array as a circular buffer and make a maximum of two passes to
                //capture everything, including the wraparound cluster if it exists
                for (size_t i = 1; i != M * 2; ++i) {
                    Slot const& curr_slot = slots[i % M], prev_slot = slots[(i - 1) % M];
                    if (curr_slot.is_occupied && prev_slot.is_occupied) {
                        //still in a cluster
                        ++curr_cluster_size;
                    } else if (curr_slot.is_occupied && prev_slot.is_occupied == false) {
                        //found a new cluster
                        curr_cluster_size = 1;
                    } else if ( ! curr_slot.is_occupied && prev_slot.is_occupied) {
                        //found the end of a cluster
                        if (first_cluster_skipped) {
                            cluster_counter[curr_cluster_size]++;
                            if (i >= M) {
                                //reached the end of the first cluster in the second pass, so no all clusters have been handled
                                break;
                            }
                        } else {
                            first_cluster_skipped = true;
                        }
                    }
                }
            }
            priority_queue<ClusterInventory> cluster_pq;
            for (size_t i = 1; i <= M; ++i)
                if (cluster_counter[i] > 0) {
                    ClusterInventory cluster{i, cluster_counter[i]};
                    cluster_pq.add_to_queue(cluster);
                }
            return cluster_pq;
        }

        /*
            generate a random number, R, (1,size), and starting with slot zero in the backing array,
            find the R-th occupied slot; remove the item from that slot (adjusting subsequent items as
            necessary), and return its key.
        */
        key_type remove_random() {
            key_type empty;
            if (size() == 0) return empty;
            size_t num_slots = capacity();
            size_t ith_node_to_delete = 1 + hash_utils::rand_i(size());
            for (size_t i = 1; i <= num_slots; ++i) {
                Slot const& slot = slots[i];
                if (slot.is_occupied && --ith_node_to_delete == 0) {
                    key_type key = slot.key;
                    value_type val_dummy;
                    remove(key, val_dummy);
                    return key;
                    break;
                }
            }
            return empty;
        }
    };
}

#endif
