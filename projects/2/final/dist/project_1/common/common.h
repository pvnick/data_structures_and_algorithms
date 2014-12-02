#ifndef _COMMON_H_
#define _COMMON_H_

#include <string.h>
#include <limits>
#include <ostream>

namespace cop3530 {
    double lg(size_t i) {
        return std::log(i) / std::log(2);
    }

    namespace hash_utils {
        static constexpr size_t max_size_t = std::numeric_limits<size_t>::max();
        struct ClusterInventory {
            size_t cluster_size;
            size_t num_instances;
            struct cluster_size_less_predicate {
                bool operator()(ClusterInventory const& cluster1, ClusterInventory const& cluster2) {
                    return cluster1.cluster_size < cluster2.cluster_size;
                }
            };
        };
        size_t rand_i(size_t max) {
            size_t bucket_size = RAND_MAX / max;
            size_t num_buckets = RAND_MAX / bucket_size;
            size_t big_rand;
            do {
                    big_rand = rand();
            } while(big_rand >= num_buckets * bucket_size);
            return big_rand / bucket_size;
        }
        size_t str_to_numeric(const char* str) {
            unsigned int base = 257; //prime number chosen near an 8-bit character
            size_t numeric = 0;
            for (; *str != 0; ++str)
                numeric = numeric * base + *str;
            return numeric;
        }
        namespace functors {
            struct map_capacity_planner {
                size_t operator()(size_t min_capacity) {
                    //make capacity a power of 2, greater than the minimum capacity
                    return 1 << static_cast<size_t>(std::ceil(lg(min_capacity)));
                }
            };
            struct compare_functor {
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
                    size_t operator()(const char* key, size_t probe_attempt) const {
                        return 1;
                    }
                };
                struct quadratic_probe {
                    bool changes_with_probe_attempt() const {
                        return true;
                    }
                    size_t operator()(const char* key, size_t probe_attempt) const {
                        return probe_attempt;
                    }
                };
                struct hash_double {
                private:
                    size_t hash_numeric(size_t numeric) const {
                        size_t hash = numeric % 97; //simple modulus using a prime number (from algorithms in c++)
                        //the second hash may not be zero (will cause an infinite loop).
                        //also, hash must be relatively prime to map_capacity so that every slot can be hit.
                        //since map capacity is a power of two if we use the capacity planner functor,
                        //both properties are attainable by adding one to the hash if it is even (despite what my
                        //7th grade algebra teacher attempted to teach me, I stubbournly consider zero to be an even
                        //integer despite no formal training in number theory)
                        bool is_even = (hash & 1) == 0;
                        if (is_even)
                            ++hash;
                        return hash;
                    }
                public:
                    bool changes_with_probe_attempt() const {
                        return false;
                    }
                    size_t operator()(const char* key, size_t unused) const {
                        size_t numeric = str_to_numeric(key);
                        return hash_numeric(numeric);
                    }
                    size_t operator()(double key, size_t unused) const {
                        return hash_numeric(key);
                    }
                    size_t operator()(int key, size_t unused) const {
                        return hash_numeric(key);
                    }
                    size_t operator()(std::string key, size_t unused) const {
                        const char* c_key = key.c_str();
                        return operator()(c_key, unused);
                    }
                };
            }
        }
    }
}

std::ostream& operator<<(std::ostream& out, cop3530::hash_utils::ClusterInventory const& rhs) {
    out << "Cluster{size=" << rhs.cluster_size << ", instances=" << rhs.num_instances << "}";
    return out;
}

#endif
