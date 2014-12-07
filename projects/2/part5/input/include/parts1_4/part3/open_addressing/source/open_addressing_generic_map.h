#ifndef _HASHMAPOPENADDRESSINGGENERIC_H_
#define _HASHMAPOPENADDRESSINGGENERIC_H_

#include <iostream>
#include <string>
#include "../../../common/common.h"
#include "../../../common/priority_queue.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type,
             typename primary_hash = hash_utils::functors::primary_hashes::hash_basic,
             typename secondary_hash = hash_utils::functors::secondary_hashes::hash_double>
    class HashMapOpenAddressingGeneric {
    private:
        typedef hash_utils::ClusterInventory ClusterInventory;
        typedef hash_utils::Key<key_type, primary_hash, secondary_hash> Key;
        typedef hash_utils::Value<value_type> Value;
        struct Item {
            Key key;
            Value value;
        };
        struct Slot {
            Item item;
            bool is_occupied = false;
        };
        Slot* slots;
        size_t curr_capacity = 0;
        size_t num_occupied_slots = 0;
        /*
            searches the map for an item matching key. returns the number of probe attempts needed
            to reach either the item or an empty slot
        */
        int search_internal(Key const& key) {
            size_t M = capacity();
            size_t probe_index;
            for (probe_index = 0; probe_index != M; ++probe_index) {
                size_t slot_index = key.hash(M, probe_index);
                if (slots[slot_index].is_occupied) {
                    if (slots[slot_index].item.key == key) {
                        //found the key
                        break;
                    }
                } else
                    //found unoccupied slot
                    break;
            }
            return 1 + probe_index; //start with a single probe
        }

        //all backing array manipulations should go through the following two methods
        void insert_at_index(Key const& key, Value const& value, size_t index) {
            Slot& s = slots[index];
            s.item.key.reset(key.raw_copy());
            s.item.value.reset(value.raw_copy());
            if ( ! s.is_occupied) {
                s.is_occupied = true;
                ++num_occupied_slots;
            }
        }
        Value const& remove_at_index(size_t index) {
            Slot& s = slots[index];
            if (s.is_occupied) {
                s.is_occupied = false;
                --num_occupied_slots;
            }
            return s.item.value;
        }
    public:
        HashMapOpenAddressingGeneric(size_t const capacity): curr_capacity(capacity)
        {
            if (capacity == 0) {
                throw std::domain_error("capacity must be at least 1");
            }
            slots = new Slot[curr_capacity];
        }
        ~HashMapOpenAddressingGeneric() {
            delete[] slots;
        }

        /*
            if there is space available, adds the specified key/value-pair to the hash map and returns the
            number of probes required, P; otherwise returns -1 * P. If an item already exists in the map
            with the same key, replace its value.
        */
        int insert(key_type const& key, value_type const& value) {
            size_t M = capacity();
            if (M == size())
                return -1 * size();
            Key k(key);
            Value v(value);
            int probes_required = search_internal(k);
            size_t index = k.hash(M, probes_required - 1);
            if (slots[index].is_occupied == true && slots[index].item.key != k)
                //map is full and we're going to hit an infinite loop if we keep going
                return probes_required * -1;
            insert_at_index(k, v, index);
            return probes_required;
        }

        /*
            if there is an item matching key, removes the key/value-pair from the map, stores it's value in
            value, and returns the number of probes required, P; otherwise returns -1 * P.
        */
        int remove(key_type const& key, value_type& value) {
            size_t M = capacity();
            Key k(key);
            int probes_required = search_internal(k);
            size_t index = k.hash(M, probes_required - 1);
            if (slots[index].is_occupied == false || slots[index].item.key != k)
                //key not found
                return -1 * probes_required;
            Value v = remove_at_index(index);
            value = v.raw_copy();
            //remove and reinsert items until find unoccupied slot (guaranteed to happen since we just removed an item)
            for (int i = 1; ; ++i) {
                index = k.hash(M, i);
                Slot const& s = slots[index];
                if (s.is_occupied) {
                    remove_at_index(index);
                    insert(s.item.key.raw_copy(), s.item.value.raw_copy());
                } else {
                    break;
                }
            }
            return probes_required;
        }

        /*
            if there is an item matching key, stores it's value in value, and returns the
            number of probes required, P; otherwise returns -1 * P. Regardless, the item
            remains in the map.
        */
        int search(key_type const& key, value_type& value) {
            size_t M = capacity();
            Key k(key);
            size_t probes_required = search_internal(k);
            size_t index = k.hash(M, probes_required - 1);
            if (slots[index].is_occupied == false || slots[index].item.key != k)
                //key not found
                return -1 * probes_required;
            value = slots[index].item.value.raw_copy();
            return probes_required;
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
        bool is_empty() const {
            return size() == 0;
        }
        /*
            returns the number of slots in the map.
        */
        size_t capacity() const {
            return curr_capacity;
        }
        /*
            returns the number of items actually stored in the map.
        */
        size_t size() const {
            return num_occupied_slots;
        }
        /*
            returns the map's load factor (size = load * capacity).
        */
        double load() const {
            return static_cast<double>(size()) / capacity();
        }
        /*
            inserts into the ostream, the backing array's contents in sequential order.
            Empty slots shall be denoted by a hyphen, non-empty slots by that item's
            key. [This function will be used for debugging/monitoring].
        */
        std::ostream& print(std::ostream& out) const {
            size_t cap = capacity();
            out << '[';
            for (size_t i = 0; i != cap; ++i) {
                if (slots[i].is_occupied) {
                    out << slots[i].item.key.raw();
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
            priority_queue<ClusterInventory> cluster_pq;
            if (size() == 0) return cluster_pq;
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
            if (size() == 0) throw std::logic_error("Cant remove from an empty map");
            size_t num_slots = capacity();
            size_t ith_node_to_delete = 1 + rand_i(size());
            for (size_t i = 0; i != num_slots; ++i) {
                Slot const& slot = slots[i];
                if (slot.is_occupied && --ith_node_to_delete == 0) {
                    key_type key = slot.item.key.raw_copy();
                    value_type val_dummy;
                    remove(key, val_dummy);
                    return key;
                }
            }
            throw std::logic_error("Unexpected end of remove_random function");
        }
    };
}

#endif
