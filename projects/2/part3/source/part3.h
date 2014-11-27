#ifndef _PART3_H_
#define _PART3_H_

#include <iostream>
#include <string>
#include "../../common/SSLL.h"
#include "../../common/common.h"

namespace cop3530 {
    template<typename key_type, typename value_type>
    class HashMapOpenAddressingGeneric {
    private:
        struct Key {
            key_type raw_key;
            size_t numeric_representation;
            bool operator==(Key const& rhs) const {
                return generic_hash_utils::is_equal(raw_key, rhs.raw_key);
            }
            bool operator==(key_type const& rhs) const {
                return generic_hash_utils::is_equal(raw_key, rhs);
            }
            size_t hash(size_t map_capacity) const {
                size_t M = map_capacity;
                return std::floor(M * std::fmod(numeric_representation * fib_hash_A, 1));
            }
            explicit Key(key_type key):
                raw_key(key),
                numeric_representation(generic_hash_utils::to_numeric(raw_key))
            {}
            Key() = default;
        };
        struct Value {
            value_type raw_value;
            bool operator==(Value const& rhs) const {
                return compare(raw_value, rhs.raw_value);
            }
            explicit Value(value_type value): raw_value(value) {}
            Value() = default;
        };
        struct Slot {
            Key key;
            Value value;
            bool is_occupied = false;
        };
        Slot* slots;
        size_t curr_capacity = 0;
        size_t num_occupied_slots = 0;
        size_t probe(size_t i) {
            //linear probing
            return 1;
        }
        size_t hash(key_type const& key) {
            return Key(key).hash(capacity());
        }
        /*
            if there is an item matching key, stores it's slot index in slot_index, and
            returns true (the item remains in the map). otherwise, returns false and,
            if there exists a free slot, stores the free slot's index in slot_index.
        */
        bool search_internal(Key const& key, size_t& slot_index) {
            bool found_item = false;
            size_t M = capacity();
            size_t hash_val = key.hash(M);
            for (size_t i = 0; i != M; ++i) {
                slot_index = (hash_val + i * probe(i)) % M;
                if (slots[slot_index].is_occupied) {
                    if (slots[slot_index].key == key) {
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
            s.key = key;
            s.value = value;
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
            return s.value;
        }
    public:
        HashMapOpenAddressingGeneric(size_t const min_capacity)
        {
            curr_capacity = 1 << static_cast<size_t>(std::ceil(lg(min_capacity))); //make capacity a power of 2, greater than the minimum capacity
            slots = new Slot[curr_capacity];
        }
        ~HashMapOpenAddressingGeneric() {
            delete slots;
        }
        /*
            if there is space available, adds the specified key/value-pair to the
            hash map and returns true; otherwise returns false. If an item already
            exists in the map with the same key, replace its value.
        */
        bool insert(key_type const& key, value_type const& value) {
            size_t index;
            if (capacity() == size())
                return false;
            Key k(key);
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
            Key k(key);
            if ( ! search_internal(k, index))
                //key not found
                return false;
            Value v = remove_at_index(index);
            value = v.raw_value;
            size_t start_index = index;
            size_t M = capacity();
            //remove and reinsert items until find unoccupied slot
            for (int i = 1; ; ++i) {
                index = (start_index + probe(i)) % M;
                Slot s = slots[index];
                if (s.is_occupied) {
                    remove_at_index(index);
                    insert(s.key.raw_key, s.value.raw_value);
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
            Key k(key);
            if ( ! search_internal(k, index))
                return false;
            Value v(slots[index].value);
            value = v.raw_value;
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
                    out << slots[i].key.raw_key;
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
