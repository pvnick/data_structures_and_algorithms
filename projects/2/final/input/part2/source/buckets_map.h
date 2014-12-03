#ifndef _BUCKETS_MAP_H_
#define _BUCKETS_MAP_H_

#include <iostream>
#include <stdexcept>
#include "../../common/common.h"

namespace cop3530 {
    class HashMapBuckets {
    private:
        typedef int key_type;
        typedef char value_type;
        typedef hash_utils::ClusterInventory ClusterInventory;
        struct Item {
            key_type key;
            value_type value;
            Item* next;
            bool is_dummy;
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
            hash_utils::functors::primary_hashes::hash_basic hasher;
            return hasher(key) % M;
        }
        /*
            searches the bucket corresponding to the specified key's hash for that
            key. if found, stores a reference to that item and returns P, the number of
            probe attempts needed to get to the item (ie the number of chain links needed
            to be traversed). otherwise return -1 * P and stores the pointer to the tail dummy node in
            item_ptr.
        */
        int search_internal(key_type const& key, link& item_ptr) {
            int probe_attempts = 1;
            size_t hash_val = hash(key);
            Bucket& bucket = buckets[hash_val];
            item_ptr = bucket.head;
            while ( ! item_ptr->is_dummy) {
                if (item_ptr->key == key) {
                    //found the key
                    return probe_attempts;
                }
                item_ptr = item_ptr->next;
                ++probe_attempts;
            }
            //key not found
            return probe_attempts * -1;
        }
        void init() {
            buckets = new Bucket[num_buckets];
            num_items = 0;
        }
    public:
        HashMapBuckets(size_t const min_buckets)
        {
            if (min_buckets == 0) {
                throw std::domain_error("min_buckets must be at least 1");
            }
            cop3530::hash_utils::functors::map_capacity_planner capacity_planner;
            num_buckets = capacity_planner(min_buckets); //make capacity prime
            init();
        }
        ~HashMapBuckets() {
            delete[] buckets;
        }
        /*
            if there is space available, adds the specified key/value-pair to the hash map and returns true; otherwise
            returns false. If an item already exists in the map with the same key, replace its value.
            note: this will never return false because we add to a linked list to resolve collisions
        */
        bool insert(key_type const& key, value_type const& value) {
            Item* item;
            int probes_required = search_internal(key, item);
            if (probes_required > 0)
                //found item
                item->value = value;
            else {
                //currently holding tail (item not found). transform it into a valid item then add a new tail
                item->is_dummy = false;
                item->key = key;
                item->value = value;
                item->next = new Item(nullptr);
                ++num_items;
            }
            return true;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the map, stores it's
            value in value, and returns true; otherwise returns false.
        */
        bool remove(key_type const& key, value_type& value) {
            Item* item;
            int probes_required = search_internal(key, item);
            if (probes_required > 0) {
                //found item
                value = item->value;
                //swap the current item for the next one
                Item* to_delete = item->next;
                *item = *to_delete;
                delete to_delete;
                --num_items;
                return true;
            }
            return false;
        }
        /*
            if there is an item matching key, stores it's value in value, and returns true (the
            item remains in the map); otherwise returns false.
        */
        bool search(key_type const& key, value_type& value) {
            Item* item;
            int probes_required = search_internal(key, item);
            if (probes_required > 0) {
                //found item
                value = item->value;
                return true;
            }
            return false;
        }
        /*
            removes all items from the map.
        */
        void clear() {
            delete[] buckets;
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
            returns the map's load factor (occupied buckets = load * capacity).
        */
        double load() {
            size_t occupied_buckets = 0;
            if (size() > 0) {
                size_t M = capacity();
                for (size_t i = 0; i != M; ++i) {
                    Bucket const& bucket = buckets[i];
                    if ( ! bucket.head->is_dummy)
                        //bucket has at least one item
                        occupied_buckets++;
                }
            }
            return static_cast<double>(occupied_buckets) / capacity();
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
                if (bucket.head->is_dummy) {
                    if (print_separator)
                        out << "|";
                    else
                        print_separator = true;
                    out << "-";
                } else {
                    for (Item* item = bucket.head; item->is_dummy != true; item = item->next) {
                        if (print_separator)
                            out << "|";
                        else
                            print_separator = true;
                        out << item->key;
                    }
                } 
            }
            out << ']';
            return out;
        }
    };
}

#endif
