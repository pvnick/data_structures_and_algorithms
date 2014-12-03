#ifndef _OPEN_ADDRESSING_MAP_H_
#define _OPEN_ADDRESSING_MAP_H_

#include <iostream>
#include "../../common/common.h"

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
            size_t probe_index;
            for (probe_index = 0; probe_index != M; ++probe_index) {
                size_t slot_index = (hash_val + probe(probe_index)) % M;
                if (slots[slot_index].is_occupied) {
                    if (slots[slot_index].key == key) {
                        //found the key
                        break;
                    }
                } else
                    //found unoccupied slot
                    break;
            }
            return 1 + probe_index; //start with a single probe when probe_index==0
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
            cop3530::hash_utils::functors::map_capacity_planner capacity_planner;
            curr_capacity = capacity_planner(min_capacity); //make capacity a power of 2, greater than the minimum capacity
            slots = new Slot[curr_capacity];
        }
        ~HashMapOpenAddressing() {
            delete slots;
        }
        /*
            if there is space available, adds the specified key/value-pair to the hash map and returns true; otherwise
            returns false. If an item already exists in the map with the same key, replace its value.
        */
        bool insert(key_type const& key, value_type const& value) {
            size_t M = capacity();
            if (M == size())
                return false;
            size_t probes_required = search_internal(key);
            size_t index = (hash(key) + probe(probes_required - 1)) % M;
            insert_at_index(key, value, index);
            return true;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the map, stores it's value in value,
            and returns true; otherwise returns false.
        */
        bool remove(key_type const& key, value_type& value) {
            size_t M = capacity();
            size_t probes_required = search_internal(key);
            size_t index = (hash(key) + probe(probes_required - 1)) % M;
            if (slots[index].key != key)
                //key not found
                return false;
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
            if there is an item matching key, stores it's value in value,
            and returns true (the item remains in the map); otherwise returns false.
        */
        bool search(key_type const& key, value_type& value) {
            size_t M = capacity();
            size_t probes_required = search_internal(key);
            size_t index = (hash(key) + probe(probes_required - 1)) % M;
            if (slots[index].key != key)
                //key not found
                return false;
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

    };
}

#endif
