#ifndef _BUCKETS_MAP_GENERIC_H_
#define _BUCKETS_MAP_GENERIC_H_

#include <iostream>
#include "../../../common/common.h"
#include "../../../common/SSLL.h"
#include "../../../common/priority_queue.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type,
             typename capacity_plan_functor = hash_utils::functors::map_capacity_planner,
             typename compare_functor = hash_utils::functors::compare_functor,
             typename primary_hash = hash_utils::functors::primary_hashes::hash_basic,
             typename secondary_hash = hash_utils::functors::secondary_hashes::hash_double>
    class HashMapBucketsGeneric {
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
            Item* next;
            bool is_dummy;
            explicit Item(Item* next): next(next), is_dummy(true) {}
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
        /*
            searches the bucket corresponding to the specified key's hash for that
            key. if found, stores a reference to that item and returns P, the number of
            probe attempts needed to get to the item (ie the number of chain links needed
            to be traversed). otherwise return -1 * P and stores the pointer to the tail dummy node in
            item_ptr.
        */
        int search_internal(Key const& key, link& item_ptr) {
            int probe_attempts = 1;
            size_t hash_val = key.hash(capacity(), 0);
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
        HashMapBucketsGeneric(size_t const min_buckets)
        {
            if (min_buckets == 0) {
                throw std::domain_error("min_buckets must be at least 1");
            }
            cop3530::hash_utils::functors::map_capacity_planner capacity_planner;
            num_buckets = capacity_planner(min_buckets); //make capacity a power of 2, greater than the minimum capacity
            init();
        }
        ~HashMapBucketsGeneric() {
            delete[] buckets;
        }
        /*
            if there is space available, adds the specified key/value-pair to the hash map and returns the
            number of probes required, P; otherwise returns -1 * P (that's a lie: we will always have space
            available because each bucket contains a linked list that is indefinitely growable). If an item
            already exists in the map with the same key, replace its value.
        */
        int insert(key_type const& key, value_type const& value) {
            Item* item;
            Key k(key);
            Value v(value);
            int probes_required = search_internal(k, item);
            if (probes_required > 0)
                //found item
                item->value = v;
            else {
                //currently holding tail (item not found). transform it into a valid item then add a new tail
                item->is_dummy = false;
                item->key = k;
                item->value = v;
                item->next = new Item(nullptr);
                ++num_items;
            }
            return std::abs(probes_required);
        }
        /*
            if there is an item matching key, removes the key/value-pair from the map, stores it's value in
            value, and returns the number of probes required, P; otherwise returns -1 * P.
        */
        int remove(key_type const& key, value_type& value) {
            Key k(key);
            Item* item;
            int probes_required = search_internal(key, item);
            if (probes_required > 0) {
                //found item
                value = item->value.raw();
                //swap the current item for the next one
                Item* to_delete = item->next;
                *item = *to_delete;
                delete to_delete;
                --num_items;
            }
            return probes_required;
        }
        /*
            if there is an item matching key, stores it's value in value, and returns the
            number of probes required, P; otherwise returns -1 * P. Regardless, the item
            remains in the map.
        */
        int search(key_type const& key, value_type& value) {
            Item* item;
            Key k(key);
            int probes_required = search_internal(k, item);
            if (probes_required > 0) {
                //found item
                value = item->value.raw();
            }
            return probes_required;
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
                for (Item* item = bucket.head; item->is_dummy != true; item = item->next) {
                    if (print_separator)
                        out << "|";
                    else
                        print_separator = true;
                    out << item->key.raw();
                }
            }
            out << ']';
            return out;
        }

        /*
            returns a priority queue containing cluster sizes and instances (in the form of ClusterInventory
            struct instances), sorted by cluster size.
        */
        priority_queue<ClusterInventory> cluster_distribution() {
            //use a simple linked list to count cluster instances, then feed those to a priority queue and return it.
            priority_queue<ClusterInventory> cluster_pq;
            if (size() == 0) return cluster_pq;
            SSLL<ClusterInventory> clusters;
            size_t M = capacity();
            for (size_t i = 0; i != M; ++i) {
                Bucket const& bucket = buckets[i];
                size_t bucket_size = 0;
                Item* item_ptr = bucket.head;
                while ( ! item_ptr->is_dummy) {
                    ++bucket_size;
                    item_ptr = item_ptr->next;
                }
                //I don't love this O(N^2) implementation, but premature optimization is the root of all evil and late projects
                SSLL<ClusterInventory>::iterator cluster_iterator = clusters.begin();
                SSLL<ClusterInventory>::iterator cluster_iterator_end = clusters.end();
                bool found_cluster = false;
                for (; cluster_iterator != cluster_iterator_end; ++cluster_iterator) {
                    if (cluster_iterator->cluster_size == bucket_size) {
                        found_cluster = true;
                        break;
                    }
                }
                if (found_cluster)
                    cluster_iterator->num_instances++;
                else
                    clusters.push_back({bucket_size, 1});
            }
            SSLL<ClusterInventory>::const_iterator cluster_iterator = clusters.begin();
            SSLL<ClusterInventory>::const_iterator cluster_iterator_end = clusters.end();
            for (; cluster_iterator != cluster_iterator_end; ++cluster_iterator) {
                if (cluster_iterator->cluster_size > 0)
                    cluster_pq.add_to_queue(*cluster_iterator);
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
                Bucket const& bucket = buckets[i];
                Item* item_ptr = bucket.head;
                while ( ! item_ptr->is_dummy) {
                    if (--ith_node_to_delete == 0) {
                        key_type key = item_ptr->key.raw();
                        value_type val_dummy;
                        remove(key, val_dummy);
                        return key;
                    }
                    item_ptr = item_ptr->next;
                }
            }
            throw std::logic_error("Unexpected end of remove_random function");
        }
    };
}

#endif
