#ifndef _BUCKETS_MAP_H_
#define _BUCKETS_MAP_H_

#include <iostream>
#include "../../common/common.h"

namespace cop3530 {
    class HashMapBuckets {
    private:
        typedef int key_type;
        typedef char value_type;
        struct Item {
            key_type key;
            value_type value;
            Item* next;
            bool is_dummy;
            Item(Item* next, key_type const& key, value_type const& value): next(next), is_dummy(false) {}
            Item(Item* next): next(next), is_dummy(true) {}
        };
        struct Bucket {
            Item* head; //use a head pointer to the first node, and include a dummy node at the end (but dont store its pointer)
            Bucket() {
                Item* tail = new Item(nullptr);
                head = tail;
            }
            ~Bucket() {
                while ( ! head->is_dummy) {
                    Item* to_delete = head;
                    head = head->next;
                    delete to_delete;
                }
                delete head; //tail
            }
        };
        typedef Item* link;
        Bucket* buckets;
        size_t num_buckets = 0;
        size_t num_items = 0;
        size_t hash(key_type const& key) {
            size_t M = capacity();
            return std::floor(M * std::fmod(key * hash_utils::fib_hash_A, 1));
        }
        /*
            searches the bucket corresponding to the specified key's hash for that
            key. if found, stores a reference to that item and returns true.
            otherwise, returns false and stores the pointer to the tail dummy node in
            item_ptr.
        */
        bool search_internal(key_type const& key, link& item_ptr) {
            bool found_item = false;
            size_t hash_val = hash(key);
            Bucket& bucket = buckets[hash_val];
            item_ptr = bucket.head;
            while ( ! item_ptr->is_dummy) {
                if (item_ptr->key == key) {
                    found_item = true;
                    break;
                }
                item_ptr = item_ptr->next;
            }
            return found_item;
        }
        void init() {
            buckets = new Bucket[num_buckets];
        }
    public:
        HashMapBuckets(size_t const min_buckets)
        {
            if (min_buckets == 0) {
                throw std::domain_error("min_buckets must be at least 1");
            }
            num_buckets = 1 << static_cast<size_t>(std::ceil(lg(min_buckets))); //make capacity a power of 2, greater than the minimum capacity
            init();
        }
        ~HashMapBuckets() {
            delete[] buckets;
        }
        bool insert(key_type const& key, value_type const& value) {
            Item* item;
            if (search_internal(key, item))
                item->value = value;
            else {
                //currently holding tail. transform it into a valid item then add a new tail
                item->is_dummy = false;
                item->key = key;
                item->value = value;
                item->next = new Item(nullptr);
                ++num_items;
            }
            return true;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the
            map, stores it's value in value, and returns true; otherwise returns false.
        */
        bool remove(key_type const& key, value_type& value) {
            Item* item;
            if ( ! search_internal(key, item))
                //key not found
                return false;
            value = item->value;
            //swap the current item for the next one
            Item* to_delete = item->next;
            *item = *to_delete;
            delete to_delete;
            --num_items;
            return true;
        }
        /*
            if there is an item matching key, stores it's value in value, and
            returns true (the item remains in the map); otherwise returns false.
        */
        bool search(key_type const& key, value_type& value) {
            Item* item;
            if ( ! search_internal(key, item))
                //key not found
                return false;
            value = item->value;
            return true;
        }
        /*
            removes all items from the map.
        */
        void clear() {
            delete buckets;
            init();
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
            return num_buckets;
        }
        /*
            returns the number of items actually stored in the map.
        */
        size_t size() {
            return num_items;
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
            bool print_separator = false;
            out << '[';
            for (size_t i = 0; i != cap; ++i) {
                Bucket const& bucket = buckets[i];
                for (Item* item = bucket.head; item->is_dummy != false; item = item->next) {
                    if (print_separator)
                        out << "|";
                    else
                        print_separator = true;
                    out << item->key;
                }
            }
            out << ']';
            return out;
        }
    };
}

#endif
