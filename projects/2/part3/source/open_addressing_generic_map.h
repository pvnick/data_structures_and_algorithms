#ifndef _HASHMAPOPENADDRESSINGGENERIC_H_
#define _HASHMAPOPENADDRESSINGGENERIC_H_

#include <iostream>
#include <string>
#include "../../common/common.h"
#include "../../common/priority_queue.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type,
             typename capacity_plan_functor = hash_utils::functors::map_capacity_planner,
             typename compare_functor = hash_utils::functors::compare_functor,
             typename primary_hash = hash_utils::functors::primary_hashes::hash_basic,
             typename secondary_hash = hash_utils::functors::secondary_hashes::hash_double>
    class HashMapOpenAddressingGeneric {
    private:
        typedef hash_utils::ClusterInventory ClusterInventory;
        class Key {
        private:
            key_type raw_key;
            compare_functor compare;
            primary_hash hasher1;
            secondary_hash hasher2;
            size_t hash1_val;
            size_t hash2_val;
            size_t old_map_capacity;
        public:
            bool operator==(Key const& rhs) const {
                return compare(raw_key, rhs.raw_key) == 0;
            }
            bool operator==(key_type const& rhs) const {
                return compare(raw_key, rhs) == 0;
            }
            bool operator!=(Key const& rhs) const {
                return ! operator==(rhs);
            }
            bool operator!=(key_type const& rhs) const {
                return ! operator==(rhs);
            }
            size_t hash(size_t map_capacity, size_t probe_attempt) const {
                size_t local_hash2_val;
                if (probe_attempt != 0 && hasher2.changes_with_probe_attempt())
                {
                    //if the hashing function value is dependent on the probe attempt
                    //(eg quadratic probing), then we need to retrieve the new value
                    local_hash2_val = hasher2(raw_key, probe_attempt);
                } else {
                    //otherwise we can just use the value we have stored
                    local_hash2_val = hash2_val;
                }
                return (hash1_val + probe_attempt * local_hash2_val) % map_capacity;
            }
            key_type const& raw() const {
                return raw_key;
            }
            void reset(key_type const& key) {
                raw_key = key;
                size_t base_probe_attempt = 0;
                hash1_val = hasher1(key);
                hash2_val = hasher2(key, base_probe_attempt);
            }
            explicit Key(key_type key) {
                reset(key);
            }
            Key() = default;
        };
        class Value {
        private:
            value_type raw_value;
        public:
            bool operator==(Value const& rhs) const {
                return compare(raw_value, rhs.raw_value);
            }
            bool operator==(value_type const& rhs) const {
                return compare(raw_value, rhs) == 0;
            }
            value_type const& raw() const {
                return raw_value;
            }
            explicit Value(value_type value): raw_value(value) {}
            Value() = default;
        };
        struct Item {
            Key key;
            Value value;
        };
        struct Slot {
            Item item;
            bool is_occupied = false;
        };
        Slot* slots;
        capacity_plan_functor choose_capacity;
        size_t curr_capacity = 0;
        size_t num_occupied_slots = 0;
        /*
            searches the map for an item matching key. returns the number of probe attempts needed
            to reach either the item or an empty slot
        */
        int search_internal(Key const& key) {
            size_t M = capacity();
            size_t probes_required;
            for (probes_required = 0; probes_required != M; ++probes_required) {
                size_t slot_index = key.hash(M, probes_required);
                if (slots[slot_index].is_occupied) {
                    if (slots[slot_index].item.key == key) {
                        //found the key
                        break;
                    }
                } else
                    //found unoccupied slot
                    break;
            }
            return probes_required;
        }

        //all backing array manipulations should go through the following two methods
        void insert_at_index(Key const& key, Value const& value, size_t index) {
            Slot& s = slots[index];
            s.item.key = key;
            s.item.value = value;
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
        HashMapOpenAddressingGeneric(size_t const min_capacity)
        {
            if (min_capacity == 0) {
                throw std::domain_error("min_capacity must be at least 1");
            }
            curr_capacity = choose_capacity(min_capacity);
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
            size_t probes_required = search_internal(k);
            size_t index = k.hash(M, probes_required);
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
            size_t probes_required = search_internal(k);
            size_t index = k.hash(M, probes_required);
            if (slots[index].is_occupied == false || slots[index].item.key != key)
                //key not found
                return -1 * probes_required;
            Value v = remove_at_index(index);
            value = v.raw();
            //remove and reinsert items until find unoccupied slot (guaranteed to happen since we just removed an item)
            for (int i = 1; ; ++i) {
                index = k.hash(M, i);
                Slot const& s = slots[index];
                if (s.is_occupied) {
                    remove_at_index(index);
                    insert(s.item.key.raw(), s.item.value.raw());
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
            size_t index = k.hash(M, probes_required);
            if (slots[index].is_occupied == false || slots[index].item.key != key)
                //key not found
                return -1 * probes_required;
            value = slots[index].item.value.raw();
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
            size_t ith_node_to_delete = 1 + hash_utils::rand_i(size());
            for (size_t i = 0; i != num_slots; ++i) {
                Slot const& slot = slots[i];
                if (slot.is_occupied && --ith_node_to_delete == 0) {
                    key_type key = slot.item.key.raw();
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
