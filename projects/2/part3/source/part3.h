#ifndef _PART3_H_
#define _PART3_H_

#include <iostream>
#include <string>
#include "../../common/SSLL.h"
#include "../../common/common.h"
#include "../../common/priority_queue.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type,
             typename capacity_plan_functor = hash_utils::functors::map_capacity_planner,
             typename compare_functor = hash_utils::functors::compare_functor,
             typename primary_hash = hash_utils::functors::primary_hashes::hash_fibonacci,
             typename secondary_hash = hash_utils::functors::secondary_hashes::hash_double>
    class HashMapOpenAddressingGeneric {
    private:
        class Key {
        private:
            key_type raw_key;
            compare_functor compare;
            primary_hash hash1;
            secondary_hash hash2;
            size_t hash1_val;
            size_t hash2_val;
            size_t old_map_capacity;
        public:
            bool operator==(Key const& rhs) const {
                return compare(raw_key, rhs.raw_key) == 0;
            }
            size_t hash(size_t map_capacity, size_t probe_attempt) const {
                size_t local_hash2_val;
                if (map_capacity != old_map_capacity
                    || (probe_attempt != 0 && hash2.changes_with_probe_attempt()))
                {
                    //if the hashing function value is dependent on the probe attempt
                    //(eg quadratic probing), then we need to retrieve the new value
                    local_hash2_val = hash2(raw_key, map_capacity, probe_attempt);
                } else {
                    //otherwise we can just use the value we have stored
                    local_hash2_val = hash2_val;
                }
                return (hash1_val + probe_attempt * local_hash2_val) % map_capacity;
            }
            key_type const& raw() const {
                return raw_key;
            }
            void reset(key_type const& key, size_t map_capacity) {
                raw_key = key;
                old_map_capacity = map_capacity;
                size_t base_probe_attempt = 0;
                hash1_val = hash1(key, map_capacity);
                hash2_val = hash2(key, map_capacity, base_probe_attempt);
            }
            explicit Key(key_type key, size_t map_capacity) {
                reset(key, map_capacity);
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
            value_type const& raw() {
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
        size_t probe(size_t i) const {
            //linear probing
            return 1;
        }
        /*
            if there is an item matching key, stores it's slot index in slot_index, and
            returns the the number of probe attempts required (the item remains in the map).
            otherwise, returns the number of probe attempts requried to reach the encountered
            empty slot and, if there exists a free slot, stores the free slot's index in slot_index.
        */
        bool search_internal(Key const& key, size_t& slot_index) const {
            bool found_item = false;
            size_t M = capacity();
            for (size_t i = 0; i != M; ++i) {
                slot_index = key.hash(M, i);
                if (slots[slot_index].is_occupied) {
                    if (slots[slot_index].item.key == key) {
                        found_item = true;
                        break;
                    }
                } else
                    //found unoccupied slot
                    break;
            }
            return found_item;
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
            if there is space available, adds the specified key/value-pair
            to the hash map and returns the number of probes required, P;
            otherwise returns -1 * P. If an item already exists in the map
            with the same key, replace its value
        */
        bool insert(key_type const& key, value_type const& value) {
            size_t index;
            if (capacity() == size())
                return false;
            Key k(key, capacity());
            Value v(value);
            search_internal(k, index);
            insert_at_index(k, v, index);
            return true;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the
            map, stores it's value in value, and returns true; otherwise returns false.
        */
        bool remove(key_type const& key, value_type& value) {
            size_t index;
            Key k(key, capacity());
            if ( ! search_internal(k, index))
                //key not found
                return false;
            Value v = remove_at_index(index);
            value = v.raw();
            size_t start_index = index;
            size_t M = capacity();
            //remove and reinsert items until find unoccupied slot
            for (int i = 1; ; ++i) {
                index = (start_index + probe(i)) % M;
                Slot s = slots[index];
                if (s.is_occupied) {
                    remove_at_index(index);
                    insert(s.item.key.raw(), s.item.value.raw());
                } else {
                    break;
                }
            }
            return true;
        }
        /*
            if there is an item matching key, stores it's value in value, and
            returns true (the item remains in the map); otherwise returns false.
        */
        bool search(key_type const& key, value_type& value) {
            size_t index;
            Key k(key, capacity());
            if ( ! search_internal(k, index))
                return false;
            value = slots[index].item.value.raw();
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

        priority_queue<hash_utils::ClusterInventory> cluster_distribution() {
            SSLL<hash_utils::ClusterInventory> clusters;

        }
    };
}

#endif
