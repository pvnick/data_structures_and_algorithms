#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef _DEBUG_
    //various internal integrity checks that can be expensive, we want to disable them in production
    #define _DEBUG_ false
#endif

#include <string.h>
#include <limits>
#include <stdexcept>
#include <ostream>
#include <cmath>

namespace cop3530 {
    inline double lg(size_t i) {
        return std::log(i) / std::log(2);
    }
    inline size_t rand_i(size_t max) {
        size_t bucket_size = RAND_MAX / max;
        size_t num_buckets = RAND_MAX / bucket_size;
        size_t big_rand;
        do {
                big_rand = rand();
        } while(big_rand >= num_buckets * bucket_size);
        return big_rand / bucket_size;
    }

    namespace hash_utils {
        static constexpr size_t max_size_t = std::numeric_limits<size_t>::max();
        static constexpr size_t primes[] = { //from algorithms in c++, helps us to choose a prime-number map capacity
            251,
            509,
            1021,
            2039,
            4093,
            8191,
            16381,
            32749,
            65521,
            131071,
            262193,
            524287,
            1048573,
            2097143,
            4194301,
            8388593,
            16777213,
            33554393,
            67108859,
            134217689,
            268435399,
            536870909,
            1073741789,
            2147483647
        };
        struct ClusterInventory {
            size_t cluster_size;
            size_t num_instances;
            struct cluster_size_less_predicate {
                bool operator()(ClusterInventory const& cluster1, ClusterInventory const& cluster2) {
                    return cluster1.cluster_size < cluster2.cluster_size;
                }
            };
        };

        inline size_t str_to_numeric(const char* str) {
            unsigned int base = 257; //prime number chosen near an 8-bit character
            size_t numeric = 0;
            for (; *str != 0; ++str)
                numeric = numeric * base + *str;
            return numeric;
        }
        namespace functors {

            struct map_capacity_planner {
                size_t operator()(size_t min_capacity) {
                    for (int i = 0; i != 24; ++i)
                        if (min_capacity < primes[i])
                            return primes[i];
                    throw std::domain_error("Provided min capacity too large. Consider extending the list of prime numbers");
                }
            };
            struct compare {
                int operator()(const char* a, const char* b) const {
                    int cmp = strcmp(a, b);
                    return (cmp < 0 ? -1 :
                                      (cmp > 0 ? 1 : 0));
                }
                int operator()(double a, double b) const {
                    return (a < b ? -1 :
                                    (a > b ? 1 : 0));
                }
                int operator()(std::string const& a, std::string const& b) const {
                    return (a < b ? -1 :
                                    (a > b ? 1 : 0));
                }
                int operator()(int a, int b) const {
                    return (a < b ? -1 :
                                    (a > b ? 1 : 0));
                }
            };
            namespace primary_hashes {
                struct hash_basic {
                //this is such a stupid hash method, but unlike my pathetic attempts at implementing
                //various other hashing methods, it works and is generalizable to all the required key
                //types. together with double hashing it should make for a passable hashing routine.
                public:
                    size_t operator()(const char* key) const {
                        return str_to_numeric(key);
                    }
                    size_t operator()(double key) const {
                        return static_cast<size_t>(std::fmod(key, max_size_t));
                    }
                    size_t operator()(int key) const {
                        return static_cast<size_t>(key);
                    }
                    size_t operator()(std::string const& key) const {
                        const char* c_key = key.c_str();
                        return operator()(c_key);
                    }
                };
            }
            namespace secondary_hashes {
                struct linear_probe {
                    bool changes_with_probe_attempt() const {
                        return false;
                    }
                    template<typename T>
                    size_t operator()(T unused, size_t probe_attempt) const {
                        return 1;
                    }
                };
                struct quadratic_probe {
                    bool changes_with_probe_attempt() const {
                        return true;
                    }
                    template<typename T>
                    size_t operator()(T unused, size_t probe_attempt) const {
                        return probe_attempt;
                    }
                };
                struct hash_double {
                private:
                    size_t hash_numeric(size_t numeric) const {
                        size_t hash = numeric % 97; //simple modulus using a prime number (from algorithms in c++)
                        //the second hash may not be zero (will cause an infinite loop).
                        //also, hash must be relatively prime to map_capacity so that every slot can be hit.
                        //map capacity is a prime number based chosen from the table, so any value less than
                        //map capacity should work
                        return hash;
                    }
                public:
                    bool changes_with_probe_attempt() const {
                        return false;
                    }
                    size_t operator()(const char* key, size_t) const {
                        size_t numeric = str_to_numeric(key);
                        return hash_numeric(numeric);
                    }
                    size_t operator()(double key, size_t) const {
                        return hash_numeric(key);
                    }
                    size_t operator()(int key, size_t) const {
                        return hash_numeric(key);
                    }
                    size_t operator()(std::string key, size_t) const {
                        const char* c_key = key.c_str();
                        return operator()(c_key, 0);
                    }
                };
            }
        }

        template<typename T>
        class GenericContainer {
            /*
                for the types we need to support other than const char* (ie int, double, and std::string),
                we can pass these around willy-nilly. for const char*, handled below, we will obtain our
                own copy of the character array by wrapping it in a std::string
            */
        private:
            T raw;
            functors::compare compare;
        public:
            GenericContainer(const T& val): raw(val) {}
            GenericContainer() = default;
            GenericContainer& operator=(GenericContainer const& rhs) = delete;
            T operator()() const {
                return raw;
            }
            T copy() const {
                return raw;
            }
            void reset(const T& val) {
                raw = val;
            }
            int compare_to(GenericContainer const& other) const {
                return compare(raw, other.raw);
            }
        };
        template<>
        class GenericContainer<const char*> {
            /*
                class template specialization for character arrays, stores a local copy of the character array
            */
        private:
            char* raw = nullptr;
            functors::compare compare;
        public:
            GenericContainer(const char* val) {
                reset(val);
            }
            GenericContainer() = default;
            const char* operator()() const {
                return raw;
            }
            const char* copy() const {
                if (raw == nullptr) return nullptr;
                size_t len = strlen(raw);
                char* new_str = new char[len + 1];
                strncpy(new_str, raw, len);
                new_str[len] = 0;
                return new_str;
            }
            void reset(const char* val) {
                if (raw) {
                    delete raw;
                    raw = nullptr;
                }
                if (val != nullptr) {
                    size_t len = strlen(val);
                    raw = new char[len + 1];
                    strncpy(raw, val, len);
                    raw[len] = 0;
                }
            }
            int compare_to(GenericContainer const& other) const {
                return compare(raw, other.raw);
            }
        };

        template<typename key_type,
                 typename primary_hash = hash_utils::functors::primary_hashes::hash_basic,
                 typename secondary_hash = hash_utils::functors::secondary_hashes::hash_double>
        class Key {
        private:
            GenericContainer<key_type> raw_key;
            primary_hash hasher1;
            secondary_hash hasher2;
            size_t hash1_val;
            size_t hash2_val;
        public:
            Key& operator=(Key const& rhs) {
                if (&rhs == this)
                    return *this;
                reset(rhs.raw_key());
            }
            bool operator==(Key const& rhs) const {
                return raw_key.compare_to(rhs.raw_key) == 0;
            }
            bool operator<(Key const& rhs) const {
                return raw_key.compare_to(rhs.raw_key) == -1;
            }
            bool operator>(Key const& rhs) const {
                return raw_key.compare_to(rhs.raw_key) == 1;
            }
            bool operator!=(Key const& rhs) const {
                return ! operator==(rhs);
            }
            size_t hash(size_t map_capacity, size_t probe_attempt) const {
                size_t local_hash2_val;
                if (probe_attempt != 0 && hasher2.changes_with_probe_attempt())
                {
                    //if the hashing function value is dependent on the probe attempt
                    //(eg quadratic probing), then we need to retrieve the new value*/
                    local_hash2_val = hasher2(raw_key(), probe_attempt);
                } else {
                    //otherwise we can just use the value we have stored
                    local_hash2_val = hash2_val;
                }
                return (hash1_val + probe_attempt * local_hash2_val) % map_capacity;
            }
            key_type raw() const {
                return raw_key();
            }
            key_type raw_copy() const {
                //this is what is returned to the client, who is responsible for deleting it if its, eg a pointer to a character array
                return raw_key.copy();
            }
            template<typename T>
            void reset(T key) {
                raw_key.reset(key);
                size_t base_probe_attempt = 0;
                hash1_val = hasher1(key);
                hash2_val = hasher2(key, base_probe_attempt);
            }
            void reset(const char* key) {
                raw_key.reset(key);
                if (key != nullptr) {
                    size_t base_probe_attempt = 0;
                    hash1_val = hasher1(key);
                    hash2_val = hasher2(key, base_probe_attempt);
                }
            }
            explicit Key(key_type const& key): raw_key(key) {
                reset(key);
            }
            Key() = default;
        };
        template <typename value_type>
        class Value {
        private:
            functors::compare compare;
            GenericContainer<value_type> raw_value;
        public:
            Value& operator=(Value const& rhs) {
                if (&rhs == this)
                    return *this;
                reset(rhs.raw_value());
            }
            bool operator==(Value const& rhs) const {
                return compare(raw_value(), rhs.raw_value());
            }
            bool operator==(value_type const& rhs) const {
                return compare(raw_value(), rhs) == 0;
            }
            value_type raw() const {
                return raw_value();
            }
            value_type raw_copy() const {
                //this is what is returned to the client, who is responsible for deleting it if its, eg a pointer to a character array
                return raw_value.copy();
            }
            void reset(value_type value) {
                raw_value.reset(value);
            }
            explicit Value(value_type const& value): raw_value(value) {}
            Value() = default;
        };
    }
}

#endif
