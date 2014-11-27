#ifndef _COMMON_H_
#define _COMMON_H_

#include <string.h>
#include <limits>

namespace cop3530 {
    double lg(size_t i) {
        return std::log(i) / std::log(2);
    }

    static constexpr double phi = 1.61803398875;
    static constexpr double fib_hash_A = 1 / phi;
    namespace generic_hash_utils {
        inline bool is_equal(const char* a, const char* b) {
            return strcmp(a, b) == 0;
        }
        inline bool is_equal(double a, double b) {
            return a == b;
        }
        inline bool is_equal(std::string a, std::string b) {
            return a == b;
        }
        inline bool is_equal(int a, int b) {
            return a == b;
        }

        inline size_t to_numeric(const char* x) {
            //this function relies upon overflowing the size_t type
            unsigned int bits = 8;
            unsigned int base = 1 << bits;
            double numeric = 0;
            for (; *x != 0; ++x)
                numeric = numeric * base + *x;
            return numeric;
        }
        inline size_t to_numeric(double x) {
            //we take the part to the left of the decimal and add it to the value of
            //the right-of-demical part times the maximum size_t value
            static const size_t max_size_t = std::numeric_limits<size_t>::max();
            return static_cast<size_t>(x) + std::fmod(x, 1) * max_size_t;
        }
        inline size_t to_numeric(int x) {
            return static_cast<size_t>(x);
        }
        inline size_t to_numeric(std::string x) {
            return to_numeric(x.c_str());
        }
    }
}

#endif
