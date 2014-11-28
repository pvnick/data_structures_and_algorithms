#ifndef _COMMON_H_
#define _COMMON_H_

#include <string.h>
#include <limits>

namespace cop3530 {
    double lg(size_t i) {
        return std::log(i) / std::log(2);
    }

    namespace hash_utils {
        static constexpr double phi = 1.61803398875;
        static constexpr double fib_hash_A = 1 / phi;

        namespace functors {
            struct equality_predicate {
                bool operator()(const char* a, const char* b) const {
                    return strcmp(a, b) == 0;
                }
                bool operator()(double a, double b) const {
                    return a == b;
                }
                bool operator()(std::string const& a, std::string const& b) const {
                    return a == b;
                }
                bool operator()(int a, int b) const {
                    return a == b;
                }
            };
            struct linear_probe {
                size_t operator()(const char* key, size_t map_capacity, size_t probe_attempt) const {
                    return 1;
                }
            };
            struct quadratic_probe {
                size_t operator()(const char* key, size_t map_capacity, size_t probe_attempt) const {
                    return probe_attempt;
                }
            };
            struct hash_fibonacci {
            private:
                size_t hash_numeric(double numeric, size_t map_capacity) const {
                    double fractional_hash = map_capacity * std::fmod(numeric * fib_hash_A, 1);
                    return static_cast<size_t>(fractional_hash);
                }
            public:
                size_t operator()(const char* key, size_t map_capacity, size_t unused) const {
                    unsigned int base = 257; //prime number chosen near an 8-bit character
                    size_t numeric = 0;
                    for (; *key != 0; ++key)
                        numeric = (numeric * base + *key) % map_capacity;
                    return hash_numeric(numeric, map_capacity);
                }
                size_t operator()(double key, size_t map_capacity, size_t unused) const {
                    return hash_numeric(key, map_capacity);
                }
                size_t operator()(int key, size_t map_capacity, size_t unused) const {
                    return hash_numeric(key, map_capacity);
                }
                size_t operator()(std::string const& key, size_t map_capacity, size_t unused) const {
                    const char* c_key = key.c_str();
                    return operator()(c_key, map_capacity, unused);
                }
            };
            struct hash_mid_bits {
            private:
                size_t hash_numeric(double numeric, size_t map_capacity) const {
                    unsigned int word_bits = 32;
                    size_t W = 1 << word_bits;
                    double coefficient = static_cast<double>(map_capacity) / W;
                    double hash = coefficient * std::fmod(numeric * numeric, W);
                    return static_cast<double>(hash);
                }
            public:
                size_t operator()(const char* key, size_t map_capacity, size_t unused) const {
                    unsigned int base = 257; //prime number chosen near an 8-bit character
                    size_t numeric = 0;
                    for (; *key != 0; ++key)
                        numeric = (numeric * base + *key) % map_capacity;
                    return hash_numeric(numeric, map_capacity);
                }
                size_t operator()(double key, size_t map_capacity, size_t unused) const {
                    return hash_numeric(key, map_capacity);
                }
                size_t operator()(int key, size_t map_capacity, size_t unused) const {
                    return hash_numeric(key, map_capacity);
                }
                size_t operator()(std::string key, size_t map_capacity, size_t unused) const {
                    const char* c_key = key.c_str();
                    return operator()(c_key, map_capacity, unused);
                }
            };
            struct map_capacity_planner {
                size_t operator()(size_t min_capacity) {
                    //make capacity a power of 2, greater than the minimum capacity
                    return 1 << static_cast<size_t>(std::ceil(lg(min_capacity)));
                }
            };
        }
    }
}

#endif
